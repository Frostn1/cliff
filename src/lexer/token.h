#ifndef TOKEN_H
#define TOKEN_H
#include <stdio.h>
#include <stdlib.h>
#include "token_type.h"

typedef struct Token {
	TokenType type; // Type of the token object
	char* lexeme; // String rep of the token itself
	int length; // Length of the lexeme field
	int line, column; // Line and column the token is found on in the raw code
} Token;

void freeToken(Token* toke);


#endif // !TOKEN_H
