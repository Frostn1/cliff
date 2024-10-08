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

typedef struct Expr Expr;

struct Expr {

    ExprType type;
    union {
        struct Binary {
            Expr* left;
            Token* op;
            Expr* right;
        } Binary;
        struct Grouping {
            Expr* expression;
        } Grouping;
        struct Literal {
            Token* value;
        } Literal;
        struct Unary {
            Token* op;
            Expr* right;
        } Unary;
    };

    void (*free)(Expr* expr);
};

Expr* newExprBinary(Expr* left, Token* op, Expr* right);
Expr* newExprGrouping(Expr* expression);
Expr* newExprLiteral(Token* value);
Expr* newExprUnary(Token* op, Expr* right);

#endif // !EXPRESSION_H
