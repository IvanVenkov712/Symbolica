#include "ShuntingYard.h"

std::string trimStr(const std::string& str)
{
	constexpr const char* wSpaces = " \n\r\t\v\f";
	size_t beg = str.find_first_not_of(wSpaces);
	if (beg == std::string::npos) {
		return "";
	}
	size_t end = str.find_last_not_of(wSpaces);
	return str.substr(beg, end - beg + 1);
}

bool isDigit(char ch) {
	return ch >= '0' && ch <= '9';
}

bool isSpecialSymbol(char ch)
{
	return ch == '+' || ch == '-' ||
		ch == '*' || ch == '/' ||
		ch == '^' ||
		ch == '(' || ch == ')' ||
		ch == ',';
}

bool isKnownConstant(const std::string& str)
{
	return str == Constants::piName || str == Constants::eName;
}

double getKnownConstant(const std::string& constantName)
{
	if (constantName == Constants::piName) {
		return Constants::_pi;
	}
	else if (constantName == Constants::eName) {
		return Constants::_e;
	}
	throw std::invalid_argument("Not a known constant!");
}

bool isNumber(const std::string& str)
{
	if (str.size() == 0) {
		return false;
	}
	std::string::const_iterator it = str.cbegin();
	if (*it == '-') {
		++it;
	}
	bool hasDecimal = false;
	if (it == str.cend() || *it == '.') {
		return false;
	}
	while (it != str.cend()) {
		if (*it == '.') {
			if (!hasDecimal) {
				if (it + 1 == str.cend()) {
					return false;
				}
				hasDecimal = true;
			}
			else return false;
		}
		else if (!isDigit(*it)) {
			return false;
		}
		++it;
	}
	return true;
}

double toNumber(const std::string& strNum)
{
	std::istringstream iss(strNum.c_str());
	double res;
	iss >> res;
	return res;
}

bool isVariable(const std::string& str)
{
	return str.size() == 1 && str[0] >= 'a' && str[0] <= 'z';
}

bool isFunction(const std::string& str)
{
	return str == Constants::sinName || str == Constants::cosName ||
		str == Constants::tgName || str == Constants::cotgName ||
		str == Constants::lnName || str == Constants::sqrtName;
}

bool isUnaryOperator(const std::string& str)
{
	return str == "neg";
}

bool isBinaryOperator(const std::string& str) {
	if (str.size() != 1) {
		return false;
	}
	char ch = str[0];
	return ch == '+' || ch == '-' ||
		ch == '*' || ch == '/' ||
		ch == '^';
}

bool isOperator(const std::string& str)
{
	return isUnaryOperator(str) || isBinaryOperator(str);
}

int getPrecedence(const std::string& str) {
	if (str == "+" || str == "-") {
		return 1;
	}
	else if (str == "*" || str == "/") {
		return 3;
	}
	else if (str == "^" || str == "neg") {
		return 4;
	}
	throw std::invalid_argument("str is not an operator!");
}

bool isLeftAssociative(const std::string& str) {
	if (str == "+" || str == "-" || str == "*" || str == "/") {
		return true;
	}
	else if (str == "^" || str == "neg") {
		return false;
	}
	throw std::invalid_argument("ch is not an operator!");
}

std::vector<std::string> convertToRPN(const std::vector<std::string>& tokens) {
	std::vector<std::string> output;
	std::stack<std::string> operatorStack;
	for (const std::string token : tokens) {
		if (isVariable(token) || isNumber(token) || isKnownConstant(token)) {
			output.push_back(token);
		}
		else if (isFunction(token)) {
			operatorStack.push(token);
		}
		else if (isOperator(token)) {
			while (!operatorStack.empty() && operatorStack.top() != "(" &&
				(getPrecedence(operatorStack.top()) > getPrecedence(token) ||
					(getPrecedence(operatorStack.top()) == getPrecedence(token)
						&& isLeftAssociative(operatorStack.top()))))
			{
				output.push_back(operatorStack.top());
				operatorStack.pop();
			}
			operatorStack.push(token);
		}
		else if (token == ",") {
			while (!operatorStack.empty() && operatorStack.top() != "(") {
				output.push_back(operatorStack.top());
				operatorStack.pop();
				if (operatorStack.empty()) {
					throw std::invalid_argument("Not a valid expression");
				}
			}
		}
		else if (token == "(") {
			operatorStack.push(token);
		}
		else if (token == ")") {
			while (!operatorStack.empty() && operatorStack.top() != "(") {
				output.push_back(operatorStack.top());
				operatorStack.pop();
				if (operatorStack.empty()) {
					throw std::invalid_argument("Not a valid expression");
				}
			}
			operatorStack.pop();
			if (!operatorStack.empty() && isFunction(operatorStack.top())) {
				output.push_back(operatorStack.top());
				operatorStack.pop();
			}
		}
	}
	while (!operatorStack.empty()) {
		if (operatorStack.top() == "(") {
			throw std::invalid_argument("Not a valid expression");
		}
		output.push_back(operatorStack.top());
		operatorStack.pop();
	}
	return output;
}

