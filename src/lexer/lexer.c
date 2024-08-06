#include "lexer.h"
#include "token_type.h"
#include <stdio.h>

void freeLexer(Lexer* lex) {
	if (!lex) return;
	free(lex->file);
}

void cleanLexer(Lexer* lex) {
	lex->file = NULL;
	lex->saved = NULL;
	lex->column = lex->line = 1;
	lex->index = lex->start = lex->chunkIndex = 0;
}

void newLexer(Lexer* lex, FILE* file) {
    cleanLexer(lex);
	fseek(file, 0L, SEEK_END);
	lex->size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	lex->file = file;
	fread(lex->chunk, sizeof(char), CHUNK_SIZE, lex->file);
	fread(lex->nextChunk, sizeof(char), CHUNK_SIZE, lex->file);
	lex->currentChar = lex->chunk[lex->index];
	return;
}



char peek(Lexer* lex) {
    if(lex->index + 1 == lex->size) return '\0';
    if(lex->chunkIndex == CHUNK_SIZE - 1) return lex->nextChunk[0];
	return  lex->chunk[lex->chunkIndex + 1];
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
		else return;

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

void saveChar(Lexer* lex, char c) {
    if(lex->saved) {
        if(lex->isChunkSize == CHUNK_SIZE) {
            realloc(lex->saved, (lex->savedSize + CHUNK_SIZE) * sizeof(char));
            lex->isChunkSize = 0;
        }
    } else if (lex->saved == NULL) {
        lex->saved = (char*)malloc(CHUNK_SIZE * sizeof(char));
        lex->isChunkSize = 0;
        lex->savedSize = 0;
    }
    lex->saved[lex->savedSize++] = c;
    lex->isChunkSize += 1;
}

char advance(Lexer* lex) {
	if ((lex->currentChar = peek(lex)) == '\n') {
		lex->line++;
		lex->column = 1;
	}
	else {
		lex->column++;
	}
	lex->index++;
	lex->chunkIndex++;
	if (lex->chunkIndex == CHUNK_SIZE) {
        strncpy(lex->chunk, lex->nextChunk, CHUNK_SIZE);
        fread(lex->nextChunk, sizeof(char), CHUNK_SIZE, lex->file);
        lex->chunkIndex = 0;
	}

	return lex->currentChar;
}

char advanceWithSave(Lexer* lex) {
    advance(lex);
    saveChar(lex, lex->currentChar);
   	return lex->currentChar;
}

Token* makeToken(Lexer* lex, TokenType type) {

	Token* toke = (Token*)malloc(sizeof(Token));

	toke->type = type;
	toke->length = 1;
	toke->lexeme = (char*)malloc(sizeof(char) * (toke->length + 1));
	toke->lexeme[0] = lex->currentChar;
	toke->lexeme[1] = '\0';
	toke->line = lex->line;
	toke->column = lex->column + 1;
	advance(lex);
	return toke;
}


Token* makeErrorToken(Lexer* lex, char* msg, int startLine) {
	Token* toke = (Token*)malloc(sizeof(Token));

	toke->type = TOKEN_ERROR;
	toke->lexeme = msg;
	toke->length = strlen(msg);
	toke->line = startLine != -1 ? startLine : lex->line;
	toke->column = lex->column;
	advance(lex);
	return toke;
}

Token* makeNumber(Lexer* lex) {
    saveChar(lex, lex->currentChar);
	while (isDigit(peek(lex))) advanceWithSave(lex);

	TokenType semiType = TOKEN_INT;
	if (peek(lex) == '.') {
		semiType = TOKEN_FLOAT;
		advanceWithSave(lex);
		while (isDigit(peek(lex))) advanceWithSave(lex);
	}

	Token* toke = (Token*)malloc(sizeof(Token));

	toke->type = semiType;
	toke->length = lex->index - lex->start + 1;
	toke->lexeme = (char*)malloc(sizeof(char) * (toke->length + 1));
	strncpy(toke->lexeme, lex->saved, toke->length);
	toke->lexeme[toke->length] = '\0';
	toke->line = lex->line;
	toke->column = lex->column - toke->length;

	free(lex->saved);
	lex->saved = NULL;
	lex->savedSize = 0;
	lex->isChunkSize = 0;


	advance(lex); //prime next
	return toke;
}


Token* makeKeywordOrIdentifier(Lexer* lex) {
    saveChar(lex, lex->currentChar);
	advanceWithSave(lex); //first letter can only be alpha

	while (isIdentifier(peek(lex))) {
		advanceWithSave(lex);
	}
	// Scan for a keyword
	for (int i = 0; keywordTypes[i].keyword; i++) {
		if (strlen(keywordTypes[i].keyword) == lex->savedSize && !strncmp(keywordTypes[i].keyword, lex->saved, lex->savedSize)) {
			Token* toke = (Token*)malloc(sizeof(Token));

			toke->type = keywordTypes[i].type;
			toke->length = lex->savedSize;
			toke->lexeme = (char*)malloc(sizeof(char) * (toke->length + 1));
			strncpy(toke->lexeme, lex->saved, toke->length);
			toke->lexeme[toke->length] = '\0';
			toke->line = lex->line;
			toke->column = lex->column - toke->length;


			free(lex->saved);
			lex->saved = NULL;
			lex->savedSize = 0;
			lex->isChunkSize = 0;
			advance(lex);

			return toke;
		}
	}

	// Return an identifier
	Token* toke = (Token*)malloc(sizeof(Token));

	toke->type = TOKEN_IDENTIFIER;
	toke->length = lex->savedSize;
	toke->lexeme = (char*)malloc(sizeof(char) * (toke->length + 1));
	strncpy(toke->lexeme, lex->saved, toke->length);
	toke->lexeme[toke->length] = '\0';
	toke->line = lex->line;
	toke->column = lex->column - toke->length;
	free(lex->saved);
	lex->saved = NULL;
	lex->savedSize = 0;
	lex->isChunkSize = 0;

	advance(lex);

	return toke;
}

Token* makeString(Lexer* lex, char terminator) {
    saveChar(lex, lex->currentChar);
	advanceWithSave(lex);
	int startLine = lex->line;
	while (peek(lex) && peek(lex) != terminator) {
		//escaping strings
		if (peek(lex) == '\\') {
			advanceWithSave(lex);
		}

		advanceWithSave(lex);
	}

	advanceWithSave(lex); // Eat terminator

	if (isAtEnd(lex)) {
		return makeErrorToken(lex, "Unterminated string", startLine);
	}


	Token* toke = (Token*)malloc(sizeof(Token));

	toke->type = TOKEN_STRING;
	toke->length = lex->savedSize;
	toke->lexeme = (char*)malloc(sizeof(char) * (toke->length + 1));
	strncpy(toke->lexeme, lex->saved, toke->length);
	toke->lexeme[toke->length] = '\0';
	toke->line = lex->line;
	toke->column = lex->column - toke->length + 1;

	free(lex->saved);
	lex->saved = NULL;
	lex->savedSize = 0;
	lex->isChunkSize = 0;

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
	case '(': return makeToken(lex, LEFT_PAREN);
	case ')': return makeToken(lex, RIGHT_PAREN);
	case '{': return makeToken(lex, LEFT_BRACE);
	case '}': return makeToken(lex, RIGHT_BRACE);
	case '[': return makeToken(lex, LEFT_BRACKET);
	case ']': return makeToken(lex, RIGHT_BRACKET);
	case ';': return makeToken(lex, SEMICOLON);
	case ',': return makeToken(lex, COMMA);

	case '+': return makeToken(lex, match(lex, '=') ? TOKEN_PLUS_EQUAL : match(lex, '+') ? TOKEN_PLUS_PLUS : PLUS);
	case '-': return makeToken(lex, match(lex, '=') ? TOKEN_MINUS_EQUAL : match(lex, '-') ? TOKEN_MINUS_MINUS : match(lex, '>') ? TOKEN_MINUS_MORE : MINUS);
	case '*': return makeToken(lex, match(lex, '=') ? TOKEN_STAR_EQUAL : STAR);
	case '/': return makeToken(lex, match(lex, '=') ? TOKEN_SLASH_EQUAL : SLASH);
	case '%': return makeToken(lex, match(lex, '=') ? TOKEN_MODULO_EQUAL : TOKEN_MODULO);

	case '!': return makeToken(lex, match(lex, '=') ? BANG_EQUAL : BANG);
	case '=': return makeToken(lex, match(lex, '=') ? EQUAL_EQUAL : match(lex, '>') ? EQUAL_GREATER : EQUAL);

	case '>': return makeToken(lex, match(lex, '=') ? GREATER_EQUAL : GREATER);
	case '<': return makeToken(lex, match(lex, '=') ? LESS_EQUAL : LESS);

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
