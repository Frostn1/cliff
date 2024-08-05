#ifndef KEYWORD_TYPES_H
#define KEYWORD_TYPES_H
#include <stdio.h>
#include <stdlib.h>
#include "token_type.h"

typedef struct {
	char* keyword;
	TokenType type;
} KeywordType;

extern KeywordType keywordTypes[];

char* findKeywordByType(TokenType type);

#endif // !KEYWORD_TYPES
