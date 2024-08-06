#include "token.h"
void freeToken(Token* toke) {
	if (!toke) return;
	free(toke->lexeme);
	free(toke);
}
