#include "expressions/MathExpression.h"

void BinaryOperation::populateVariables(VariableInterpretation& vi) const
{
	operand1->populateVariables(vi);
	operand2->populateVariables(vi);
}

bool BinaryOperation::hasVariables() const
{
	return operand1->hasVariables() || operand2->hasVariables();
}
