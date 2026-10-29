#include "MathExpression.h"


MathExpression* Negation::clone() const
{
	return new Negation(operand);
}

MathExpression::NumericType Negation::eval(const VariableInterpretation& vi) const
{
	return -operand->eval(vi);
}

void Negation::print(std::ostream& os) const
{
	os << "-";
	if (operand->getPriority() <= 1) {
		os << "(";
		operand->print(os);
		os << ")";
	}
	else {
		operand->print(os);
	}
}

MathExpressionHandler Negation::differentiate(char var) const
{
	return new Negation(operand->differentiate(var));
}

MathExpressionHandler Negation::simplify() const
{
	MathExpressionHandler newOperand = operand->simplify();
	if (newOperand->hasVariables()) {
		return new Negation(std::move(newOperand));
	}
	else return new Constant(-newOperand->eval(VariableInterpretation::Empty));
}
