#include "StringList.h"

StringList* newStringList() {
	StringList* list = (StringList*)malloc(sizeof(StringList));
	list->amount = 0;
	list->strings = (char**)malloc(sizeof(char*) * (list->amount + 1));

	list->add = &__ADD__;
	list->free = &__FREE__;
	return list;
}

void __ADD__(StringList* list, char* string) {
	list->strings = (char**)realloc(list->strings, sizeof(char*) * (list->amount + 1));
	list->strings[list->amount] = (char*)malloc(sizeof(char) * (LENGTH(string)));
	strncpy(list->strings[list->amount], string, strlen(string));
	list->strings[list->amount][strlen(string)] = '\0';
	list->amount++;
	return;
}

void __FREE__(StringList* list) {
	for (size_t i = 0; i < list->amount; i++) {
		free(list->strings[i]);
	}
	free(list->strings);
	free(list);
	return;
}