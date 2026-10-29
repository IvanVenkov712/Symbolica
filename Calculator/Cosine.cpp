#include "MathExpression.h"

MathExpression* Cosine::clone() const
{
	return new Cosine(operand);
}

MathExpression::NumericType Cosine::eval(const VariableInterpretation& vi) const
{
	return std::cos(operand->eval(vi));
}

MathExpressionHandler Cosine::differentiate(char var) const
{
	return new Negation(
		new Multiplication(
			new Sine(operand),
			operand->differentiate(var)
		)
	);
}

MathExpressionHandler Cosine::simplify() const
{
	MathExpressionHandler newOperand = operand->simplify();
	if (newOperand->hasVariables()) {
		return new Cosine(std::move(newOperand));
	}
	else return new Constant(std::cos(newOperand->eval(VariableInterpretation::Empty)));
}
