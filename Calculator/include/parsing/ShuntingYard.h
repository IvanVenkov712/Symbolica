#pragma once
#include <iostream>
#include <queue>
#include <stack>
#include <vector>
#include <string>
#include <sstream>
#include "expressions/MathExpression.h"

std::string trimStr(const std::string& str);

bool isDigit(char ch);

bool isSpecialSymbol(char ch);

bool isKnownConstant(const std::string& str);

double getKnownConstant(const std::string& constantName);

bool isNumber(const std::string& str);

double toNumber(const std::string& strNum);

bool isVariable(const std::string& str);

bool isFunction(const std::string& str);

bool isUnaryOperator(const std::string& str);

bool isBinaryOperator(const std::string& str);

bool isOperator(const std::string& str);

int getPrecedence(const std::string& str);

bool isLeftAssociative(const std::string& str);

std::vector<std::string> convertToRPN(const std::vector<std::string>& tokens);

poly_obj_handler<MathExpression> executeBinaryOperation(poly_obj_handler<MathExpression>&& arg1, poly_obj_handler<MathExpression>&& arg2, char op);

poly_obj_handler<MathExpression> executeBinaryOperation(poly_obj_handler<MathExpression>&& arg1, poly_obj_handler<MathExpression>&& arg2, const std::string& op);

poly_obj_handler<MathExpression> executeUnaryOperation(poly_obj_handler<MathExpression>&& arg, const std::string& op);

poly_obj_handler<MathExpression> executeFunction(poly_obj_handler<MathExpression>&& arg, const std::string& funName);

std::vector<std::string> tokenize(const std::string& expression);

poly_obj_handler<MathExpression> popArg(std::stack<poly_obj_handler<MathExpression>>& argStack);

poly_obj_handler<MathExpression> calculatePRN(const std::vector<std::string>& rpnVector);