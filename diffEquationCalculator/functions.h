#pragma once
#include <queue>
#include <vector>
#include <string>

/*

	functions.h - PARSER and FUNCTION CALCULATION

*/

/*
	PARSER~

	Converts infix function (std::string) into postfix function (queue of tokens (std::queue)) - Shunting yard algorithm

	Tokens:
	operators: (+-/*^) and standard functions (sin; cos; tan; cot; ln; lg; etc)
	operands: constant numbers (5; pi; 4325.8457; etc), variables (x)
*/


//Token types
enum class TokenType {
	Operator,
	Operand,
	Function,
	Parenthesis,
	Unknown
};

//Functions for infix-to-postfix conversion
bool isDigitOrDecimal(const char);							 //Returns true if a char is a digit or a decimal point, otherwise false 
bool isLetter(const char);									 //Returns true if a char is a letter (in lower register), otherwise false
bool isRightAssociative(const std::string&);				 //Returns true if an operator is right associative, otherwise false
bool checkFuncSyntax(const std::string&);					 //Checks if syntax of infix function is correct (is not implemented yet)
int getOperandPriority(const std::string&);				     //Returns int that represents operand priority
TokenType getTokenType(const std::string&);					 // Returns type of a token



//Token class
class Token {
public:
	const TokenType type;
	const std::string value;
	const int priority;
	const bool rightAssociative;

	Token(std::string val) : type{ getTokenType(val) }, value{ val }, priority{ getOperandPriority(val) }, rightAssociative{ isRightAssociative(val) } {}
};

std::queue<Token> funcToPostfix(const std::string&); //Converts infix function(std::string) into postfix function(queue of tokens(std::queue))

/*
	~PARSER
*/



/*

	FUNCTION CALCULATION~

*/

double calcFunc(const double, std::string);					 // Returns result of a standard function
double calcOperator(const double, const double, const char); // Returns result of an operator

/*
	Function class:

*/
class Function {
private:
	const std::queue<Token> function;

public:
	Function(std::string functionInfix) : function{ funcToPostfix(functionInfix) } {}

	void printPostfix();
	double calc(const double, const double);
	std::vector<std::pair<double, double>> EulerSolution(const double, const double, const double, const double);
	std::vector<std::pair<double, double>> RungeKuttaSolution(const double, const double, const double, const double);
};

std::ostream& printSolution(const std::vector<std::pair<double, double>>&, std::ostream&);

/*

	~FUNCTION CALCULATION

*/