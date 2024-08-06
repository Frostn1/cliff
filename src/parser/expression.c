#include "expression.h"


Expr* newExprBinary(Expr* left, Token* op, Expr* right) {
    Expr* expr = (Expr*)malloc(sizeof(Expr));
    expr->type = EXPR_BINARY;
    expr->Binary.left = left;
    expr->Binary.op = op;
    expr->Binary.right = right;
    expr->free = &__FREE_BINARY__;
    return expr;
}
Expr* newExprGrouping(Expr* expression) {
    Expr* expr = (Expr*)malloc(sizeof(Expr));
    expr->type = EXPR_GROUPING;
    expr->Grouping.expression = expression;
    expr->free = &__FREE_GROUPING__;
    return expr;
}
Expr* newExprLiteral(Token* value) {
    Expr* expr = (Expr*)malloc(sizeof(Expr));
    expr->type = EXPR_LITERAL;
    expr->Literal.value = value;
    expr->free = &__FREE_LITERAL__;
    return expr;
}
Expr* newExprUnary(Token* op, Expr* right) {
    Expr* expr = (Expr*)malloc(sizeof(Expr));
    expr->type = EXPR_UNARY;
    expr->Unary.op = op;
    expr->Unary.right = right;
    expr->free = &__FREE_UNARY__;
    return expr;
}

void __FREE_BINARY__(Expr* expr) {
    freeToken(expr->Binary.op);
    expr->Binary.left->free(expr->Binary.left);
    expr->Binary.right->free(expr->Binary.right);
    free(expr);
}
void __FREE_GROUPING__(Expr* expr) {
    expr->Grouping.expression->free(expr->Grouping.expression);
    free(expr);
}
void __FREE_LITERAL__(Expr* expr) {
    freeToken(expr->Literal.value);
    free(expr);
}
void __FREE_UNARY__(Expr* expr) {
    freeToken(expr->Unary.op);
    expr->Unary.right->free(expr->Unary.right);
    free(expr);
}
