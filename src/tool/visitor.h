#ifndef VISITOR_H
#define VISITOR_H

#include "string_builder.h"
#include "../parser/expression.h"
#include "../parser/statement.h"
#include "../parser/declaration.h"

#include "../debug.h"
// typedef struct Visitor {
//     StringBuilder* (*visitBinaryExpr)(Expr* expr);
//     StringBuilder* (*visitGroupingExpr)(Expr* expr);
//     StringBuilder* (*visitUnaryExpr)(Expr* expr);
//     StringBuilder* (*visitLiteralExpr)(Expr* expr);
// }Visitor;

// Visitor* newVisitor();
char* acceptExpr(Expr* expr);
char* parenthesize(char* name, size_t count, ...);
char* visitBinaryExpr(Expr* expr);
char* visitGroupingExpr(Expr* expr);
char* visitUnaryExpr(Expr* expr);
char* visitLiteralExpr(Expr* expr);

char* visitVarDecl(Declaration* decl);

char* acceptStatment(Statement* stat);
char* acceptDeclaration(Declaration* decl);

char* parenthesize(char* name, size_t count, ...);
#endif // !VISITOR_H
