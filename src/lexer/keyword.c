#include "keyword.h"

KeywordType keywordTypes[] = {
	{"if", IF},
	{"else", ELSE},
	{"true", TRUE},
	{"false", FALSE},
	{"for", FOR},
	{"loop", LOOP},
	{"fn", FUNCTION},
	{"print", PRINT},
	{"return", RETURN},
	{"nill", NILL},
	{NULL, T_EOF}
};

char* findKeywordByType(TokenType type) {
	for(int i = 0; keywordTypes[i].keyword; i++) {
		if (keywordTypes[i].type == type) {
			return keywordTypes[i].keyword;
		}
	}

	return NULL;
}
