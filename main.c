#include <stdio.h>
// #include "debug.h"
#include "src/lexer/lexer.h"
#include "src/parser/parser.h"
#include "src/tool/visitor.h"
// #include "src/tool/visitor.h"

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
    Parser parser;
    FILE* filePointer = fopen("./main.cliff", "r");
    newLexer(&lexer, filePointer);
    newParser(&parser,&lexer);
    Expr* expr = parse(&parser);
    printf("%s", acceptExpr(expr));
    return 0;
}
