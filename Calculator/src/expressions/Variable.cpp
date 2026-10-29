#include "expressions/MathExpression.h"

Variable::Variable(char ch)
{
	if (!isValidVariable(ch)) {
		throw std::logic_error("Initialize with invalid variable");
	}
	var = ch;
}

MathExpression* Variable::clone() const
{
	return new Variable(var);
}

MathExpression::NumericType Variable::eval(const VariableInterpretation& vi) const
{
	if (!vi.participates(var)) {
		throw std::invalid_argument("Not a value for that variable!");
	}
	return vi[var];
}

void Variable::print(std::ostream& os) const
{
	os << var;
}

void Variable::populateVariables(VariableInterpretation& vi) const
{
	vi.addVar(var);
}

MathExpressionHandler Variable::differentiate(char var) const
{
	if (var == this->var) {
		return new Constant(1);
	}
	return new Constant(0);
}

int Variable::getPriority() const
{
	return 5;
}

bool Variable::isLeftAssociative() const
{
	return false;
}

bool Variable::hasVariables() const
{
	return true;
}

MathExpressionHandler Variable::simplify() const
{
	return new Variable(var);
}
