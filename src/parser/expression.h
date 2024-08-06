#ifndef EXPRESSION_H
#define EXPRESSION_H
#include <stdio.h>
#include <stdlib.h>
#include "../lexer/token.h"

typedef enum {
    EXPR_BINARY,
    EXPR_GROUPING,
    EXPR_LITERAL,
    EXPR_UNARY,


}ExprType;

typedef struct Expr {

    ExprType type;
    union {
        typedef struct Binary {
            Expr* left;
            Token* op;
            Expr* right;
        } Binary;
        typedef struct Grouping {
            Expr* expression;
        } Grouping;
        typedef struct Literal {
            Token* value;
        } Literal;
        typedef struct Unary {
            Token* op;
            Expr* right;
        } Unary;
    };

    void (*free)(Expr* expr);
} Expr;

Expr* newExprBinary(Expr* left, Token* op, Expr* right);
Expr* newExprGrouping(Expr* expression);
Expr* newExprLiteral(Token* value);
Expr* newExprUnary(Token* op, Expr* right);

void __FREE_BINARY__(Expr* expr);
void __FREE_GROUPING__(Expr* expr);
void __FREE_LITERAL__(Expr* expr);
void __FREE_UNARY__(Expr* expr);





#endif // !EXPRESSION_H
