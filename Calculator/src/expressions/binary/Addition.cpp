#include "expressions/MathExpression.h"

MathExpression* Addition::clone() const
{
	return new Addition(operand1, operand2);
}

MathExpression::NumericType Addition::eval(const VariableInterpretation& vi) const
{
	return operand1->eval(vi) + operand2->eval(vi);
}

void Addition::print(std::ostream& os) const
{
	operand1->print(os);
	os << "+";
	operand2->print(os);
}

MathExpressionHandler Addition::differentiate(char var) const
{
	return new Addition(operand1->differentiate(var), operand2->differentiate(var));
}

int Addition::getPriority() const
{
	return 1;
}

bool Addition::isLeftAssociative() const
{
	return true;
}

MathExpressionHandler Addition::simplify() const
{
	MathExpressionHandler newOperand1 = operand1->simplify();
	MathExpressionHandler newOperand2 = operand2->simplify();
	if (!newOperand1->hasVariables() && !newOperand2->hasVariables()) {
		return new Constant(newOperand1->eval(VariableInterpretation::Empty) + newOperand2->eval(VariableInterpretation::Empty));
	}
	else if (!newOperand1->hasVariables()) {
		NumericType val = newOperand1->eval(VariableInterpretation::Empty);
		if (areEqual(val, 0))
		{
			return newOperand2->clone();
		}
		else return new Addition(std::move(newOperand1), std::move(newOperand2));
	}
	else if (!newOperand2->hasVariables()) {
		NumericType val = newOperand2->eval(VariableInterpretation::Empty);
		if (areEqual(val, 0))
		{
			return newOperand1->clone();
		}
		else return new Addition(std::move(newOperand1), std::move(newOperand2));
	}
	else return new Addition(std::move(newOperand1), std::move(newOperand2));
}