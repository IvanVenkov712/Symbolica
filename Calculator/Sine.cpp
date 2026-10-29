#include "MathExpression.h"

MathExpression* Sine::clone() const
{
	return new Sine(operand);
}

MathExpression::NumericType Sine::eval(const VariableInterpretation& vi) const
{
	return std::sin(operand->eval(vi));
}

MathExpressionHandler Sine::differentiate(char var) const
{
	return new Multiplication(new Cosine(operand), operand->differentiate(var));
}

MathExpressionHandler Sine::simplify() const
{
	MathExpressionHandler newOperand = operand->simplify();
	if (newOperand->hasVariables()) {
		return new Sine(std::move(newOperand));
	}
	else return new Constant(std::sin(newOperand->eval(VariableInterpretation::Empty)));
}
