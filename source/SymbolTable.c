#include "SymbolTable.h"

void newTable(Table* table) {
    table->size = 0;
    table->keys = (char**)malloc(sizeof(char*) * ( table->size + 1 ));
    table->values = (TABLE_VALUE*)malloc(sizeof(TABLE_VALUE) * ( table->size + 1 ));
    return;
}

bool isDefined(Table* table, char* key) {
    for (size_t i = 0; i < table->size; i++)
        if(!strcmp(table->keys[i], key)) return true;
    
    return false;   
}

TABLE_VALUE* getValue(Table* table, char* key) {
    for (size_t i = 0; i < table->size; i++)
        if (!strcmp(table->keys[i], key))
                return table->values[i];
    
    struct error err = makeError("no such key");
    TABLE_VALUE* val = newValue(ERROR_TAG, &err, -1, -1);
    return val;
    
}

/* Return :
    1 - Insertion succesded
    0 - Insertion Failed
*/
bool insertValue(Table* table, char* key, TABLE_VALUE* value) {
    if (!isDefined(table,key)) {
        table->keys = (char**)realloc(table->keys, sizeof(char*) * ( table->size + 1 ));
        table->keys[table->size] = (char*)malloc(sizeof(char) * (LENGTH(key)));
        strncpy(table->keys[table->size], key, strlen(key));
        table->keys[table->size][strlen(key)] = '\0';
        table->values = (TABLE_VALUE**)realloc(table->values, sizeof(TABLE_VALUE*) * (table->size + 1));
        table->values[table->size] = value;
        table->size++;

        return true;
    }

    return false;
}


struct function* makeFunction(struct arg* args, int amount, char* returnType) {
    struct function* func = (struct function*)malloc(sizeof(struct function));
    func->args = args;
    func->amount = amount;
    func->returnType = (char*)malloc(sizeof(char) * (LENGTH(returnType)));
    strncpy(func->returnType, returnType, LENGTH(returnType));
    return func;
}

struct variable* makeVariable(char* type, ParseTree* value) {
    struct variable* var = (struct variable*)malloc(sizeof(struct variable));
    var->type = (char*)malloc(sizeof(char) * (LENGTH(type)));
    var->value = value;
    strncpy(var->type, type, LENGTH(type));
    return var;
}

struct error makeError(char* msg) {
    struct error err;
    err.msg = (char*)malloc(sizeof(char) * (LENGTH(msg)));
    strncpy(err.msg, msg, LENGTH(msg));
    return err;
}

struct arg* makeArg(char* name, char* type) {
    struct arg* arg2 = (struct arg*)malloc(sizeof(struct arg));
    arg2->name = (char*)malloc(sizeof(char) * (LENGTH(name)));
    arg2->type = (char*)malloc(sizeof(char) * (LENGTH(type)));
    strncpy(arg2->name, name, LENGTH(name));
    strncpy(arg2->type, type, LENGTH(type));
    return arg2;
}

TABLE_VALUE* newValue(ValueTag tag, void* structValuePointer, int line, int column) {
    TABLE_VALUE* value = (TABLE_VALUE*)malloc(sizeof(TABLE_VALUE));
    value->line = line;
    value->column = column;
    value->tag = tag;

    switch(value->tag) {
        case FUNCTION_TAG: value->function = (struct function*)structValuePointer; return value;
        case VARIABLE_TAG: value->variable = (struct variable*)structValuePointer; return value;
        case ERROR_TAG: value->error = (struct error*)structValuePointer; return value;
        default:return value;
    }
}
