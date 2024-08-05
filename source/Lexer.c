#include "Lexer.h"

void freeToken(Token* toke) {
	if (!toke) return;
	free(toke->lexeme);
	free(toke);
}
void freeLexer(Lexer* lex) {
	if (!lex) return;
	free(lex->text);
	free(lex);
}

void cleanLexer(Lexer* lex) {
	lex->text = NULL;
	lex->column = lex->line = 1;
	lex->index = lex->start = 0;
}

void newLexer(Lexer* lex, char* text) {
	cleanLexer(lex);
	lex->size = strlen(text);
	lex->text = (char*)malloc(sizeof(char) * (lex->size + 1));
	strncpy(lex->text, text, lex->size);
	lex->currentChar = lex->text[lex->index];
	return;
}



char peek(Lexer* lex) {
	return lex->index + 1 == lex->size ? '\0' : lex->text[lex->index + 1];
}

char show(Lexer* lex) {
	return lex->currentChar;
}

bool isAtEnd(Lexer* lex) {
	return lex->currentChar == '\0';
}

bool isDigit(char c) {
	return c >= '0' &&
		c <= '9';
}

bool isAlpha(char c) {
	return c >= 'a' && c <= 'z' ||
		c >= 'A' && c <= 'Z';
}

bool isIdentifier(char c) {
	return isAlpha(c) ||
		isDigit(c) ||
		c == '_';
}

bool match(Lexer* lex, char c) {
	if (peek(lex) == c) {
		advance(lex);
		return true;
	}

	return false;
}

void eatWhiteSpace(Lexer* lex) {
	switch (lex->currentChar) {
	case ' ':
	case '\r':
	case '\t':
		advance(lex);
		break;
	case '\n':
		if (peek(lex) == '\n') {
			advance(lex);
			break;
		}
		else {
			return;
		}


		//comments
	case '/':
		if (peek(lex) == '/') {
			while (advance(lex) != '\n' && !isAtEnd(lex));
			break;
		}

		if (peek(lex) == '*') {
			advance(lex);
			advance(lex);
			while (show(lex) != '*' && peek(lex) != '/') advance(lex);
			advance(lex);
			advance(lex);
			break;
		}

	default:
		return;
	}
	eatWhiteSpace(lex);
	return;
}

Token* makeToken(Lexer* lex, TokenType type) {

	Token* toke = (Token*)malloc(sizeof(Token));

	toke->type = type;
	toke->length = 1;
	toke->lexeme = (char*)malloc(sizeof(char) * (toke->length + 1));
	toke->lexeme[0] = lex->currentChar;
	toke->lexeme[1] = '\0';
	toke->line = lex->line;
	toke->column = lex->column;
	advance(lex);
	return toke;
}

char advance(Lexer* lex) {
	if ((lex->currentChar = peek(lex)) == '\n') {
		lex->line += 1;
		lex->column = 1;
		lex->index += 1;
	}
	else {
		lex->column += 1;
		lex->index += 1;
	}
	return lex->currentChar;
}

Token* makeErrorToken(Lexer* lex, char* msg, int startLine) {
	Token* toke = (Token*)malloc(sizeof(Token));

	toke->type = TOKEN_ERROR;
	toke->lexeme = msg;
	toke->length = strlen(msg);
	toke->line = startLine != -1 ? startLine : lex->line;
	toke->column = lex->column - 1;
	advance(lex);
	return toke;
}

Token* makeNumber(Lexer* lex) {
	while (isDigit(peek(lex))) advance(lex);

	TokenType semiType = TOKEN_INT;
	if (peek(lex) == '.') {
		semiType = TOKEN_FLOAT;
		advance(lex);
		while (isDigit(peek(lex))) advance(lex);
	}

	Token* toke = (Token*)malloc(sizeof(Token));

	toke->type = semiType;
	toke->length = lex->index - lex->start + 1;
	toke->lexeme = (char*)malloc(sizeof(char) * (toke->length + 1));
	strncpy(toke->lexeme, &lex->text[lex->start], toke->length);
	toke->lexeme[toke->length] = '\0';
	toke->line = lex->line;
	toke->column = lex->column - toke->length + 1;

	advance(lex); //prime next
	return toke;
}


