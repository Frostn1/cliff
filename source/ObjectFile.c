#include "ObjectFile.h"

void newObjectFile(ObjectFile* obj, char* path) {
	obj->filePath = (char*)malloc(sizeof(char) * LENGTH(path));
	strncpy(obj->filePath, path, LENGTH(path));
	obj->filePointer = CreateObjFile(path);
	obj->fileHeaders = malloc(sizeof(fileHeader));
	obj->_textHeaders = malloc(sizeof(sectionHeadrer));
	memset(obj->_textHeaders->name, 0, NAME_SIZE);
	strncpy(obj->_textHeaders->name, ".text", LENGTH(".text"));
	obj->_dataHeaders = malloc(sizeof(sectionHeadrer));
	memset(obj->_dataHeaders->name, 0, NAME_SIZE);
	strncpy(obj->_dataHeaders->name, ".data", LENGTH(".data"));
	obj->_bssHeaders = malloc(sizeof(sectionHeadrer));
	memset(obj->_bssHeaders->name, 0, NAME_SIZE);
	strncpy(obj->_bssHeaders->name, ".bss", LENGTH(".bss"));
	obj->_textSection = malloc(1);
	*obj->_textSection = '\0';
	obj->symbolTable = NULL;
	obj->relocationTable = NULL;
	obj->symbolTableSize = 0;
	obj->relocarionTableSize = 0;
}

void freeObjectFile(ObjectFile* obj) {
	free(obj->filePath);
	fclose(obj->filePointer);
	free(obj->fileHeaders);
	free(obj->_textHeaders);
	free(obj->_dataHeaders);
	free(obj->_bssHeaders);
	for (int i = 0; i < obj->symbolTableSize; i++)
	{
		free(obj->symbolTable[i]);
	}
	for (int i = 0; i < obj->relocationTable; i++)
	{
		free(obj->symbolTable[i]);
	}
}

FILE* CreateObjFile(const char* path) {
	FILE* fp;
	char* objPath = (char*)malloc(sizeof(char) * LENGTH(path));
	strncpy(objPath, path, LENGTH(path));
	int pos = strlen(objPath);
	for (size_t i = 0; i < strlen(objPath); i++)
		if (objPath[i] == '.') pos = i;
	objPath[pos] = '\0';
	objPath = (char*)realloc(objPath, sizeof(char) * LENGTH(objPath) + LENGTH(O_EXTENSION));
	strncat(objPath, O_EXTENSION, LENGTH(O_EXTENSION));

	fp = fopen(objPath, "wb+");
	return fp;
}

void writeFile(ObjectFile* obj) {
	fwrite(obj->fileHeaders, sizeof(fileHeader), 1, obj->filePointer);
	fwrite(obj->_textHeaders, sizeof(sectionHeadrer), 1, obj->filePointer);
	fwrite(obj->_dataHeaders, sizeof(sectionHeadrer), 1, obj->filePointer);
	fwrite(obj->_bssHeaders, sizeof(sectionHeadrer), 1, obj->filePointer);
	fwrite(obj->_textSection, 1, obj->_textHeaders->rawDataSize, obj->filePointer);
	for (int i = 0; i < obj->symbolTableSize; i++)
	{
		fwrite(obj->symbolTable[i], sizeof(symbolTableSection), 1, obj->filePointer);
	}
	for (int i = 0; i < obj->relocarionTableSize; i++)
	{
		fwrite(obj->relocationTable[i], sizeof(relocationTableSection), 1, obj->filePointer);
	}

}

void addSymbol(ObjectFile* obj, char name[NAME_SIZE], int value, short sectionNum, short type, char storageClass, char numOfAuxSymbols) {
	symbolTableSection* newSection = malloc(sizeof(symbolTableSection));
	memset(newSection->name, 0, NAME_SIZE);
	strncpy(newSection->name, name, strlen(name));
	newSection->value = value;
	newSection->sectionNum = sectionNum;
	newSection->type = type;
	newSection->storageClass = storageClass;
	newSection->numOfAuxSymbols = numOfAuxSymbols;
	if (!obj->symbolTableSize) {
		obj->symbolTable = (symbolTableSection**)malloc(++obj->symbolTableSize * sizeof(symbolTableSection*));
	}else{
		obj->symbolTable = (symbolTableSection**)realloc(obj->symbolTable, ++obj->symbolTableSize * sizeof(symbolTableSection*));
	}
	obj->symbolTable[obj->symbolTableSize - 1] = newSection;
}

void extractTextSegment(ObjectFile* obj, FILE* fp) {
	sectionHeadrer textHeaders;

	fseek(fp, sizeof(fileHeader), SEEK_SET);
	fread(&textHeaders, sizeof(textHeaders), 1, fp);
	obj->_textSection = malloc(textHeaders.rawDataSize);
	obj->_textHeaders->rawDataSize = textHeaders.rawDataSize;
	fseek(fp, textHeaders.dataAddress, SEEK_SET);
	fread(obj->_textSection, sizeof(unsigned char), textHeaders.rawDataSize, fp);
	fclose(fp);
	return;
}

void setSectionHeaders(sectionHeadrer* sect, int virtualSize, int virtualAddress, int rawDataSize,
	int dataAddress, int ptrToRelocation, int lineNumbrsPtr, short numOfRelocations, short numOfLineNums, int flag) {
	sect->virtualSize = virtualSize;
	sect->virtualAddress = virtualAddress;
	sect->rawDataSize = rawDataSize;
	sect->dataAddress = dataAddress;
	sect->ptrToRelocation = ptrToRelocation;
	sect->lineNumbrsPtr = lineNumbrsPtr;
	sect->numOfRelocations = numOfRelocations;
	sect->numOfLineNums = numOfLineNums;
	sect->flags = flag;
}

void setHeaders(ObjectFile* obj, int pcType, int numSections, int timeDate, int symbolTableOffset, int numSymbols, int SizeOptionalHeader, int flag)
{
	obj->fileHeaders->pcType = pcType;
	obj->fileHeaders->numSections = numSections;
	obj->fileHeaders->timedate = timeDate;
	obj->fileHeaders->symbolTablePtr = symbolTableOffset;
	obj->fileHeaders->numOfSymbols = numSymbols;
	obj->fileHeaders->OpHeaderSize = SizeOptionalHeader;
	obj->fileHeaders->flags = flag;
}

