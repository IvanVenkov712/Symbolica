#include "expressions/MathExpression.h"

Constant::Constant(const NumericType& value) : value(value) {}

Constant::Constant(NumericType&& value) : value(std::move(value)) {}

MathExpression* Constant::clone() const
{
	return new Constant(value);
}

MathExpression::NumericType Constant::eval(const VariableInterpretation& vi) const
{
	return value;
}

void Constant::print(std::ostream& os) const
{
	os << value;
}

void Constant::populateVariables(VariableInterpretation& vi) const {
	return; // There are no variables in a constant!
}

MathExpressionHandler Constant::differentiate(char var) const
{
	return new Constant(0);
}

int Constant::getPriority() const
{
	return 5;
}

bool Constant::isLeftAssociative() const
{
	return false;
}

bool Constant::hasVariables() const
{
	return false;
}

MathExpressionHandler Constant::simplify() const
{
	return new Constant(value);
}
