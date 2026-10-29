#pragma once
/** @file MathExpressionFactory.h
 * @brief Text-to-expression entry point for clients and the console application.
 */
#include "parsing/ShuntingYard.h"

/// Stateless facade over tokenization, shunting-yard conversion, and tree construction.
class MathExpressionFactory {
public:
	/** @brief Parse infix text into a new owned expression tree, without simplifying it.
	 * @param strExp Case-sensitive expression using a-z, decimal literals, built-in
	 * constants/functions, explicit binary operators, unary minus, and parentheses.
	 * @throws std::invalid_argument For detected syntax/arity errors.
	 * @warning Validation is incomplete: unknown tokens can be ignored and unmatched
	 * closing parentheses can cause undefined behavior. See docs/known-issues.md.
	 */
	static poly_obj_handler<MathExpression> create(const std::string& strExp);
};
