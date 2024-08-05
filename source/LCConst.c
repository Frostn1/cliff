#include "LCConst.h"

LCConst* newLC(LC_TYPE type, char* value, Token* toke) {
	LCConst* lc = (LCConst*)malloc(sizeof(LCConst));
	lc->type = type;
	lc->value = (char*)malloc(sizeof(char) * LENGTH(value));
	strncpy(lc->value, value, LENGTH(value));
	lc->token = *toke;
	return lc;
}

void freeLC(LCConst* lc) {
	free(lc->value);
	free(lc);
	return;
}

LC_List* newLC_List() {
	LC_List* list = (LC_List*)malloc(sizeof(LC_List));
	list->size = 0;
	list->consts = (LCConst**)malloc(sizeof(LCConst*));
	return list;
}

void LC_ListAdd(LC_List* list, LCConst* lcConst) {
	list->consts = (LCConst**)malloc(sizeof(LCConst*) * (list->size + 1));
	list->consts[list->size] = (LCConst*)malloc(sizeof(LCConst));
	list->consts[list->size] = lcConst;
	list->size++;
	return;
}

void freeLC_List(LC_List* list) {
	for (size_t i = 0; i < list->size; i++) 
		freeLC(list->consts[list->size]);
	free(list->consts);
	free(list);
	return;
}

int getLCOffset(LC_List* list, char* value) {
	for (size_t i = 0; i < list->size; i++) {
		if (!strcmp(list->consts[i]->value, value))
			return i;
	}
	return 0;
}

void printLC(LCConst* lc, int index, FILE* fp) {
	fprintf(fp, "\nLC%d:\n", index);
	fprintf(fp, "\t.%s ", lc->type == LC_String ? "string" : "long");
	if (lc->type == LC_String) fprintf(fp, "\"%s\"\n\n", lc->value);
	else {
		ufloat u1;
		u1.f = atof(lc->value);
		fprintf(fp, "%d\n\n", u1.u);
	}
	return;
}
