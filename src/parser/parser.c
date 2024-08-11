#include "parser.h"

void newParser(Parser *par, Lexer *lex)
{
    par->lex = lex;
    par->prev = par->current = NULL;
    par->error = par->panic = 0;
    // par->mainTree = newTree(MAIN_PARSE, NULL);
    par->match = &__MATCH__;
    par->check = &__CHECK__;
    par->isAtEnd = &__IS_AT_END__;
    par->peek = &__PEEK__;
    par->previous = &__PREVIOUS__;
    par->advance = &__ADVANCE__;
    par->synchronize = &__SYNCHRONIZE__;

    // par->table = (Table*)malloc(sizeof(Table));
    // newTable(par->table);
    // loadFunctionsToTable(par->table);
    par->advance(par);
    // par->advance(par);
}

// ============================== Utilities ==============================

void error(Parser *parser, Token *token, const char *message)
{
    // keep going while panicing
    if (parser->panic)
        return;

    fprintf(stderr, "[Line %d] Error", token->line);

    // check type
    if (token->type == T_EOF || token->type == END_LINE)
    {
        fprintf(stderr, " at end");
    }

    else
    {
        fprintf(stderr, " near '%.*s'", token->length, token->lexeme);
    }

    // finally
    fprintf(stderr, ": %s\n", message);
    parser->error = 1;
    parser->panic = 1;
    exit(0);
}

bool __MATCH__(Parser *par, size_t size, ...)
{
    va_list ptr;
    va_start(ptr, size);
    for (size_t i = 0; i < size; i++)
    {
        if (par->check(par, va_arg(ptr, TokenType)))
        {
            par->advance(par);
            va_end(ptr);
            return true;
        }
    }
    va_end(ptr);
    return false;
}

bool __CHECK__(Parser *par, TokenType type)
{
    if (par->isAtEnd(par))
        return false;
    return par->peek(par)->type == type;
}

bool __IS_AT_END__(Parser *par)
{
    return par->peek(par)->type == T_EOF;
}

Token *__PEEK__(Parser *par)
{
    return par->current;
}

Token *__PREVIOUS__(Parser *par)
{
    return par->prev;
}

Token *__ADVANCE__(Parser *par)
{
    par->prev = par->current;
    par->current = scanLexer(par->lex);
    return par->previous(par);
    // if (par->current->type == TOKEN_ERROR) error(par, par->current, "Lexer error");
}

void __SYNCHRONIZE__(Parser *par)
{
    par->advance(par);

    while (!par->isAtEnd(par))
    {
        if (par->previous(par)->type == SEMICOLON)
            return;

        switch (par->peek(par)->type)
        {
        case FUNCTION:
        case LOOP:
        case PRINT:
        case IF:
        case RETURN:
            return;
        }

        par->advance(par);
    }
}

// ============================== X ==============================

Statement *parse(Parser *par)
{
    return expressionStatement(par);
}

Token *consume(Parser *par, TokenType type, char *message)
{
    if (par->check(par, type))
        return par->advance(par);
    error(par, par->peek(par), message);
    exit(1);
}

// ============================== Parsing ==============================

Expr *primary(Parser *par)
{
    if (par->match(par, 3, FALSE, TRUE, NILL))
        return newExprLiteral(par->current);

    if (par->match(par, 3, NUMBER, STRING, IDENTIFIER))
    {
        return newExprLiteral(par->previous(par));
    }

    if (par->match(par, 1, LEFT_PAREN))
    {
        Expr *expr = expression(par);
        consume(par, RIGHT_PAREN, "Expect ')' after expression.");
        return newExprGrouping(expr);
    }

    error(par, par->peek(par), "Expect expression.");
    return NULL;
}

Expr *unary(Parser *par)
{
    if (par->match(par, 3, BANG, MINUS, PLUS, TILDA))
    {
        Token *operator= par->previous(par);
        Expr *right = unary(par);
        return newExprUnary(operator, right);
    }

    return primary(par);
}

Expr *factor(Parser *par)
{
    Expr *expr = unary(par);
    while (par->match(par, 2, SLASH, STAR))
    {
        Token *operator= par->previous(par);
        Expr *right = unary(par);
        expr = newExprBinary(expr, operator, right);
    }

    return expr;
}

Expr *term(Parser *par)
{
    Expr *expr = factor(par);

    while (par->match(par, 2, MINUS, PLUS))
    {
        Token *operator= par->previous(par);
        Expr *right = factor(par);
        expr = newExprBinary(expr, operator, right);
    }

    return expr;
}

Expr *comparison(Parser *par)
{
    Expr *expr = term(par);

    while (par->match(par, 4, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL))
    {
        Token *operator= par->previous(par);
        Expr *right = term(par);
        expr = newExprBinary(expr, operator, right);
    }

    return expr;
}

Expr *equality(Parser *par)
{
    Expr *expr = comparison(par);

    while (par->match(par, 2, BANG_EQUAL, EQUAL_EQUAL))
    {
        Token *operator= par->previous(par);
        Expr *right = comparison(par);
        expr = newExprBinary(expr, operator, right);
    }

    return expr;
}

Expr *expression(Parser *par)
{
    return equality(par);
}

// Statement* statement(Parser* par) {
//     return expressionStatement(par);
// }
// void __FREE_EXPR__(Statement* stat) {
//     stat->expr->free(stat->expr);
//     free(stat);
// }
// Statement* newStatementExpr(Expr* expr) {
//     Statement* stat = (Statement*)malloc(sizeof(Statement));
//     stat->type = STATEMENT_EXPR;
//     stat->expr = expr;
//     stat->free = &__FREE_EXPR__;
//     return stat;
// }
Statement *expressionStatement(Parser *par)
{
    Expr *expr = expression(par);
    return newStatementExpr(expr);
}
// Statement* variableDeclaration(Parser* par) {
//     Token* name = consume(par, IDENTIFIER, "Expect variable name.");
//     consume(par, COLON, "Expecting ':' token.");
//     Token* type = consume(par, IDENTIFIER, "Expect variable type.");
//     Expr* initialValue = NULL;
//     if (par->match(par, 1, EQUAL)) initialValue = expression(par);
//     return newStatementDecl(newDeclarationVariable(name, type, initialValue));
// }

// Statement* declartion(Parser* par) {
//     if(par->match(par, 1, IDENTIFIER)) return variableDeclaration(par);
//     return statement(par);
// }
