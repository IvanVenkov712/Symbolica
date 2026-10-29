#include "MathExpression.h"

MathExpression* Power::clone() const
{
	return new Power(operand1, operand2);
}

MathExpression::NumericType Power::eval(const VariableInterpretation& vi) const
{
	double base = operand1->eval(vi);
	if (base < 0) {
		throw std::logic_error("Trying to power a negative number");
	}
	return std::pow(base, operand2->eval(vi));
}

void Power::print(std::ostream& os) const
{
	if (operand1->getPriority() < getPriority() || (operand1->getPriority() == getPriority() && !operand1->isLeftAssociative())) {
		os << "(";
		operand1->print(os);
		os << ")";
	}
	else {
		operand1->print(os);
	}
	os << "^";
	if (operand2->getPriority() < getPriority()) {
		os << "(";
		operand2->print(os);
		os << ")";
	}
	else {
		operand2->print(os);
	}
}

MathExpressionHandler Power::differentiate(char var) const
{
	return new Multiplication(
		new Power(operand1, operand2),
		new Addition(
			new Multiplication(
				new Division(operand1->differentiate(var), operand1),
				operand2
			),
			new Multiplication(
				new NaturalLogarithm(operand1), operand2->differentiate(var)
			)
		)
	);
}

int Power::getPriority() const
{
	return 3;
}

bool Power::isLeftAssociative() const
{
	return false;
}

MathExpressionHandler Power::simplify() const
{
	MathExpressionHandler newOperand1 = operand1->simplify();
	MathExpressionHandler newOperand2 = operand2->simplify();
	if (!newOperand1->hasVariables() && !newOperand2->hasVariables()) {
		return new Constant(std::pow(newOperand1->eval(VariableInterpretation::Empty), newOperand2->eval(VariableInterpretation::Empty)));
	}
	else if (!newOperand2->hasVariables()) {
		double val = newOperand2->eval(VariableInterpretation::Empty);
		if (areEqual(val, 0)) {
			return new Constant(1);
		}
		else if (areEqual(val, 1)) {
			return newOperand1;
		}
		else {
			return new Power(std::move(newOperand1), std::move(newOperand2));
		}
	}
	else if (!newOperand1->hasVariables()) {
		double val = newOperand1->eval(VariableInterpretation::Empty);
		if (areEqual(val, 0)) {
			return new Constant(0);
		}
		else if (areEqual(val, 1)) {
			return new Constant(1);
		}
		else {
			return new Power(std::move(newOperand1), std::move(newOperand2));
		}
	}
	else {
		return new Power(std::move(newOperand1), std::move(newOperand2));
	}
}
