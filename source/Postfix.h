#ifndef POSTFIX_H
#define POSTFIX_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "Parser.h"

#define CON_PRIORITY 1
#define PLUS_MINUS_PRIORITY 2
#define MUL_DIV_PRIORITY 3

#define MAX_STACK_SIZE 256
#define EMPTY_STACK -1

#define EPSILON_PRECISION 0.0001



Token* pop(Token** stack, int* top);
Token* peekPost(Token** stack, int* top);
int priority(Token* x);
bool convertToPost(Parser* par, ParseTree* current, TokenType EO_Expr);

ParseTree* foldTerms(ParseTree* currentTree, ParseTree* child, Token* stack[], int* top);

#endif // !POSTFIX_H