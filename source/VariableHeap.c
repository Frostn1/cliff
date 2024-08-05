#include "VariableHeap.h"

VariableHeap* getHeap(Heap_List* list, char* key) {
	for (size_t i = 0; i < list->size; i++) {
		if (!strcmp(list->heaps[i]->key, key))
			return list->heaps[i];
	}
	return NULL;
}

VariableHeap* newHeap(Heap_TYPE type, char* key, int margin) {
	VariableHeap* lc = (VariableHeap*)malloc(sizeof(VariableHeap));
	lc->type = type;
	lc->key = (char*)malloc(sizeof(char) * LENGTH(key));
	strncpy(lc->key, key, LENGTH(key));
	lc->margin = margin;
	return lc;
}
Heap_List* newHeap_List() {
	Heap_List* list = (Heap_List*)malloc(sizeof(Heap_List));
	list->size = 0;
	list->heaps = (VariableHeap**)malloc(sizeof(VariableHeap*));
	return list;
}
void Heap_ListAdd(Heap_List* list, VariableHeap* heap) {
	list->heaps = (VariableHeap**)realloc(list->heaps, sizeof(VariableHeap*) * (list->size + 1));
	list->heaps[list->size] = (VariableHeap*)malloc(sizeof(VariableHeap));
	list->heaps[list->size] = heap;
	list->size++;
	return;
}
void freeHeap_List(Heap_List* list) {
	for (size_t i = 0; i < list->size; i++)
		freeHeap(list->heaps[list->size]);
	free(list->heaps);
	free(list);
	return;
}
void freeHeap(VariableHeap* heap) {
	free(heap->key);
	free(heap);
	return;
}

void Heap_ListDel(Heap_List* list, int index) {
	if (!list->size || index >= list->size || index < 0 ) return;
	freeHeap(list->heaps[index]);
	list->heaps = (VariableHeap**)realloc(list->heaps, sizeof(VariableHeap*) * (list->size - 1));
	list->size--;
	return;
}

void Heap_ListDelLast(Heap_List* list, int amount) {
	if (!list->size || amount >= list->size || amount < 0) return;
	for (size_t i = 0; i < amount; i++) 
		Heap_ListDel(list, list->size - 1);
	return;
}


int getLast(Heap_List* list, Search_Flag flag) {
	for (int i = list->size - 1; i >= 0; i--) {
		if (list->heaps[i]->margin < 0 && !flag) return list->heaps[i]->margin - DWORD_SIZE;
		if (list->heaps[i]->margin > 0 && flag) return list->heaps[i]->margin + DWORD_SIZE;
	}
	return flag ? DWORD_SIZE : -DWORD_SIZE;
}
