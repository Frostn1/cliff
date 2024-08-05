#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "SymbolTable.h"

#define TEXT_CHARACTERISTICS 0x60300020
#define DATA_CHARACTERISTICS 0xc0300040
#define BSS_CHARACTERISTICS 0xc0300080
#define MAGE_SYM_DEBUG 0xFFFE
#define IMAGE_SYM_CLASS_FILE 0x67
#define DT_FUNCTION 0x67
#define IMAGE_SYM_CLASS_EXTERNAL 0x2
#define IMAGE_SYM_CLASS_STATIC 0x3

#define TEXT_SECTION_NUM 1
#define DATA_SECTION_NUM 2
#define BSS_SECTION_NUM 3

#define O_EXTENSION ".o"
#define COMPUTER_TYPE 0x14c
#define NAME_SIZE 8


typedef struct fileHeader {
	unsigned short pcType;
	unsigned short numSections;
	unsigned int timedate;
	unsigned int symbolTablePtr;
	unsigned int numOfSymbols;
	unsigned short OpHeaderSize;
	unsigned short flags;
}fileHeader;

typedef struct sectionHeadrer {
	unsigned char name[NAME_SIZE];
	unsigned int virtualSize;
	unsigned int virtualAddress;
	unsigned int rawDataSize;
	unsigned int dataAddress;
	unsigned int ptrToRelocation;
	unsigned int lineNumbrsPtr;
	unsigned short numOfRelocations;
	unsigned short numOfLineNums;
	unsigned int flags;
}sectionHeadrer;

typedef struct symbolTableSection {
	unsigned char name[NAME_SIZE];
	unsigned int value;
	unsigned short sectionNum;
	unsigned short type;
	unsigned char storageClass;
	unsigned char numOfAuxSymbols;
}symbolTableSection;

typedef struct relocationTableSection {
	unsigned char RVA[8];
	unsigned char symbolTableIndex[8];
	unsigned int type;
}relocationTableSection;

typedef struct ObjectFile
{
	char* filePath;
	FILE* filePointer;
	fileHeader* fileHeaders;
	sectionHeadrer* _textHeaders;
	sectionHeadrer* _dataHeaders;
	sectionHeadrer* _bssHeaders;
	char* _textSection;
	symbolTableSection** symbolTable;
	relocationTableSection** relocationTable;
	int symbolTableSize;
	int relocarionTableSize;

}ObjectFile;

void newObjectFile(ObjectFile* obj, char* path);
void freeObjectFile(ObjectFile* obj);
FILE* CreateObjFile(const char* path);
void setHeaders(ObjectFile* obj, int pcType, int numSections, int timeDate, int symbolTableOffset, int numSymbols, int SizeOptionalHeader, int flag);
void writeFile(ObjectFile* obj);
void setSectionHeaders(sectionHeadrer* sect, int virtualSize, int virtualAddress,	int rawDataSize,
	int dataAddress, int ptrToRelocation, int lineNumbrsPtr, short numOfRelocations, short numOfLineNums, int flags);
void addSymbol(ObjectFile* obj, char name[NAME_SIZE], int value, short sectionNum, short type, char storageClass, char numOfAuxSymbols);
void extractTextSegment(ObjectFile* obj, FILE* fp);