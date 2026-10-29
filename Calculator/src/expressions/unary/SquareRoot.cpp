#include "expressions/MathExpression.h"

MathExpression* SquareRoot::clone() const
{
	return new SquareRoot(operand);
}

MathExpression::NumericType SquareRoot::eval(const VariableInterpretation& vi) const
{
	NumericType operandVal = operand->eval(vi);
	if (operandVal < 0) {
		throw std::logic_error("Trying to find sqrt of negative number!");
	}
	return sqrt(operandVal);
}

MathExpressionHandler SquareRoot::differentiate(char var) const
{
	return new Division(
		operand->differentiate(var),
		new Multiplication(new Constant(2), new SquareRoot(operand))
	);
}

MathExpressionHandler SquareRoot::simplify() const
{
	MathExpressionHandler newOperand = operand->simplify();
	if (newOperand->hasVariables()) {
		return new SquareRoot(std::move(newOperand));
	}
	else return new Constant(std::sqrt(newOperand->eval(VariableInterpretation::Empty)));
}
