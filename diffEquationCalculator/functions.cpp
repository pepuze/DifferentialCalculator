#include <stack>
#include <queue>
#include <string>
#include <cmath>
#include <iostream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "functions.h"

/*

	functions.h - PARSER and FUNCTION CALCULATION

*/

/*

	PARSER~

*/

/*Tokens:
	operators: (+-/*^) and functions (sin cos tan cot ln lg)
	operands: constant numbers (5; 3.14; 4325.8457), variables (x)
*/

const std::string funcs = "asin|acos|atan|acot|ln|lg|sqrt|e", ops = "+-*/^";

bool isDigitOrDecimal(const char c) {
	return ((c >= '0' && c <= '9') || c == '.');
}

bool isLetter(const char c) {
	return (c >= 'a' && c <= 'z');
}

bool isRightAssociative(const std::string& op) {
	if (op == "^") return 1;
	return 0;
}

bool checkFuncSyntax(const std::string& func) {
	int parenthesisCount = 0;
	unsigned int i = 0;
	char prevChar = ' ';
	TokenType prevToken = TokenType::Unknown;
	while (i < func.length()) {

		if (isDigitOrDecimal(func[i])) {
			if (prevToken == TokenType::Operand) {
				std::cout << "Illegal syntax: operand cannot follow another operand\n\n";
				return 0;
			}

			int dPointCount = 0;
			while (isDigitOrDecimal(func[i]) && i < func.length()) {
				if (func[i] == '.') ++dPointCount;
				++i;
			}
			prevToken = TokenType::Operand;

			if (dPointCount > 1) {
				std::cout << "Illegal syntax: too many decimal points\n\n";
				return 0;
			}
			prevChar = func[i - 1];
			continue;
		}

		if (ops.find(func[i]) != -1) {
			if (prevToken == TokenType::Operator) {
				std::cout << "Illegal syntax: operator cannot follow another operator\n\n";
				return 0;
			}
			if (prevToken == TokenType::Unknown) {
				std::cout << "Illegal syntax: operator cannot follow unknown token\n\n";
				return 0;
			}
			if (prevChar == '(') {
				std::cout << "Illegal syntax: operator cannot follow open parenthesis\n\n";
				return 0;
			}
			prevToken = TokenType::Operator;
			prevChar = func[i];
			++i;
			continue;
		}

		if (isLetter(func[i]) && func[i] != 'x' && func[i] != 'y') {
			if (prevChar == ')') {
				std::cout << "Illegal syntax: function cannot follow closed parenthesis\n\n";
				return 0;
			}

			std::string buffer;
			while (isLetter(func[i])) buffer += func[i++];
			if (funcs.find(buffer) == -1) {
				std::cout << "Illegal syntax: found unknown function: " << buffer << "\n\n";
				return 0;
			}
			prevToken = TokenType::Function;
			prevChar = func[i - 1];
			continue;
		}

		switch (func[i]) {
		case '(':
			if (prevToken == TokenType::Operand) {
				std::cout << "Illegal syntax: open parenthesis cannot follow operand\n\n";
				return 0;
			}
			++parenthesisCount;
			++i;
			prevToken = TokenType::Parenthesis;
			prevChar = '(';
			break;

		case ')':
			if (prevToken == TokenType::Operator) {
				std::cout << "Illegal syntax: closed parenthesis cannot follow operator\n\n";
				return 0;
			}
			if (prevToken == TokenType::Function) {
				std::cout << "Illegal syntax: closed parenthesis cannot follow function\n\n";
				return 0;
			}
			--parenthesisCount;
			if (parenthesisCount < 0) {
				std::cout << "Illegal syntax: missmatched parenthesis\n\n";
				return 0;
			}
			++i;
			prevToken = TokenType::Parenthesis;
			prevChar = ')';
			break;

		case 'x':
			if (prevToken == TokenType::Operand) {
				std::cout << "Illegal syntax: operand cannot follow another operand\n\n";
				return 0;
			}
			prevToken = TokenType::Operand;
			prevChar = 'x';
			++i;
			break;

		case 'y':
			if (prevToken == TokenType::Operand) {
				std::cout << "Illegal syntax: operand cannot follow another operand\n\n";
				return 0;
			}
			prevToken = TokenType::Operand;
			prevChar = 'y';
			++i;
			break;

		case ' ':
			++i;
			break;

		default:
			std::cout << "Illegal syntax: found illegal symbol: " << func[i] << "\n\n";
			return 0;
		}
	}
	if (parenthesisCount != 0) {
		std::cout << "Illegal syntax: missmatched parenthesis\n\n";
		return 0;
	}
	return 1;
}

