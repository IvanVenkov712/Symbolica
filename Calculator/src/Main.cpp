#include <iostream>
#include "parsing/MathExpressionFactory.h"

int main()
{
	std::string exprStr;
	while (exprStr != "exit") {
		try {
			std::cout << "Enter an expression:   ";
			std::getline(std::cin, exprStr);
			MathExpressionHandler expression = MathExpressionFactory::create(exprStr);
			std::cout << "Parsed expression:     " << expression << std::endl;
			std::cout << "Simplified expression: " << expression->simplify() << std::endl;
			std::cout << "Derivative:            " << expression->differentiate('x') << std::endl;
			std::cout << "Simplified derivative: " << expression->differentiate('x')->simplify() << std::endl;
		}
		catch (const std::exception& e) {
			std::cout << "An error occurred: " << e.what() << std::endl;
		}
	}
}