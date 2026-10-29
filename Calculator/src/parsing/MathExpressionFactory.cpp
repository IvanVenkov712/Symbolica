#include "parsing/MathExpressionFactory.h"

poly_obj_handler<MathExpression> MathExpressionFactory::create(const std::string& strExp)
{
	std::vector<std::string> tokens = tokenize(strExp);
	std::vector<std::string> rpnTokens = convertToRPN(tokens);
	return calculatePRN(rpnTokens);
}
