#include <iostream>
#include "parsing/MathExpressionFactory.h"

/** @brief Run the interactive parse/simplify/differentiate demonstration for x.
 * @bug The newly read exit command is parsed before the loop condition sees it.
 * @bug Stream failure/EOF is not checked and can cause repeated prompts/errors.
 */
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
