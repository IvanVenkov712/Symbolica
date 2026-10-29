#include "expressions/MathExpression.h"

void UnaryOperation::populateVariables(VariableInterpretation& vi) const
{
	operand->populateVariables(vi);
}

int UnaryOperation::getPriority() const
{
	return 3;
}

bool UnaryOperation::isLeftAssociative() const
{
	return false;
}

bool UnaryOperation::hasVariables() const
{
	return operand->hasVariables();
}

int UnaryFunction::getPriority() const
{
	return 4;
}

bool UnaryFunction::isLeftAssociative() const
{
	return false;
}

void UnaryFunction::print(std::ostream& os) const
{
	os << funName << "(";
	operand->print(os);
	os << ")";
}