int getOperandPriority(const std::string& op) {
	if (op == "+" || op == "-") return 1;
	if (op == "*" || op == "/") return 2;
	if (op == "^")				return 3;
	return 0;
}

TokenType getTokenType(const std::string& value) {
	if (ops.find(value) != -1)		  return TokenType::Operator;
	if (funcs.find(value) != -1)      return TokenType::Function;
	if (value == ")" || value == "(") return TokenType::Parenthesis;
	return TokenType::Operand;
}

std::queue<Token> funcToPostfix(const std::string& function) {
	std::queue<Token> output;
	std::stack<Token> opStack;

	unsigned int i = 0;
	while (i < function.size()) {
		std::string value;

		//Reading token value
		if (function[i] == ' ') {
			++i;
			continue;
		}
		if (isDigitOrDecimal(function[i]) || function[i] == 'x' || function[i] == 'y') {
			while ((isDigitOrDecimal(function[i]) || function[i] == 'x' || function[i] == 'y') && i < function.size()) value += function[i++];
		}
		else if (isLetter(function[i])) {
			while (isLetter(function[i]) && i < function.size()) value += function[i++];
		}
		else value = function[i++];

		Token token(value);
		//What to do depending on the type of token
		switch (token.type) {

			//Function - put is on operator stack
		case TokenType::Function:
			opStack.push(token);
			break;

			//Operator - if there are operators on top of operator stack (not left parenthesis)
			//that have higher priority than current operator - pop them into output
			//
			//if not - put current operator on operator stack
		case TokenType::Operator:
			while (!opStack.empty() && opStack.top().value != "(") {
				Token topOperator = opStack.top();
				if (topOperator.priority > token.priority || (topOperator.priority == token.priority && !token.rightAssociative)) {
					output.push(topOperator);
					opStack.pop();
					continue;
				}
				break;
			}
			opStack.push(token);
			break;

			//Parenthesis - 
			//left parenthesis - put is on operator stack
			//right parenthesis - pop every operator in ouput until left parenthesis is found
			//then pop left parenthesis (if there is a function on top of the stack - pop it to the ouput) and discard token
		case TokenType::Parenthesis:
			if (token.value == ")") {
				while (opStack.top().value != "(") {
					output.push(opStack.top());
					opStack.pop();
				}
				opStack.pop();
				if (!opStack.empty() && opStack.top().type == TokenType::Function) {
					output.push(opStack.top());
					opStack.pop();
				}
				break;
			}
			opStack.push(token);
			break;

			//Operand - put in in output
		case TokenType::Operand:
			output.push(token);
			break;
		}
	}

	//Pop all remainding operands in ouput
	while (!opStack.empty()) {
		output.push(opStack.top());
		opStack.pop();
	}

	return output;
}


/*

	~PARSER

*/

/*

	FUNCTION CALCULATION~

*/

double calcFunc(const double a, std::string function) {
	if (function == "asin")	return asin(a);
	if (function == "acos")	return acos(a);
	if (function == "atan")	return atan(a);
	if (function == "acot")	return atan(1.0/a);
	if (function == "sin")	return sin(a);
	if (function == "cos")	return cos(a);
	if (function == "tan")	return tan(a);
	if (function == "cot")	return 1.0 / tan(a);
	if (function == "lg")	return log10(a);
	if (function == "ln")	return log(a);
	if (function == "e")	return exp(a);
	if (function == "sqrt") return sqrt(a);
}

