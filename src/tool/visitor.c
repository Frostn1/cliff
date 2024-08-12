#include "visitor.h"
#include "string_builder.h"
#include <stdarg.h>

char* acceptExpr(Expr* expr) {
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

char* acceptStatment(Statement* stat) {
    char* str = NULL;
    switch(stat->type) {
        case STATEMENT_EXPR:
            str = acceptExpr(stat->expr);
            break;
        case STATEMENT_DECLARATION:
            str = acceptDeclaration(stat->decl);
            break;
    }
    return str;
}

char* acceptDeclaration(Declaration* decl) {
    char* str = NULL;
    switch(decl->type) {
        case DECLARATION_VARIABLE:
            str = visitVarDecl(decl);
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

        sb_append(builder, acceptExpr(expr));
    }
    sb_append(builder, ")");

    return sb_concat(builder);
}


char* visitVarDecl(Declaration* decl) {
    if (!decl->Variable.initialValue) return parenthesize(decl->Variable.name->lexeme, 1,  newExprLiteral(decl->Variable.type));
    return parenthesize(decl->Variable.name->lexeme, 2, newExprLiteral(decl->Variable.type), decl->Variable.initialValue);
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
