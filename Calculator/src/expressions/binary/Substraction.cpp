#include "expressions/MathExpression.h"


MathExpression* Substraction::clone() const
{
	return new Substraction(operand1, operand2);
}

MathExpression::NumericType Substraction::eval(const VariableInterpretation& vi) const
{
	return operand1->eval(vi) - operand2->eval(vi);
}

void Substraction::print(std::ostream& os) const
{
	operand1->print(os);
	os << "-";
	if (operand2->getPriority() <= getPriority()) {
		os << "(";
		operand2->print(os);
		os << ")";
	}
	else {
		operand2->print(os);
	}

}

MathExpressionHandler Substraction::differentiate(char var) const
{
	return 	new Substraction(operand1->differentiate(var), operand2->differentiate(var));
}

int Substraction::getPriority() const
{
	return 1;
}

bool Substraction::isLeftAssociative() const
{
	return true;
}

MathExpressionHandler Substraction::simplify() const
{
	MathExpressionHandler newOperand1 = operand1->simplify();
	MathExpressionHandler newOperand2 = operand2->simplify();
	if (!newOperand1->hasVariables() && !newOperand2->hasVariables()) {
		return new Constant(newOperand1->eval(VariableInterpretation::Empty) - newOperand2->eval(VariableInterpretation::Empty));
	}
	else if (!newOperand1->hasVariables()) {
		NumericType val = newOperand1->eval(VariableInterpretation::Empty);
		if (areEqual(val, 0))
		{
			return new Negation(std::move(newOperand2));
		}
		else return new Substraction(std::move(newOperand1), std::move(newOperand2));
	}
	else if (!newOperand2->hasVariables()) {
		NumericType val = newOperand2->eval(VariableInterpretation::Empty);
		if (areEqual(val, 0))
		{
			return newOperand1->clone();
		}
		else return new Substraction(std::move(newOperand1), std::move(newOperand2));
	}
	else return new Substraction(std::move(newOperand1), std::move(newOperand2));
}
