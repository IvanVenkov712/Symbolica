#include "expressions/MathExpression.h"

MathExpression* Multiplication::clone() const
{
	return new Multiplication(operand1, operand2);
}

MathExpression::NumericType Multiplication::eval(const VariableInterpretation& vi) const
{
	return operand1->eval(vi) * operand2->eval(vi);
}

void Multiplication::print(std::ostream& os) const
{
	if (operand1->getPriority() < getPriority()) {
		os << "(";
		operand1->print(os);
		os << ")";
	}
	else {
		operand1->print(os);
	}

	os << "*";

	if (operand2->getPriority() < getPriority()) {
		os << "(";
		operand2->print(os);
		os << ")";
	}
	else {
		operand2->print(os);
	}
}

MathExpressionHandler Multiplication::differentiate(char var) const
{
	return new Addition(
		new Multiplication(operand1->differentiate(var), operand2),
		new Multiplication(operand1, operand2->differentiate(var))
	);
}

int Multiplication::getPriority() const
{
	return 2;
}

bool Multiplication::isLeftAssociative() const
{
	return true;
}

MathExpressionHandler Multiplication::simplify() const
{
	MathExpressionHandler newOperand1 = operand1->simplify();
	MathExpressionHandler newOperand2 = operand2->simplify();
	if (!newOperand1->hasVariables() && !newOperand2->hasVariables()) {
		return new Constant(newOperand1->eval(VariableInterpretation::Empty) * newOperand2->eval(VariableInterpretation::Empty));
	}
	else if (!newOperand1->hasVariables()) {
		double val = newOperand1->eval(VariableInterpretation::Empty);
		if (areEqual(val, 0)) {
			return new Constant(0);
		}
		else if (areEqual(val, 1)) {
			return newOperand2;
		}
		else if (areEqual(val, -1)) {
			return new Negation(std::move(newOperand2));
		}
		else {
			return new Multiplication(std::move(newOperand1), std::move(newOperand2));
		}
	}
	else if (!newOperand2->hasVariables()) {
		double val = newOperand2->eval(VariableInterpretation::Empty);
		if (areEqual(val, 0)) {
			return new Constant(0);
		}
		else if (areEqual(val, 1)) {
			return newOperand1;
		}
		else if (areEqual(val, -1)) {
			return new Negation(std::move(newOperand1));
		}
		else {
			return new Multiplication(std::move(newOperand1), std::move(newOperand2));
		}
	}
	else {
		return new Multiplication(std::move(newOperand1), std::move(newOperand2));
	}
}
