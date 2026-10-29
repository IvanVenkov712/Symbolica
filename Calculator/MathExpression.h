#pragma once
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <utility>
#include <string>
#include "poly_obj_handler.hpp"
#include "VariableInterpretation.h"

namespace Constants {
	constexpr double _pi = 3.14159265358979323846264338327950288419716939937510582;
	constexpr double _e  = 2.71828182845904523536028747135266249775724709369995957;
	constexpr const char* sinName = "sin";
	constexpr const char* cosName = "cos";
	constexpr const char* tgName = "tg";
	constexpr const char* cotgName = "cotg";
	constexpr const char* sqrtName = "sqrt";
	constexpr const char* lnName = "ln";
	constexpr const char* piName = "_pi";
	constexpr const char* eName = "_e";
}

bool areEqual(double a, double b);

class MathExpression;
using MathExpressionHandler = poly_obj_handler<MathExpression>;

class MathExpression {
public:
	using NumericType = double;

	MathExpression() = default;

	virtual MathExpression* clone() const = 0;

	virtual NumericType eval(const VariableInterpretation& vi) const = 0;

	virtual void print(std::ostream& os) const = 0;

	virtual void populateVariables(VariableInterpretation& vi) const = 0;

	virtual MathExpressionHandler differentiate(char var) const = 0;

	virtual int getPriority() const = 0;

	virtual bool isLeftAssociative() const = 0;

	virtual bool hasVariables() const = 0;

	virtual MathExpressionHandler simplify() const = 0;

	virtual ~MathExpression() = default;

	MathExpression(const MathExpression&) = delete;

	MathExpression& operator=(const MathExpression&) = delete;
};

class Constant : public MathExpression {
public:

	Constant(const NumericType& value);

	Constant(NumericType&& value);

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	void print(std::ostream& os) const override;

	void populateVariables(VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	int getPriority() const override;

	bool isLeftAssociative() const override;

	bool hasVariables() const override;

	MathExpressionHandler simplify() const override;

private:
	NumericType value;
};

class Variable : public MathExpression {
public:
	Variable(char ch);

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const;

	void print(std::ostream& os) const override;

	void populateVariables(VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	int getPriority() const override;

	bool isLeftAssociative() const override;

	bool hasVariables() const override;

	MathExpressionHandler simplify() const override;

private:
	char var;
};

class UnaryOperation : public MathExpression {
public:
	template<typename T>
	UnaryOperation(T&& operand) : operand(std::forward<T>(operand)){}

	void populateVariables(VariableInterpretation& vi) const override;

	int getPriority() const override;

	bool isLeftAssociative() const override;

	bool hasVariables() const override;

protected:
	poly_obj_handler<MathExpression> operand;
};

class UnaryFunction : public UnaryOperation {
public:
	template<typename T1, typename T2>
	UnaryFunction(T1&& t1,T2&& t2) : 
		UnaryOperation(std::forward<T1>(t1)), funName(std::forward<T2>(t2)){}

	int getPriority() const override;

	bool isLeftAssociative() const override;

	void print(std::ostream& os) const override;
private:
	std::string funName;
};

class Negation : public UnaryOperation {
public:
	template<typename T>
	Negation(T&& operand) : UnaryOperation(std::forward<T>(operand)) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	void print(std::ostream& os) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;

};

class Sine : public UnaryFunction{
public:
	template<typename T>
	Sine(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::sinName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;

};

class Cosine : public UnaryFunction {
public:
	template<typename T>
	Cosine(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::cosName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;

};


class Tangent : public UnaryFunction {
public:
	template<typename T>
	Tangent(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::tgName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;
};

class Cotangent : public UnaryFunction {
public:
	template<typename T>
	Cotangent(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::cotgName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;
};


class SquareRoot : public UnaryFunction {
public:
	template<typename T>
	SquareRoot(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::sqrtName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;
};

class NaturalLogarithm : public UnaryFunction {
public:
	template<typename T>
	NaturalLogarithm(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::lnName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;
};

class BinaryOperation : public MathExpression {
public:
	template<typename T1, typename T2>
	BinaryOperation(T1&& operand1, T2&& operand2) : 
		operand1(std::forward<T1>(operand1)), 
		operand2(std::forward<T2>(operand2)){}

	void populateVariables(VariableInterpretation& vi) const override;

	bool hasVariables() const override;

protected:
	poly_obj_handler<MathExpression> operand1;
	poly_obj_handler<MathExpression> operand2;
};

class Addition : public BinaryOperation {
public:
	template<typename T1, typename T2>
	Addition(T1&& op1, T2&& op2) : BinaryOperation(
		std::forward<T1>(op1), std::forward<T2>(op2)
	) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	void print(std::ostream& os) const override;

	MathExpressionHandler differentiate(char var) const override;

	int getPriority() const override;

	bool isLeftAssociative() const override;

	MathExpressionHandler simplify() const override;
};


class Substraction : public BinaryOperation {
public:
	template<typename T1, typename T2>
	Substraction(T1&& op1, T2&& op2) : BinaryOperation(
		std::forward<T1>(op1), std::forward<T2>(op2)
	) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	void print(std::ostream& os) const override;

	MathExpressionHandler differentiate(char var) const override;

	int getPriority() const override;

	bool isLeftAssociative() const override;

	MathExpressionHandler simplify() const override;
};


class Multiplication : public BinaryOperation {
public:
	template<typename T1, typename T2>
	Multiplication(T1&& op1, T2&& op2) : BinaryOperation(
		std::forward<T1>(op1), std::forward<T2>(op2)
	) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	void print(std::ostream& os) const override;

	MathExpressionHandler differentiate(char var) const override;

	int getPriority() const override;

	bool isLeftAssociative() const override;

	MathExpressionHandler simplify() const override;
};

class Division : public BinaryOperation {
public:
	template<typename T1, typename T2>
	Division(T1&& op1, T2&& op2) : BinaryOperation(
		std::forward<T1>(op1), std::forward<T2>(op2)
	) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	void print(std::ostream& os) const override;

	MathExpressionHandler differentiate(char var) const override;

	int getPriority() const override;

	bool isLeftAssociative() const override;

	MathExpressionHandler simplify() const override;
};

class Power : public BinaryOperation {
public:
	template<typename T1, typename T2>
	Power(T1&& op1, T2&& op2) : BinaryOperation(
		std::forward<T1>(op1), std::forward<T2>(op2)
	) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	void print(std::ostream& os) const override;

	MathExpressionHandler differentiate(char var) const override;

	int getPriority() const override;

	bool isLeftAssociative() const override;

	MathExpressionHandler simplify() const override;
};

std::ostream& operator<<(std::ostream& os, const MathExpressionHandler& handler);
