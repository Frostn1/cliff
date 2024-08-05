#ifndef LCCONST_H
#define LCCONST_H

#include <stdlib.h>
#include <string.h>
#include "Lexer.h"

#define LENGTH(var) strlen(var) + 1
#define printBinary(n) { unsigned i;  for (i = 1 << 31; i > 0; i = i / 2) { (n & i)? printf("1") : printf("0"); } printf("\n"); }

typedef union ufloat {
	float f;
	unsigned u;
}ufloat;

typedef enum LC_TYPE {
	LC_String, // .string
	LC_Long, // .long
}LC_TYPE;


typedef struct LCConst {
	/*
	LC Example:
	-----------

	.LC0: -- Count
		.string "Hello"
		-- Type -- Value
	*/
	LC_TYPE type;
	char* value;
	Token token; // Token of the value itself in the parseTree
}LCConst;

typedef struct LC_List {
	LCConst** consts;
	int size;
}LC_List;


LCConst* newLC(LC_TYPE type, char* value, Token* toke);
LC_List* newLC_List();
void LC_ListAdd(LC_List* list, LCConst* lcConst);
void freeLC_List(LC_List* list);
void freeLC(LCConst* lc);
int getLCOffset(LC_List* list, char* value);
void printLC(LCConst* lc, int index, FILE* fp);

#endif // !LCCONST_H
