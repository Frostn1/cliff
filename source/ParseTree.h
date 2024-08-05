#ifndef PARSE_TREE
#define PARSE_TREE
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "Lexer.h"

typedef enum {
	// Capsul

	FULL_CONDITIONAL_PARSE,
	FULL_LOOP_PARSE,
	CONDITION_PARSE,
	IF_PARSE,
	LOOP_PARSE,
	ELSE_PARSE,
	VARIABLE_PARSE,
	EXPRESSION_PARSE, // expression
	MAIN_PARSE,
	ASSIGN_PARSE,
	FULL_FUNCTION_PARSE,
	FUNCTION_NAME_PARSE,
	FUNCTION_ARGS_PARSE,
	FUNCTION_BODY_PARSE,
	FUNCTION_TYPE_PARSE,
	ARG_PARSE,
	FULL_CALL_PARSE,
	CALL_NAME_PARSE,
	CALL_ARGS_PARSE,

	// Special
	ATOMIC_PARSE,
	IDENTIFIER_PARSE,
	
	// Default
	//single character PARSEs
	PARSE_LEFT_PAREN,
	PARSE_RIGHT_PAREN,
	PARSE_LEFT_BRACE,
	PARSE_RIGHT_BRACE,
	PARSE_LEFT_BRACKET,
	PARSE_RIGHT_BRACKET,
	PARSE_SEMICOLON,
	PARSE_COMMA,

	//one or two character PARSEs
	PARSE_PLUS,
	PARSE_PLUS_EQUAL,
	PARSE_PLUS_PLUS,

	PARSE_MINUS,
	PARSE_MINUS_EQUAL,
	PARSE_MINUS_MINUS,
	PARSE_MINUS_MORE, //MINUS_LESS is for function syntax
	PARSE_STAR,
	PARSE_STAR_EQUAL,

	PARSE_SLASH,
	PARSE_SLASH_EQUAL,

	PARSE_MODULO,
	PARSE_MODULO_EQUAL,

	PARSE_BANG,
	PARSE_BANG_EQUAL,

	PARSE_EQUAL,
	PARSE_EQUAL_EQUAL,
	PARSE_EQUAL_GREATER, //EQUAL_GREATER is for the arrow syntax

	PARSE_GREATER,
	PARSE_GREATER_EQUAL,

	PARSE_LESS,
	PARSE_LESS_EQUAL,

	//these can single, double
	PARSE_AND_AND,
	PARSE_AND,
	PARSE_OR_OR,
	PARSE_OR,
	PARSE_DOT,
	PARSE_DOT_DOT,

	//ternary operator
	PARSE_QUESTION,
	PARSE_COLON,


	//literals
	PARSE_IDENTIFIER,
	PARSE_NUMBER,
	PARSE_STRING,

	PARSE_ELSE,
	PARSE_FALSE,
	PARSE_FOR,
	PARSE_IF,
	PARSE_PRINT,
	PARSE_RETURN,
	PARSE_TRUE,
	PARSE_LOOP,
	PARSE_FUNCTION,
	PARSE_RETURN_FULL,

	//meta
	PARSE_PASS, //do nothing
	PARSE_ERROR,
	PARSE_EOF,

	// Variables Primitive Usable Types
	PARSE_INT_V,
	PARSE_FLOAT_V,
	PARSE_CHAR_V,
	PARSE_STRING_V
} ParseTreeType;

typedef struct ParseTree {
	ParseTreeType type; // expression, function, atomic
	Token* token;
	int amountOfChilds;
	struct ParseTree** childs;

	void (*addChild)(struct ParseTree* tree, struct ParseTree* child);
	struct ParseTree* (*getChild)(struct ParseTree* tree, int index);
	void (*delChild)(struct ParseTree* tree, struct ParseTree* child);
	void (*freeParseTree)(struct ParseTree* tree);

} ParseTree;

ParseTree* newTree(ParseTreeType type, Token* toke);
void __ADDCHILD__(ParseTree* tree, ParseTree* child);
ParseTree* __GETCHILD__(ParseTree* tree, int index);
void __DELCHILD__(ParseTree* tree, ParseTree* child, bool freeFlag);
// Free tree's memory recursively
void __FREEPARSETREE__(ParseTree* tree);
#endif // !PARSE_TREE

