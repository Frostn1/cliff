#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include <stdarg.h>

#include "../lexer/lexer.h"
#include "../lexer/token_type.h"
#include "../debug.h"
#include "expression.h"

// #include "SymbolTable.h"
// #include "LoadBuiltin.h"

#define START_TREE 0
#define END_VARIABLE_TREE 3

typedef struct Parser Parser;


struct Parser {
	Lexer* lex;
	Token* current;
	Token* prev;

	bool error; // Tells if there was an error in the code, so to figure out if to continue to the visitor stage or not
	bool panic; // Tells if we are in a middle of an error and wheter to ignore new errors or not

	bool (*match)(Parser* par, size_t size, ...);
	bool (*check)(Parser* par, TokenType type);
    bool (*isAtEnd)(Parser* par);
    Token* (*peek)(Parser* par);
    Token* (*previous)(Parser* par);
    Token* (*advance)(Parser* par);
    void (*synchronize)(Parser* par);

};



//forward declare the precedent rules, from low to high
typedef enum {
	PREC_NONE,
	PREC_ASSIGNMENT,
	PREC_TERNARY,
	PREC_OR,
	PREC_AND,
	PREC_EQUALITY,
	PREC_COMPARISON,
	PREC_TERM,
	PREC_FACTOR,
	PREC_UNARY,
	PREC_CALL,
	PREC_PRIMARY,
} Precedence;

// Creates a new parser
void newParser(Parser* par, Lexer* lex);
Expr* parse(Parser* par);
void synchronize(Parser* parser);

// Utilities
void error(Parser* parser, Token* token, const char* message);
bool __MATCH__(Parser* par, size_t size, ...); // Variadic is list of TokenType
bool __CHECK__(Parser* par, TokenType type);
bool __IS_AT_END__(Parser* par);
Token* __PEEK__(Parser* par);
Token* __PREVIOUS__(Parser* par);
Token* __ADVANCE__(Parser* par);
void __SYNCHRONIZE__(Parser* par);

// X
Token* consume(Parser* par, TokenType type, char* message);

// Parsing
Expr* expression(Parser* par);
Expr* term(Parser* par);
Expr* factor(Parser* par);
Expr* unary(Parser* par);
Expr* primary(Parser* par);


Expr* comparison(Parser* par);
Expr* equality(Parser* par);


#endif // !PARSER_H
