#include "expressions/MathExpression.h"


MathExpression* NaturalLogarithm::clone() const
{
	return new NaturalLogarithm(operand);
}

MathExpression::NumericType NaturalLogarithm::eval(const VariableInterpretation& vi) const
{
	NumericType operandVal = operand->eval(vi);
	if (operandVal <= 0) {
		throw std::logic_error("Trying to find ln of negative number!");
	}
	return std::log(operandVal);
}

MathExpressionHandler NaturalLogarithm::differentiate(char var) const
{
	return new Division(
		operand->differentiate(var),
		operand
	);
}
MathExpressionHandler NaturalLogarithm::simplify() const
{
	// TODO: Constant folding currently bypasses eval()'s nonpositive-input check.
	MathExpressionHandler newOperand = operand->simplify();
	if (newOperand->hasVariables()) {
		return new NaturalLogarithm(std::move(newOperand));
	}
	else return new Constant(std::log(newOperand->eval(VariableInterpretation::Empty)));
}
