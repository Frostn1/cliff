#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "Lexer.h"
#include "Parser.h"
#include "interface.h"
#include "Visitor.h"
#include "CodeGen.h"
#include "Assembler.h"



int main( int argc, char** argv ) {

    char* srcFileName = NULL;
    handleInput(argc, argv, &srcFileName);
    
    Lexer lex;
    
    Parser par;
    Visitor vis;
    CodeGen gen;
    Assembler asm;

    newLexer(&lex, getFileContent(srcFileName));
    newParser(&par, &lex);
    Error_Codes code = newVisitor(&vis, &par);
    if (code != VIS_OK) {
        c(RED);
        fprintf(stderr, "Error while parsing trees\n");
        c(GRAY);
        return 1;
    }
    visitAll(&vis);
    if (vis.error) {
        c(RED);
        fprintf(stderr, "Error while visiting trees\n");
        c(GRAY);
        return 1;
    }
    newCodeGen(&gen, srcFileName, vis.par->mainTree, vis.par->table);
    gen.filePointer = CreateBlankFile(gen.filePath);
    Generate(&gen, NULL, gen._main, gen.codeList);
    emitAsm(&gen);
    freeCodeGen(&gen);
    newAssembler(&asm, srcFileName, par.table);
    runAssembler(&asm);
    freeAssembler(&asm);
    return 0;
}
