#include "LoadBuiltin.h"


extern char* FunctionNames[FUNCTION_AMOUNT] = {
	"printi",
	"printf",
	"prints"
};

extern char* ArgTypes[FUNCTION_AMOUNT] = {
	"int",
	"float",
	"string"
};


// TODO : Fill in the assembly code for each type of print
extern char* FunctionCode[FUNCTION_AMOUNT] = {
	"",
	"",
	""
};



void loadFunctionsToTable(Table* table) {
	for (size_t i = 0; i < FUNCTION_AMOUNT; i++) {
		insertValue(table, FunctionNames[i], newValue(FUNCTION_TAG, makeFunction(makeArg("x", ArgTypes[i]), 1, "void"), -1, -1));
	}
	return;
}

void loadFunctionBinaries(FunctionList* list) {
	for (size_t i = 0; i < FUNCTION_AMOUNT; i++) {
		StringList* strList = newStringList();
		strList->add(strList, FunctionCode[i]);
		FunctionListAdd(list, newFunctionDef(FunctionNames[i], strList));
	}
	return;
}
