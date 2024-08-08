#include "visitor.h"
#include "string_builder.h"
#include <stdarg.h>

// Visitor* newVisitor() {
//     Visitor* visitor = (Visitor*)malloc(sizeof(Visitor));
//     visitor->visitBinaryExpr = &visitBinaryExpr;
//     visitor->visitGroupingExpr = &visitGroupingExpr;
//     visitor->visitUnaryExpr = &visitUnaryExpr;
//     visitor->visitLiteralExpr = &visitLiteralExpr;
//     return visitor;
// }


char* accept(Expr* expr) {
    char* str = NULL;
    switch(expr->type) {
        case EXPR_BINARY:
            str = visitBinaryExpr(expr);
            break;
        case EXPR_GROUPING:
            str = visitGroupingExpr(expr);
            break;
        case EXPR_LITERAL:
            str = visitLiteralExpr(expr);
            break;
        case EXPR_UNARY:
            str = visitUnaryExpr(expr);
            break;
    }
    return str;
}


char* parenthesize(char* name, size_t count, ...) {
    StringBuilder* builder = sb_create();

    sb_append(builder, "(");
    sb_append(builder, name);
    va_list ptr;
    Expr* expr;
    va_start(ptr, count);

    for (size_t i = 0; i < count; i++) {
        expr = va_arg(ptr, Expr*);
        sb_append(builder, " ");

        sb_append(builder, accept(expr));
    }
    sb_append(builder, ")");

    return sb_concat(builder);
}

char* visitBinaryExpr(Expr* expr) {
    return parenthesize(expr->Binary.op->lexeme, 2, expr->Binary.left, expr->Binary.right);
}

char* visitGroupingExpr(Expr* expr) {
    return parenthesize("group", 1, expr->Grouping.expression);
}

char* visitUnaryExpr(Expr* expr) {
    return parenthesize(expr->Unary.op->lexeme, 1, expr->Unary.right);
}

char* visitLiteralExpr(Expr* expr) {
    if (expr->Literal.value == NULL) return "nil";
    return expr->Literal.value->lexeme;
}
