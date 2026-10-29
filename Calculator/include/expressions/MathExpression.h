#pragma once
/** @file MathExpression.h
 * @brief Expression tree API, concrete node types, and built-in function names.
 */
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <utility>
#include <string>
#include "utility/poly_obj_handler.hpp"
#include "expressions/VariableInterpretation.h"

/// Numeric constants and exact, case-sensitive spellings recognized by the parser.
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

/** @brief Compare using an absolute difference strictly below double epsilon.
 * @note This is not a relative tolerance; non-finite values do not compare equal.
 */
bool areEqual(double a, double b);

class MathExpression;
/// Exclusive tree ownership with deep copies on copy and ownership transfer on move.
using MathExpressionHandler = poly_obj_handler<MathExpression>;

/** @brief Polymorphic node in an owned expression tree.
 *
 * Transformations return independently owned trees and leave the source unchanged.
 * Child handlers deep-copy lvalues and take ownership of rvalues or raw pointers.
 * Concrete overrides follow these contracts unless their class notes say otherwise.
 * @see docs/architecture.md and docs/known-issues.md in the repository root.
 */
class MathExpression {
public:
	using NumericType = double;

	MathExpression() = default;

	/** @brief Deep-copy this node and all descendants.
	 * @return Newly allocated root; the caller must delete it or adopt it in a handler.
	 */
	virtual MathExpression* clone() const = 0;

	/** @brief Recursively evaluate using bindings for every referenced variable.
	 * @throws std::invalid_argument If a variable has no binding.
	 * @throws std::logic_error For domain errors explicitly checked by a node.
	 * @note Domain checks are incomplete; some operations return NaN or infinity.
	 */
	virtual NumericType eval(const VariableInterpretation& vi) const = 0;

	/** @brief Write infix notation without a trailing newline.
	 * @note Formatting is intended for display; exact parse/print round trips are
	 * not guaranteed, especially for negative or rounded numeric constants.
	 */
	virtual void print(std::ostream& os) const = 0;

	/** @brief Register referenced variables in vi with their default value of zero.
	 * @warning Existing values for those variables are overwritten. Populate first,
	 * then assign values before calling eval().
	 */
	virtual void populateVariables(VariableInterpretation& vi) const = 0;

	/** @brief Construct a symbolic derivative with respect to var.
	 * @param var Normally a lowercase letter; other variable nodes are constants.
	 * @return New, unsimplified tree; its evaluable domain may differ from the source.
	 */
	virtual MathExpressionHandler differentiate(char var) const = 0;

	/// Display precedence: larger values bind more tightly; separate from parser values.
	virtual int getPriority() const = 0;

	/// Associativity metadata used by printers to choose parentheses.
	virtual bool isLeftAssociative() const = 0;

	/// Whether any descendant is a variable, independent of numerical cancellation.
	virtual bool hasVariables() const = 0;

	/** @brief Recursively fold constants and apply local algebraic identities.
	 * @return New tree; this is not a canonical form or a fixed-point rewrite engine.
	 * @warning Some identities discard domain restrictions; constant folding may
	 * bypass the checks in eval(). See the documented domain consistency problems.
	 */
	virtual MathExpressionHandler simplify() const = 0;

	virtual ~MathExpression() = default;

	MathExpression(const MathExpression&) = delete;

	MathExpression& operator=(const MathExpression&) = delete;
};

/// Double-valued leaf; ignores bindings and differentiates to zero.
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

/** @brief Single lowercase variable leaf; differentiates to one for a matching name.
 * Construction rejects names outside a-z with std::logic_error.
 */
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

/** @brief Own one child and delegate variable discovery to it.
 * Forwarded constructor arguments follow MathExpressionHandler ownership rules.
 * Callers must supply a nonempty child handler.
 */
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

/// Unary node with shared function(arg) printing and a stored function name.
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

/// Arithmetic negation; derivative is the negation of the child derivative.
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

/// Sine in radians; derivative is cos(u) * u'.
class Sine : public UnaryFunction{
public:
	template<typename T>
	Sine(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::sinName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;

};

/// Cosine in radians; derivative is -(sin(u) * u').
class Cosine : public UnaryFunction {
public:
	template<typename T>
	Cosine(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::cosName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;

};


/// Tangent in radians, spelled tg; derivative is u' / cos(u)^2.
class Tangent : public UnaryFunction {
public:
	template<typename T>
	Tangent(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::tgName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;
};

/// Cotangent in radians, spelled cotg; derivative is -(u' / sin(u)^2).
class Cotangent : public UnaryFunction {
public:
	template<typename T>
	Cotangent(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::cotgName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;
};


/** @brief Square root; derivative is u' / (2 * sqrt(u)).
 * eval() rejects negative inputs; constant folding calls std::sqrt directly.
 */
class SquareRoot : public UnaryFunction {
public:
	template<typename T>
	SquareRoot(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::sqrtName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;
};

/** @brief Natural logarithm; derivative is u' / u.
 * eval() rejects inputs <= 0; constant folding calls std::log directly.
 */
class NaturalLogarithm : public UnaryFunction {
public:
	template<typename T>
	NaturalLogarithm(T&& operand) : UnaryFunction(std::forward<T>(operand), Constants::lnName) {}

	MathExpression* clone() const override;

	NumericType eval(const VariableInterpretation& vi) const override;

	MathExpressionHandler differentiate(char var) const override;

	MathExpressionHandler simplify() const override;
};

/** @brief Own ordered left/right children and share variable discovery.
 * Lvalue child handlers are cloned; rvalues transfer ownership.
 * Callers must supply two nonempty child handlers.
 */
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

/// Sum; derivative is u' + v', with constant folding and zero elimination.
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


/// Difference; derivative is u' - v'. The public name retains its original spelling.
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


/// Product; derivative uses u' * v + u * v', with local zero/one/minus-one rules.
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

/** @brief Quotient; derivative is (u' * v - u * v') / v^2.
 * eval() performs floating-point division without checking for a zero denominator.
 * simplify() can discard the denominator of a zero numerator.
 */
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

/** @brief Exponentiation; derivative uses u^v * ((u' / u) * v + ln(u) * v').
 * eval() rejects all negative bases, including integer powers. Constant folding
 * instead calls std::pow directly. The derivative introduces logarithm/division
 * domain restrictions even for a constant exponent.
 */
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

/// Print the handled expression; an empty handler throws std::logic_error.
std::ostream& operator<<(std::ostream& os, const MathExpressionHandler& handler);
