#ifndef LEXER_H
#define LEXER_H
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "token_type.h"
#include "keyword.h"
#include "token.h"


#define DONT_OVERWRITE_BASELINE -1
#define CHUNK_SIZE 32


typedef struct Lexer {

    int line, column, index, size, start, chunkIndex;
    char currentChar; // Current character the lexer is 'standing' on to check against
    FILE* file;
    char chunk[CHUNK_SIZE];
    char nextChunk[CHUNK_SIZE];


    char* saved;
    int isChunkSize; // Size from 0 till CHUNK_SIZE, to know when to realloc more
    int savedSize; // Full size, for strncat

} Lexer;

void cleanLexer(Lexer* lex);
// Iniatiates a lexer with blank values
void newLexer(Lexer* lex, FILE* file);
bool isAtEnd(Lexer* lex);
// Peeks next character
char peek(Lexer* lex);
// Shows current character
char show(Lexer* lex);
/**
 * @lex -
 Lexer to advance
 Advances to the next character in the raw code text
**/
char advance(Lexer* lex);
char advanceWithSave(Lexer* lex);
void saveChar(Lexer* lex, char c);
// Checks if a char is a digit
bool isDigit(char c);
// Checks if a char is a letter
bool isAlpha(char c);
// Checks if a charh is a letter or _
bool isIdentifier(char c);
// Makes a new token of some type enterd in the arg `type`
Token* makeToken(Lexer* lex, TokenType type);
// Scans lexer for next possible token,
// if not found will rerturn error token,
// and if at end will return EOF token,
// otherwise whatever token found
Token* scanLexer(Lexer* lex);
// Eats uneccessery white space infront of next token
void eatWhiteSpace(Lexer* lex);
// Makes token of type number
Token* makeNumber(Lexer* lexer);
// Makes token of type string
Token* makeString(Lexer* lexer, char terminator);
// Makes token of an identifier or a saved keyword
Token* makeKeywordOrIdentifier(Lexer* lexer);
// Makes token of type error with an error message to display
Token* makeErrorToken(Lexer* lexer, char* msg, int startLine);
// Checks wheter current character in lexer in equal to char in arg and return bool
bool match(Lexer* lexer, char c);
// Free lexer's memory
void freeLexer(Lexer* lex);
#endif // !LEXER_H
