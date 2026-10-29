#include "expressions/MathExpression.h"
#include <limits>

bool areEqual(double a, double b)
{
	return std::abs(a - b) < std::numeric_limits<double>::epsilon();
}

std::ostream& operator<<(std::ostream& os, const MathExpressionHandler& handler)
{
	handler->print(os);
	return os;
}