poly_obj_handler<MathExpression> executeBinaryOperation(poly_obj_handler<MathExpression>&& arg1, poly_obj_handler<MathExpression>&& arg2, char op)
{
	switch (op) {
	case '+':
		return new Addition(std::move(arg1), std::move(arg2));
	case '-':
		return new Substraction(std::move(arg1), std::move(arg2));
	case '*':
		return new Multiplication(std::move(arg1), std::move(arg2));
	case '/':
		return new Division(std::move(arg1), std::move(arg2));
	case '^':
		return new Power(std::move(arg1), std::move(arg2));
	default:
		throw std::invalid_argument("op is not a valid operator!");
	}
}

poly_obj_handler<MathExpression> executeBinaryOperation(poly_obj_handler<MathExpression>&& arg1, poly_obj_handler<MathExpression>&& arg2, const std::string& op)
{
	if (op.size() != 1) {
		throw std::invalid_argument("str is not an operator!");
	}
	return executeBinaryOperation(std::move(arg1), std::move(arg2), op[0]);
}

poly_obj_handler<MathExpression> executeUnaryOperation(poly_obj_handler<MathExpression>&& arg, const std::string& op)
{
	if (op == "neg") {
		return new Negation(std::move(arg));
	}
	else {
		throw std::invalid_argument("op is not an unary operator!");
	}
}

poly_obj_handler<MathExpression> executeFunction(poly_obj_handler<MathExpression>&& arg, const std::string& funName)
{
	if (funName == Constants::sinName) {
		return new Sine(std::move(arg));
	}
	else if (funName == Constants::cosName) {
		return new Cosine(std::move(arg));
	}
	else if (funName == Constants::tgName) {
		return new Tangent(std::move(arg));
	}
	else if (funName == Constants::cotgName) {
		return new Cotangent(std::move(arg));
	}
	else if (funName == Constants::lnName) {
		return new NaturalLogarithm(std::move(arg));
	}
	else if (funName == Constants::sqrtName) {
		return new SquareRoot(std::move(arg));
	}
	else {
		throw std::invalid_argument("Unrecognised function!");
	}
}

std::vector<std::string> tokenize(const std::string& expression)
{
	std::vector<std::string> res;
	std::string curr;
	for (char ch : expression) {
		if (ch == '-') {
			if (!curr.empty()) {
				res.push_back(trimStr(curr));
				curr = "";
			}
			if (res.size() == 0 || res.back() == "(" || isBinaryOperator(res.back()) || res.back() == "-") {
				res.push_back("neg");
			}
			else {
				res.push_back("-");
			}
		}
		else if (isSpecialSymbol(ch)) {
			if (!curr.empty()) {
				res.push_back(trimStr(curr));
				curr = "";
			}
			res.push_back(std::string(&ch, 1));
		}
		else {
			curr += ch;
		}
	}
	if (!curr.empty()) {
		res.push_back(trimStr(curr));
	}
	return res;
}

poly_obj_handler<MathExpression> popArg(std::stack<poly_obj_handler<MathExpression>>& argStack)
{
	if (argStack.empty()) {
		throw std::invalid_argument("Invalid expression!");
	}
	poly_obj_handler<MathExpression> arg = std::move(argStack.top());
	argStack.pop();
	return arg;
}

poly_obj_handler<MathExpression> calculatePRN(const std::vector<std::string>& rpnVector) {
	std::stack<poly_obj_handler<MathExpression>> resStack;
	for (size_t i = 0; i < rpnVector.size(); ++i) {
		if (isKnownConstant(rpnVector[i])) {
			resStack.push(new Constant(getKnownConstant(rpnVector[i])));
		}
		else if (isNumber(rpnVector[i])) {
			resStack.push(new Constant(toNumber(rpnVector[i])));
		}
		else if (isVariable(rpnVector[i])) {
			resStack.push(new Variable(rpnVector[i][0]));
		}
		else if (isUnaryOperator(rpnVector[i])) {
			poly_obj_handler<MathExpression> arg = popArg(resStack);
			poly_obj_handler<MathExpression> res = executeUnaryOperation(std::move(arg), rpnVector[i]);
			resStack.push(std::move(res));
		}
		else if (isBinaryOperator(rpnVector[i])) {
			poly_obj_handler<MathExpression> arg2 = popArg(resStack);
			poly_obj_handler<MathExpression> arg1 = popArg(resStack);
			poly_obj_handler<MathExpression> res = executeBinaryOperation(std::move(arg1), std::move(arg2), rpnVector[i]);
			resStack.push(std::move(res));
		}
		else if (isFunction(rpnVector[i])) {
			poly_obj_handler<MathExpression> arg = popArg(resStack);
			poly_obj_handler<MathExpression> res = executeFunction(std::move(arg), rpnVector[i]);
			resStack.push(std::move(res));
		}
	}
	if (resStack.size() != 1) {
		throw std::invalid_argument("Invalid expression!");
	}
	return resStack.top();
}
