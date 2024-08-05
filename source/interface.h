#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#define MIN_ARGC 2
#define MAX_ARGC 3
#define MAX_STRING 3
#define FILE_ENDING "ourLanguage"
#define DARK_GRAY 30
#define RED 31
#define WHITE 37
#define LIGHT_GREEN 32
#define ORANGE 33
#define BLUE 34
#define LIGHT_PURPLE 35
#define LIGHT_BLUE 36
#define GRAY 38

#define ERROR RED
#define NATRUAL GRAY
#define BOLD WHITE
enum Flags {
	Unknown_Flag,
	None_Flag,
	Help_Flag
};

// Main function of the interface handler, returns a flag to check against in case of error
int handleInput(int argc, char* argv[], char** fileNamePtr);

int handleFlags(char* flag);
void checkSrcFile(char* srcfileName);
char* getFileContent(char* filePath);
void c(int color);
void errorInterface(char* msg);