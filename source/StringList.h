#ifndef STRING_LIST_H
#define STRING_LIST_H
#include <stdlib.h>
#include <string.h>

#define LENGTH(var) strlen(var) + 1
typedef struct StringList {
	char** strings;
	int amount;

	void (*add)(struct StringList*, char*);
	void (*free)(struct StringList*);
}StringList;

StringList* newStringList();
void __ADD__(StringList* list, char* string);
void __FREE__(StringList* list);
#endif // !STRING_LIST_H


