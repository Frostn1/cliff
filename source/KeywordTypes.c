#include "KeywordTypes.h"

KeywordType keywordTypes[] = {
	{"else", TOKEN_ELSE},
	{"false", TOKEN_FALSE},
	{"for", TOKEN_FOR},
	{"if", TOKEN_IF},
	{"print", TOKEN_PRINT},
	{"return", TOKEN_RETURN},
	{"fn", TOKEN_FUNCTION},
	{"true", TOKEN_TRUE},
	{"loop", TOKEN_LOOP},

	// Variables Primitive Usable Types
	{"int", TOKEN_INT_V},
	{"float", TOKEN_FLOAT_V},
	{"string", TOKEN_STRING_V},

	{NULL, TOKEN_EOF}
};

char* findKeywordByType(TokenType type) {
	for(int i = 0; keywordTypes[i].keyword; i++) {
		if (keywordTypes[i].type == type) {
			return keywordTypes[i].keyword;
		}
	}

	return NULL;
}