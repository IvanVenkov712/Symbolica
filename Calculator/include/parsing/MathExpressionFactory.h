#pragma once
#include "parsing/ShuntingYard.h"

class MathExpressionFactory {
public:
	static poly_obj_handler<MathExpression> create(const std::string& strExp);
};