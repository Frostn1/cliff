#include "keyword.h"

KeywordType keywordTypes[] = {
	{"else", ELSE},
	{"false", FALSE},
	{"for", FOR},
	{"if", IF},
	{"print", PRINT},
	{"return", RETURN},
	{"fn", FUNCTION},
	{"true", TRUE},
	{"loop", LOOP},
	{"nill", NILL},

	// Variables Primitive Usable Types
	// {"int", TOKEN_INT_V},
	// {"float", TOKEN_FLOAT_V},
	// {"string", TOKEN_STRING_V},

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
