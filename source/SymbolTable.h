#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ParseTree.h"
#define LENGTH(var) strlen(var) + 1
typedef enum {
        FUNCTION_TAG, VARIABLE_TAG, ERROR_TAG
} ValueTag;

struct variable {
    char* type;
    ParseTree* value;
};


struct arg {
    char* name;
    char* type;
};
struct function {
    struct arg* args;
    int amount;
    char* returnType;
};

struct error {
    char* msg;
};

// C Style
typedef struct TABLE_VALUE {
    ValueTag tag;
    int line;
    int column;

    union {
        
        struct function* function;
        struct variable* variable;
        struct error* error;
    };
}TABLE_VALUE;


typedef struct Table {
    int size;
    char** keys;
    TABLE_VALUE** values;
}Table;

void newTable(Table* table);
bool isDefined(Table* table, char* key);
TABLE_VALUE* getValue(Table* table, char* key);
/* Return :
    1 - Insertion succesded
    0 - Insertion Failed
*/
bool insertValue(Table* table, char* key, TABLE_VALUE* value);

struct function* makeFunction(struct arg* args, int amount, char* returnType);
struct variable* makeVariable(char* type, ParseTree* value);
struct error makeError(char* msg);
struct arg* makeArg(char* name, char* type);
TABLE_VALUE* newValue(ValueTag tag, void* structValuePointer, int line, int column);




#endif // !SYMBOL_TABLE_H