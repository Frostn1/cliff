#include "declaration.h"


Declaration* newDeclarationVariable(Token* name, Token* type, Expr* initialValue) {
    Declaration* decl = (Declaration*)malloc(sizeof(Declaration));
    decl->type = DECLARATION_VARIABLE;
    decl->Variable.type = type;
    decl->Variable.name = name;
    decl->Variable.initialValue = initialValue;

    decl->free = &__FREE_VARIABLE__;
    return decl;
}


void __FREE_VARIABLE__(Declaration* decl) {
    freeToken(decl->Variable.type);
    freeToken(decl->Variable.name);
    decl->Variable.initialValue->free(decl->Variable.initialValue);
    free(decl);
}