double calcOperator(const double a, const double b, const char op) {
	switch (op) {
	case '+':
		return a + b;
	case '-':
		return a - b;
	case '*':
		return a * b;
	case '/':
		return a / b;
	case '^':
		return pow(a, b);
	default:
		return -1;
	}
}

void Function::printPostfix() {
	std::queue<Token> funcCopy = function;
	while (!funcCopy.empty()) {
		std::cout << funcCopy.front().value << " ";
		funcCopy.pop();
	}
	return;
}

double Function::calc(const double x, const double y) {
	std::queue<Token> funcCopy = function;
	std::stack<double> stck;
	double a, b;

	while (!funcCopy.empty()) {
		Token token = funcCopy.front();
		switch (token.type) {

		case TokenType::Operand:
			if (token.value == "x")      stck.push(x);
			else if (token.value == "y") stck.push(y);
			else stck.push(std::stod(token.value));
			funcCopy.pop();
			break;

		case TokenType::Operator:
			b = stck.top(); stck.pop();
			a = stck.top(); stck.pop();
			stck.push(calcOperator(a, b, token.value[0]));
			funcCopy.pop();
			break;

		case TokenType::Function:
			a = stck.top(); stck.pop();
			stck.push(calcFunc(a, token.value));
			funcCopy.pop();
			break;
		}
	}

	return stck.top();
}

std::vector<std::pair<double, double>> Function::EulerSolution(const double leftBound, const double rightBound, const double y0 = 1, const double step = 0.1) {
	const unsigned int nPoints = (rightBound == leftBound) ? 1 : std::max((int)ceil((rightBound - leftBound) / step) + 1, 2);
	std::vector<std::pair<double, double>> solution(nPoints);

	solution[0].first = leftBound;  solution[0].second = y0;
	for (unsigned int i = 1; i < nPoints; ++i) {
		solution[i].first = std::min(leftBound + step * i, rightBound);
		solution[i].second = solution[i - 1].second + step * calc(solution[i - 1].first, solution[i - 1].second);
	}

	return solution;
}

std::vector<std::pair<double, double>> Function::RungeKuttaSolution(const double leftBound, const double rightBound, const double y0 = 1, const double step = 0.1){
	const unsigned int nPoints = (rightBound == leftBound) ? 1 : std::max((int)ceil((rightBound - leftBound) / step) + 1, 2);
	std::vector<std::pair<double, double>> solution(nPoints);

	solution[0].first = leftBound;  solution[0].second = y0;
	for (unsigned int i = 1; i < nPoints; ++i) {
		solution[i].first = std::min(leftBound + step * i, rightBound);
		const double k1 = calc(solution[i - 1].first, solution[i - 1].second),
			k2 = calc(solution[i - 1].first + step / 2, solution[i - 1].second + step / 2 * k1),
			k3 = calc(solution[i - 1].first + step / 2, solution[i - 1].second + step / 2 * k2),
			k4 = calc(solution[i - 1].first + step, solution[i - 1].second + step * k3);
		solution[i].second = solution[i - 1].second + step / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
	}

	return solution;
}

std::ostream& printSolution(const std::vector<std::pair<double, double>>& solution, std::ostream& outputStream = std::cout) {
	outputStream << std::setw(4) << "i" << std::setw(20) << "x" << std::setw(20) << "y\n";
	for (unsigned int i = 0; i < solution.size(); ++i) {
		outputStream << std::setw(4) << i << std::setw(20) << std::fixed << std::setprecision(6) << solution[i].first << std::setw(20) << std::fixed << std::setprecision(6) << solution[i].second << "\n";
	}
	return outputStream;
}

/*

	~FUNCTION CALCULATION

*/