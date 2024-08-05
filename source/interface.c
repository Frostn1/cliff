#include "interface.h"
#define _CRT_SECURE_NO_WARNINGS

void c(int color) {
	printf("\033[0;%d;%dm", color, 1);
}
int handleInput(int argc, char* argv[], char** fileNamePtr) {
	int flag = 0;
	switch (argc) {
	case MIN_ARGC:
		flag = handleFlags(argv[1]);
		if (Unknown_Flag == flag) {
			flag = None_Flag;
			checkSrcFile(argv[1]);
			*fileNamePtr = (char*)malloc(sizeof(char) * strlen(argv[1]) + sizeof(char));
			strcpy(*fileNamePtr, argv[1]);
		}
		break;
	case MAX_ARGC:
		flag = handleFlags(argv[1]);
		if (Unknown_Flag == flag) {
			errorInterface("Invalid Flag.\n Try -h for help");
		}
		checkSrcFile(argv[2]);
		*fileNamePtr = (char*)malloc(sizeof(char) * strlen(argv[2]) + sizeof(char));
		strcpy(*fileNamePtr, argv[2]);
		break;
	default:
		errorInterface("Invalid Scheme\n.Please use the form of flag(optional) file.ourLanguage");
		break;
	}
	return flag;
}

void errorInterface(char* msg) {
	c(WHITE);
	printf("Error while processing file");
	c(NATRUAL);
	printf(" : ");
	c(ERROR);
	printf("%s\n", msg);
	c(NATRUAL);
	exit(0);
}

int handleFlags(char* flag) {
	if (!strcmp(flag, "-h") || !strcmp(flag, "--help")) {
		printf("Flag Options:\n");
		printf("\t-h/--help - For this menu\n");
		errorInterface("\tYou can run without any flags\n");
		//no need to return a thing because the program stops
	}
	else { return Unknown_Flag; };
}

void checkSrcFile(char* srcfileName)
{
	FILE* file;
	if (file = fopen(srcfileName, "r"))
	{
		int test = getc(file);
		if (test != EOF) {
			fclose(file);
			return;
		}
		else {
			errorInterface("Source file is empty");
		}
		fclose(file);
	}
	errorInterface("Source file doesnt exists");
}


char* getFileContent(char* filePath) {
	FILE* filePointer = fopen(filePath, "r");
	if (!filePointer) errorInterface("Can't open file");
	char* buffer = 0;
	long length;
	fseek(filePointer, 0, SEEK_END);
	length = ftell(filePointer);
	fseek(filePointer, 0, SEEK_SET);
	buffer = calloc(length, length);
	if (buffer) fread(buffer, 1, length, filePointer);
	fclose(filePointer);
	return buffer;
}