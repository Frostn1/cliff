#include <stdio.h>
#include "src/lexer/lexer.h"
#include "src/parser/parser.h"
#include "src/parser/statement.h"
#include "src/tool/visitor.h"

int main() {
    Lexer lexer;
    Parser parser;
    FILE* filePointer = fopen("./main.cliff", "r");
    newLexer(&lexer, filePointer);
    newParser(&parser,&lexer);
    while(!parser.isAtEnd(&parser)) {
        Statement* stat = parse(&parser);
        printf("%s", acceptStatment(stat));
    }
    return 0;
}
