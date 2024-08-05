#ifndef VARIABLE_HEAP_H
#define VARIABLE_HEAP_H

#include <stdlib.h>
#include <string.h>

#define LENGTH(var) strlen(var) + 1
#define DWORD_SIZE 4

typedef enum {
	HEAP_DWORD,
}Heap_TYPE;

typedef enum {
	SEARCH_VARIABLE,
	SEARCH_ARG
}Search_Flag;

typedef struct VariableHeap {
	Heap_TYPE type;
	char* key; // i.e. name of the variable
	int margin; // i.e. amount of spots you move in the stack, usually 4 or 8
}VariableHeap;

typedef struct Heap_List {
	VariableHeap** heaps;
	int size;
}Heap_List;

VariableHeap* getHeap(Heap_List* list, char* key);
int getLast(Heap_List* list, Search_Flag flag);


VariableHeap* newHeap(Heap_TYPE type, char* key, int margin);
Heap_List* newHeap_List();
void Heap_ListAdd(Heap_List* list, VariableHeap* heap);
void Heap_ListDel(Heap_List* list, int index);
void Heap_ListDelLast(Heap_List* list, int amount);
void freeHeap_List(Heap_List* list);
void freeHeap(VariableHeap* heap);

#endif // !VARIABLE_HEAP_H
