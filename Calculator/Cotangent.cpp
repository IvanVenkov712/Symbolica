#include "MathExpression.h"

MathExpression* Cotangent::clone() const
{
	return new Cotangent(operand);
}

MathExpression::NumericType Cotangent::eval(const VariableInterpretation& vi) const
{
	return 1.0 / std::tan(operand->eval(vi));
}

MathExpressionHandler Cotangent::differentiate(char var) const
{
	return new Negation(
		new Division(
			operand->differentiate(var),
			new Power(new Sine(operand), new Constant(2))
		)
	);
}

MathExpressionHandler Cotangent::simplify() const
{
	MathExpressionHandler newOperand = operand->simplify();
	if (newOperand->hasVariables()) {
		return new Cotangent(std::move(newOperand));
	}
	else return new Constant(1.0 / std::tan(newOperand->eval(VariableInterpretation::Empty)));
}
