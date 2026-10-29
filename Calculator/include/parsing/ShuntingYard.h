#pragma once
/** @file ShuntingYard.h
 * @brief Internal parser helpers, exposed as global functions by the current API.
 *
 * Pipeline: infix text -> string tokens -> reverse Polish notation -> expression tree.
 * These helpers do not constitute a fully validating grammar.
 */
#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <sstream>
#include "expressions/MathExpression.h"

/// Remove leading/trailing ASCII whitespace; preserve internal whitespace.
std::string trimStr(const std::string& str);

/// Whether ch is an ASCII digit from 0 through 9.
bool isDigit(char ch);

/// Whether ch is one of + - * / ^ ( ) , and therefore splits tokens.
bool isSpecialSymbol(char ch);

/// Recognize exactly _pi and _e.
bool isKnownConstant(const std::string& str);

/// Resolve _pi or _e; throw std::invalid_argument for any other name.
double getKnownConstant(const std::string& constantName);

/// Recognize optional minus, digits, and at most one internal decimal point (no exponent).
bool isNumber(const std::string& str);

/// Convert a previously validated decimal token; no conversion failure check is made.
double toNumber(const std::string& strNum);

/// Recognize exactly one lowercase letter a-z.
bool isVariable(const std::string& str);

/// Recognize sin, cos, tg, cotg, ln, and sqrt (case-sensitive).
bool isFunction(const std::string& str);

/// Recognize the internal unary-minus token neg.
bool isUnaryOperator(const std::string& str);

/// Recognize one of + - * / ^.
bool isBinaryOperator(const std::string& str);

/// Recognize either a binary operator or the internal neg token.
bool isOperator(const std::string& str);

/// Parser precedence: +/- = 1, * and / = 3, ^ and neg = 4; otherwise throw invalid_argument.
int getPrecedence(const std::string& str);

/// Return true for + - * / and false for ^/neg; otherwise throw std::invalid_argument.
bool isLeftAssociative(const std::string& str);

/** @brief Reorder tokens into reverse Polish notation using an operator stack.
 * @throws std::invalid_argument For detected parenthesis/operator errors.
 * @warning Unknown tokens are ignored; a closing parenthesis can pop an empty stack.
 */
std::vector<std::string> convertToRPN(const std::vector<std::string>& tokens);

/// Adopt/move the ordered left/right operands into a binary node; invalid op throws invalid_argument.
poly_obj_handler<MathExpression> executeBinaryOperation(poly_obj_handler<MathExpression>&& arg1, poly_obj_handler<MathExpression>&& arg2, char op);

/// String overload of executeBinaryOperation; requires a recognized one-character operator.
poly_obj_handler<MathExpression> executeBinaryOperation(poly_obj_handler<MathExpression>&& arg1, poly_obj_handler<MathExpression>&& arg2, const std::string& op);

/// Move arg into a Negation node; any op other than neg throws std::invalid_argument.
poly_obj_handler<MathExpression> executeUnaryOperation(poly_obj_handler<MathExpression>&& arg, const std::string& op);

/// Move arg into a built-in unary function node; unknown names throw std::invalid_argument.
poly_obj_handler<MathExpression> executeFunction(poly_obj_handler<MathExpression>&& arg, const std::string& funName);

/** @brief Split on special symbols, trim fragments, and rewrite unary minus to neg.
 * Unary minus is recognized at the start, after (, or after a binary operator.
 * @note Whitespace-only fragments can become empty tokens and affect this decision.
 */
std::vector<std::string> tokenize(const std::string& expression);

/// Move/pop the top operand; throw std::invalid_argument if the stack is empty.
poly_obj_handler<MathExpression> popArg(std::stack<poly_obj_handler<MathExpression>>& argStack);

/** @brief Build a tree from RPN tokens using an operand stack; does not evaluate it.
 * The historical name PRN means RPN. Binary operands are popped right then left.
 * @throws std::invalid_argument For operand underflow or a final stack size other than one.
 * @note Unknown tokens are ignored and the returned root is currently deep-copied.
 */
poly_obj_handler<MathExpression> calculatePRN(const std::vector<std::string>& rpnVector);
