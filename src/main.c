#include <stdio.h>
#include "debug.h"
#include "lexer/lexer.h"

char* getFileContent(char* filePath) {
	FILE* filePointer = fopen(filePath, "r");
	if (!filePointer) printf("Can't open file");
	char* buffer = 0;
	long length;
	fseek(filePointer, 0, SEEK_END);
	length = ftell(filePointer);
	fseek(filePointer, 0, SEEK_SET);
	buffer = calloc(length, length);
	if (buffer) fread(buffer, 1, length, filePointer);

	return buffer;
}

int main() {
    Lexer lexer;
    FILE* filePointer = fopen("./main.cliff", "r");
    newLexer(&lexer, filePointer);
    Token* token = NULL;
    do {
        printToken(token);
    } while((token = scanLexer(&lexer))->type != TOKEN_EOF);
    // fclose(filePointer);
    // freeLexer(&lexer);
}
