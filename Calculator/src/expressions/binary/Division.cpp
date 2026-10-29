#include "expressions/MathExpression.h"

MathExpression* Division::clone() const
{
	return new Division(operand1, operand2);
}

MathExpression::NumericType Division::eval(const VariableInterpretation& vi) const
{
	return operand1->eval(vi) / operand2->eval(vi);
}

void Division::print(std::ostream& os) const
{
	if (operand1->getPriority() < getPriority()) {
		os << "(";
		operand1->print(os);
		os << ")";
	}
	else {
		operand1->print(os);
	}

	os << "/";

	if (operand2->getPriority() < getPriority() || (operand2->getPriority() == getPriority() && operand2->isLeftAssociative())) {
		os << "(";
		operand2->print(os);
		os << ")";
	}
	else {
		operand2->print(os);
	}
}

/// Construct the quotient rule (u' * v - u * v') / v^2 without simplifying it.
MathExpressionHandler Division::differentiate(char var) const
{
	return new Division(
		new Substraction(
			new Multiplication(operand1->differentiate(var), operand2),
			new Multiplication(operand1, operand2->differentiate(var))
		),
		new Power(operand2, new Constant(2))
	);
}

int Division::getPriority() const
{
	return 2;
}

bool Division::isLeftAssociative() const
{
	return true;
}

MathExpressionHandler Division::simplify() const
{
	MathExpressionHandler newOperand1 = operand1->simplify();
	MathExpressionHandler newOperand2 = operand2->simplify();
	if (!newOperand1->hasVariables() && !newOperand2->hasVariables()) {
		return new Constant(newOperand1->eval(VariableInterpretation::Empty) / newOperand2->eval(VariableInterpretation::Empty));
	}
	else if (!newOperand1->hasVariables()) {
		double val = newOperand1->eval(VariableInterpretation::Empty);
		if (areEqual(val, 0)) {
			return new Constant(0);
		}
		else {
			return new Division(std::move(newOperand1), std::move(newOperand2));
		}
	}
	else if (!newOperand2->hasVariables()) {
		double val = newOperand2->eval(VariableInterpretation::Empty);
		if (areEqual(val, 1)) {
			return newOperand1;
		}
		else if (areEqual(val, -1)) {
			return new Negation(std::move(newOperand1));
		}
		else {
			return new Division(std::move(newOperand1), std::move(newOperand2));
		}
	}
	else {
		return new Division(std::move(newOperand1), std::move(newOperand2));
	}
}