Token* makeKeywordOrIdentifier(Lexer* lex) {
	advance(lex); //first letter can only be alpha

	while (isIdentifier(show(lex))) {
		advance(lex);
	}

	//scan for a keyword
	for (int i = 0; keywordTypes[i].keyword; i++) {
		if (strlen(keywordTypes[i].keyword) == lex->index - lex->start && !strncmp(keywordTypes[i].keyword, &lex->text[lex->start], lex->index - lex->start)) {
			Token* toke = (Token*)malloc(sizeof(Token));

			toke->type = keywordTypes[i].type;
			toke->length = lex->index - lex->start;
			toke->lexeme = (char*)malloc(sizeof(char) * (toke->length + 1));
			strncpy(toke->lexeme, &lex->text[lex->start], toke->length);
			toke->lexeme[toke->length] = '\0';
			toke->line = lex->line;
			toke->column = lex->column - toke->length - 1;

			return toke;
		}
	}

	//return an identifier
	Token* toke = (Token*)malloc(sizeof(Token));

	toke->type = TOKEN_IDENTIFIER;
	toke->length = lex->index - lex->start;
	toke->lexeme = (char*)malloc(sizeof(char) * (toke->length + 1));
	strncpy(toke->lexeme, &lex->text[lex->start], toke->length);
	toke->lexeme[toke->length] = '\0';
	toke->line = lex->line;
	toke->column = lex->column - toke->length - 1;

	return toke;
}

Token* makeString(Lexer* lex, char terminator) {
	advance(lex);
	int startLine = lex->line;
	while (peek(lex) && peek(lex) != terminator) {
		//escaping strings
		if (peek(lex) == '\\') {
			advance(lex);
		}

		advance(lex);
	}

	advance(lex); //eat terminator

	if (isAtEnd(lex)) {
		return makeErrorToken(lex, "Unterminated string", startLine);
	}


	Token* toke = (Token*)malloc(sizeof(Token));

	toke->type = TOKEN_STRING;
	toke->length = lex->index - lex->start - 1;
	toke->lexeme = (char*)malloc(sizeof(char) * (toke->length + 1));
	strncpy(toke->lexeme, &lex->text[lex->start + 1], toke->length);
	toke->lexeme[toke->length] = '\0';
	toke->line = lex->line;
	toke->column = lex->column - toke->length;

	advance(lex); //prime next
	return toke;
}


Token* scanLexer(Lexer* lex) {

	eatWhiteSpace(lex);
	lex->start = lex->index;
	if (isAtEnd(lex)) return makeToken(lex, TOKEN_EOF);

	if (isDigit(show(lex))) return makeNumber(lex);
	if (isAlpha(show(lex))) return makeKeywordOrIdentifier(lex);

	switch (lex->currentChar) {
	case '(': return makeToken(lex, TOKEN_LEFT_PAREN);
	case ')': return makeToken(lex, TOKEN_RIGHT_PAREN);
	case '{': return makeToken(lex, TOKEN_LEFT_BRACE);
	case '}': return makeToken(lex, TOKEN_RIGHT_BRACE);
	case '[': return makeToken(lex, TOKEN_LEFT_BRACKET);
	case ']': return makeToken(lex, TOKEN_RIGHT_BRACKET);
	case ';': return makeToken(lex, TOKEN_SEMICOLON);
	case ',': return makeToken(lex, TOKEN_COMMA);

	case '+': return makeToken(lex, match(lex, '=') ? TOKEN_PLUS_EQUAL : match(lex, '+') ? TOKEN_PLUS_PLUS : TOKEN_PLUS);
	case '-': return makeToken(lex, match(lex, '=') ? TOKEN_MINUS_EQUAL : match(lex, '-') ? TOKEN_MINUS_MINUS : match(lex, '>') ? TOKEN_MINUS_MORE : TOKEN_MINUS);
	case '*': return makeToken(lex, match(lex, '=') ? TOKEN_STAR_EQUAL : TOKEN_STAR);
	case '/': return makeToken(lex, match(lex, '=') ? TOKEN_SLASH_EQUAL : TOKEN_SLASH);
	case '%': return makeToken(lex, match(lex, '=') ? TOKEN_MODULO_EQUAL : TOKEN_MODULO);

	case '!': return makeToken(lex, match(lex, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
	case '=': return makeToken(lex, match(lex, '=') ? TOKEN_EQUAL_EQUAL : match(lex, '>') ? TOKEN_EQUAL_GREATER : TOKEN_EQUAL);

	case '>': return makeToken(lex, match(lex, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
	case '<': return makeToken(lex, match(lex, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);

	case '"':
	case '\'':
		return makeString(lex, lex->currentChar);
	case '\n':
		return makeToken(lex, TOKEN_END_LINE);
	default:
		// -1 in the final arg in the `makeErrorToken` function call tells me wheter I should ignore the line given from the lexer itself and use a new line I give it or use the line from the lexer
		// If -1 is used then I should just used the line from the lexer, if any other number I should use said number instead
		// This feature is used when I have an error spread on multiple lines and I want to tell the user when did the line begin
		return makeErrorToken(lex, "Unexpected token", DONT_OVERWRITE_BASELINE);
	}
}



