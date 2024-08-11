#include "statement.h"

void __FREE_EXPR__(Statement* stat);
void __FREE_DECL__(Statement* stat)

Statement* newStatementExpr(Expr* expr) {
    Statement* stat = (Statement*)malloc(sizeof(Statement));
    stat->type = STATEMENT_EXPR;
    stat->expr = expr;
    stat->free = &__FREE_EXPR__;
    return stat;
}
Statement* newStatementDecl(Declaration* decl) {
    Statement* stat = (Statement*)malloc(sizeof(Statement));
    stat->type = STATEMENT_DECLARATION;
    stat->decl = decl;
    stat->free = &__FREE_DECL__;
    return stat;
}

void __FREE_EXPR__(Statement* stat) {
    stat->expr->free(stat->expr);
    free(stat);
}


void __FREE_DECL__(Statement* stat) {
    stat->decl->free(stat->decl);
    free(stat);
}
