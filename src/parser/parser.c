#include "Parser.h"
#include "Postfix.h"
#include "expression.h"
#include "parser.h"

void newParser(Parser* par, Lexer* lex) {
	par->lex = lex;
	par->pre = par->current = NULL;
	par->error = par->panic = 0;
	par->mainTree = newTree(MAIN_PARSE, NULL);
	par->match = &__MATCH__;
	par->check = &__CHECK__;
    par->isAtEnd = &__IS_AT_END__;
    par->peek = &__PEEK__;
    par->previous = &__PREVIOUS__;
    par->advance = &__ADVANCE__;

	// par->table = (Table*)malloc(sizeof(Table));
	// newTable(par->table);
	// loadFunctionsToTable(par->table);
}



// ============================== Utilities ==============================

void error(Parser* parser, Token* token, const char* message) {
	//keep going while panicing
	if (parser->panic) return;

	fprintf(stderr, "[Line %d] Error", token->line);

	//check type
	if (token->type == TOKEN_EOF || token->type == TOKEN_END_LINE) {
		fprintf(stderr, " at end");
	}

	else {
		fprintf(stderr, " near '%.*s'", token->length, token->lexeme);
	}

	//finally
	fprintf(stderr, ": %s\n", message);
	parser->error = 1;
	parser->panic = 1;
	exit(0);
}


bool __MATCH__(Parser* par, size_t size, ...) {
    va_list ptr;
    va_start(ptr, size);
    for (size_t i = 0; i < size; i++) {
        if (par->check(par, va_arg(ptr, TokenType))) {
            par->advance(par);
            va_end(ptr);
            return true;
        }
    }
    va_end(ptr);
    return false;
}
bool __CHECK__(Parser* par, TokenType type) {
    if (par->isAtEnd(par)) return false;
    return par->peek(par)->type == type;
}
bool __IS_AT_END__(Parser* par) {

}
Token* __PEEK__(Parser* par) {

}
Token* __PREVIOUS(Parser* par) {

}
Token* __ADVANCE__(Parser* par) {
	par->prev = par->current;
	par->current = scanLexer(par->lex);
	if (par->current->type == TOKEN_ERROR) error(par, par->current, "Lexer error");
}


// ============================== X ==============================


Token* consume(Parser* par, TokenType type, char* message) {
    if (par->check(par, type)) return par->advance(par);
    error(par, par->peek(par), message);
    exit(1);
}


// ============================== Parsing ==============================

Expr* primary(Parser* par) {
    if (par->match(par, 3, TOKEN_FALSE, TOKEN_TRUE, TOKEN_NULL)) return newExprLiteral(par->current);

    if (par->match(par, 2, TOKEN_NUMBER, TOKEN_STRING)) {
      return newExprLiteral(par->prev);
    }

    if (par->match(par, 1, TOKEN_LEFT_PAREN)) {
      Expr* expr = expression(par);
      consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
      return new Expr.Grouping(expr);
    }
  }



Expr* unary(Parser* par) {
    if (match(BANG, MINUS)) {
      Token operator = previous();
      Expr right = unary();
      return new Expr.Unary(operator, right);
    }

    return primary();
}

Expr* factor(Parser* par) {
    Expr expr = unary();

    while (match(SLASH, STAR)) {
      Token operator = previous();
      Expr right = unary();
      expr = new Expr.Binary(expr, operator, right);
    }

    return expr;
}


Expr* term(Parser* par) {
   Expr expr = factor(par);

   while (match(MINUS, PLUS)) {
     Token operator = previous();
     Expr right = factor();
     expr = new Expr.Binary(expr, operator, right);
   }

   return expr;
}

Expr* comparison(Parser* par) {
    Expr expr = term();

    while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
        Token operator = previous();
        Expr right = term();
        expr = new Expr.Binary(expr, operator, right);
    }

    return expr;
}

Expr* expression(Parser* par) {
    return equality(par);
}


Expr* equality(Parser* par) {
    Expr expr = comparison();

    while (match(BANG_EQUAL, EQUAL_EQUAL)) {
        Token operator = previous();
        Expr right = comparison();
        expr = newExprBinary(expr, operator, right);
    }

    return expr;
}
