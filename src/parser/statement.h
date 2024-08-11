#ifndef STATEMENT_H
#define STATEMENT_H
#include <stdio.h>
#include <stdlib.h>
#include "../lexer/token.h"
#include "declaration.h"
#include "expression.h"

typedef enum {
    STATEMENT_EXPR,
    STATEMENT_DECLARATION,
} StatementType;

typedef struct Statement Statement;

struct Statement {

    StatementType type;
    union {
        Expr* expr;
        Declaration* decl;
    };

    void (*free)(Statement* expr);
};

Statement* newStatementExpr(Expr* expr);
Statement* newStatementDecl(Declaration* decl);






#endif // !EXPRESSION_H
