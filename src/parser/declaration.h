#ifndef DECLARATION_H
#define DECLARATION_H
#include <stdio.h>
#include <stdlib.h>
#include "../lexer/token.h"
#include "expression.h"

typedef enum {
    DECLARATION_VARIABLE
} DeclarationType;

typedef struct Declaration Declaration;

struct Declaration {

    DeclarationType type;
    union {
        struct Variable {
            Token* name;
            Token* type;
            Expr* initialValue;
        } Variable;
    };

    void (*free)(Declaration* expr);
};

Declaration* newDeclarationVariable(Token* name, Token* type, Expr* initialValue);

#endif // !EXPRESSION_H
