#include "FunctionDef.h"

FunctionDef* getFunction(FunctionList* list, char* name) {
	for (size_t i = 0; i < list->size; i++) {
		if (!strcmp(list->funcs[i]->name, name))
			return list->funcs[i];
	}
	return NULL;
}

FunctionDef* newFunctionDef(char* name, StringList* codePointer) {
	FunctionDef* lc = (FunctionDef*)malloc(sizeof(FunctionDef));
	lc->name = (char*)malloc(sizeof(char) * LENGTH(name));
	strncpy(lc->name, name, LENGTH(name));
	lc->code = codePointer;
	return lc;
}
FunctionList* newFunctionList() {
	FunctionList* list = (FunctionList*)malloc(sizeof(FunctionList));
	list->size = 0;
	list->funcs = (FunctionDef**)malloc(sizeof(FunctionDef*));
	return list;
}
void FunctionListAdd(FunctionList* list, FunctionDef* def) {
	list->funcs = (FunctionDef**)realloc(list->funcs, sizeof(FunctionDef*) * (list->size + 1));
	list->funcs[list->size] = (FunctionDef*)malloc(sizeof(FunctionDef));
	list->funcs[list->size] = def;
	list->size++;
	return;
}
void freeFunctionList(FunctionList* list) {
	for (size_t i = 0; i < list->size; i++)
		freeFunctionDef(list->funcs[i]);
	free(list->funcs);
	free(list);
	return;
}
void freeFunctionDef(FunctionDef* def) {
	free(def->name);
	def->code->free(def->code);
	free(def);
	return;
}

void FunctionListDel(FunctionList* list, int index) {
	if (!list->size || index >= list->size || index < 0) return;
	freeFunctionDef(list->funcs[index]);
	list->funcs = (FunctionDef**)realloc(list->funcs, sizeof(FunctionDef*) * (list->size - 1));
	list->size--;
	return;
}

void FunctionListDelLast(FunctionList* list, int amount) {
	if (!list->size || amount >= list->size || amount < 0) return;
	for (size_t i = 0; i < amount; i++)
		FunctionListDel(list, list->size - 1);
	return;
}
