#ifndef FUNCTION_DEF_H
#define FUNCTION_DEF_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "StringList.h"

typedef struct FunctionDef {
	char* name;
	StringList* code;
}FunctionDef;

typedef struct FunctionList {
	FunctionDef** funcs;
	int size;
}FunctionList;

FunctionDef* getFunction(FunctionList* list, char* name);

FunctionDef* newFunctionDef(char* name, StringList* codePointer);
FunctionList* newFunctionList();
void FunctionListAdd(FunctionList* list, FunctionDef* def);
void FunctionListDel(FunctionList* list, int index);
void FunctionListDelLast(FunctionList* list, int amount);
void freeFunctionList(FunctionList* list);
void freeFunctionDef(FunctionDef* heap);
#endif // !FUNCTION_DEF_H
