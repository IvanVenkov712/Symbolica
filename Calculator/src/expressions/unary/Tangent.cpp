#include "expressions/MathExpression.h"

MathExpression* Tangent::clone() const
{
	return new Tangent(operand);
}

MathExpression::NumericType Tangent::eval(const VariableInterpretation& vi) const
{
	return std::tan(operand->eval(vi));
}

MathExpressionHandler Tangent::differentiate(char var) const
{
	return new Division(
		operand->differentiate(var),
		new Power(new Cosine(operand), new Constant(2))
	);
}

MathExpressionHandler Tangent::simplify() const
{
	MathExpressionHandler newOperand = operand->simplify();
	if (newOperand->hasVariables()) {
		return new Tangent(std::move(newOperand));
	}
	else return new Constant(std::tan(newOperand->eval(VariableInterpretation::Empty)));
}

