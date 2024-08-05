#pragma once
#include <assert.h>
#include <stdio.h>
//#include <Windows.h>
//#include <ShellApi.h>
#include <conio.h>

#include "ObjectFile.h"

#define BINARY_FILE_LIMIT_INT 10000
#define BINARY_FILE_LIMIT_STRING 5
#define CMD_COMMAND_LIMIT 512

#define S_EXTENSION ".s"

typedef struct Assembler {
	Table* table;
	ObjectFile* _obj;

	FILE* binaryCodeFilePointer; // File pointer given by the visitor, which points to the file which holds the binary code for all code ( main and functions )
	char* binaryCodeFilePath; // File Path to the file which holds the binary code for all code ( main and functions )

	char* asmCodeFilePath; // File Path to the file which holds the asm code for all code ( main and functions )
}Assembler;

void newAssembler(Assembler* asm, char* path, Table* table);
void freeAssembler(Assembler* asm);

// --------------------

void runAssembler(Assembler* asm); // Function sets the values of objectFile, and manages the conversion of the asm code
void execGCC(Assembler* asm);

// --------------------
// Tooling

char* GenerateRandomBinFile(char* path);
int RandInt(int n); // Returns an integer in the range [0, n)
int FindFileExtPos(char* path);
