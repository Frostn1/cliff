#include "CodeGen.h"
#include "Postfix.h"
#include "SymbolTable.h"

void newCodeGen(CodeGen* gen, char* path, ParseTree* mainTree, Table* table) {
	gen->filePath = (char*)malloc(sizeof(char) * LENGTH(path));
	strncpy(gen->filePath, path, LENGTH(path));
	gen->_main = mainTree;
	gen->table = table;

	gen->codeList = newStringList();
	gen->lcList = newLC_List();
	gen->funcList = newFunctionList();

	gen->loopCounter = 0;
	gen->conditionCounter = 0;

	loadFunctionBinaries(gen->funcList);
	return;
}

void freeCodeGen(CodeGen* gen) {
	gen->_main->freeParseTree(gen->_main);
	free(gen->filePath);
	fclose(gen->filePointer);
	gen->codeList->free(gen->codeList);
	freeFunctionList(gen->funcList);
	return;
}

FILE* CreateBlankFile(const char* path) {
	FILE* fp;
	char* asmPath = (char*)malloc(sizeof(char) * LENGTH(path));
	strncpy(asmPath, path, LENGTH(path));
	int pos = strlen(asmPath);
	for (size_t i = 0; i < strlen(asmPath); i++)
		if (asmPath[i] == '.') pos = i;
	asmPath[pos] = '\0';
	asmPath = (char*)realloc(asmPath, sizeof(char) * LENGTH(asmPath) + LENGTH(ASM_EXTENSION));
	strncat(asmPath, ASM_EXTENSION, LENGTH(ASM_EXTENSION));

	fp = fopen(asmPath, "w");
	fprintf(fp, "\t.file \"%s\"", path);
	fprintf(fp, "\n\t.intel_syntax noprefix\n\t.text\n\t.def\t___main;\t.scl  2;\t.type  32;\t.endef\n\t.section .rdata,\"dr\"\n\n.section .text");
	return fp;
}

void emitAsm(CodeGen* gen) {
	
	// Print LC Constants
	for (size_t i = 0; i < gen->lcList->size; i++)		printLC(gen->lcList->consts[i], i, gen->filePointer);
	
	// Print saved functions after constants
	for (size_t i = 0; i < gen->funcList->size; i++) {
		fprintf(gen->filePointer, "\n\n%s:\n", gen->funcList->funcs[i]->name);
		for (size_t j = 0; j < gen->funcList->funcs[i]->code->amount; j++)
			writeLine(gen->filePointer, gen->funcList->funcs[i]->code->strings[j]);
		writeLine(gen->filePointer, "\tRET");
	}

	writeLine(gen->filePointer, "\n_main:\n");
	writeLine(gen->filePointer, "\tPUSH ebp");
	writeLine(gen->filePointer, "\tMOV ebp, esp\n");
	

	// Print entire main function code
	for (size_t i = 0; i < gen->codeList->amount; i++)
		writeLine(gen->filePointer, gen->codeList->strings[i]);

	fputc('\n', gen->filePointer);
	writeLine(gen->filePointer, "\tMOV eax, 0");
	writeLine(gen->filePointer, "\tPOP ebp");
	writeLine(gen->filePointer, "\tRET");
	return;
}

void Generate(CodeGen* gen, Heap_List* list, ParseTree* current, StringList* codeList) {
	size_t index = 0;
	ParseTree* currentChild = current->getChild(current, index);
	Heap_List* heapList = list ? list : newHeap_List();
	int marginSize = 0;
	for (index = 0; index < current->amountOfChilds; index++, currentChild = current->getChild(current, index)) {

		switch (currentChild->type) {
		case VARIABLE_PARSE:
			CaseVariable(gen, heapList, currentChild, codeList);
			break;
		case ASSIGN_PARSE:
			CaseAssign(gen, heapList, currentChild, codeList);
			break;
		case FULL_LOOP_PARSE:
			CaseLoop(gen, heapList, currentChild, codeList);
			break;
		case FULL_CONDITIONAL_PARSE:
			CaseConditions(gen, heapList, currentChild, codeList);
			break;
		case FULL_FUNCTION_PARSE:
			marginSize = heapList->size;
			CaseFunctionDef(gen, heapList, currentChild, newStringList());
			Heap_ListDelLast(heapList, heapList->size - marginSize);
			break;
		case FULL_CALL_PARSE:
			CaseFunctionCall(gen, heapList, currentChild, codeList);
			break;
		case PARSE_RETURN_FULL:
			CaseExpression(gen, heapList, currentChild->getChild(currentChild, 1), codeList);
			break;
		}

	}
}

char* GetOPRow(CodeGen* gen, ParseTree* child, char* currentRow, StringList* codeList) {
	switch (child->type) {

		// Arithmetic
		case PARSE_PLUS: {
			currentRow = assembleRow(currentRow, "\tADD eax, ");
			break;
		}
		case PARSE_MINUS: {
			currentRow = assembleRow(currentRow, "\tSUB eax, ");
			break;
		}
		case PARSE_STAR: {
			currentRow = assembleRow(currentRow, "\tIMUL eax, ");
			break;
		}
		case PARSE_SLASH: {
			codeList->add(codeList, "\tCDQ");
			currentRow = assembleRow(currentRow, "\tIDIV ");
			break;
		}

		// Boolean
		// If not any arithmetic symbols are present it means that we had a boolean operation a step before
		default:
			currentRow = assembleRow(currentRow, "\tCMP eax, ");
			break;
	}
	return currentRow;
}
void PostToAsmExp(CodeGen* gen, Heap_List* heapList, ParseTree* current, StringList* codeList) {
	ExpressionFirst(gen, heapList, current->getChild(current, 0), codeList);
	ParseTree* stack[MAX_STACK_SIZE], * child = NULL;
	int top = EMPTY_STACK;
	char* currentRow = NULL;
	size_t i = 1;
	for (child = current->getChild(current, i); i < current->amountOfChilds; i++, child = current->getChild(current, i)) {
		switch (child->type) {
		case IDENTIFIER_PARSE:
		case ATOMIC_PARSE:
		case FULL_CALL_PARSE:
			stack[++top] = child;
			break;
		default:

			currentRow = GetOPRow(gen, child, currentRow, codeList);
			switch (stack[top]->type) {
			case IDENTIFIER_PARSE: {
				char marginString[MAX_DIGIT_LENGTH] = "";
				int margin = getHeap(heapList, stack[top]->token->lexeme)->margin;
				currentRow = assembleRow(currentRow, "DWORD PTR [ebp");
				sprintf(marginString, "%d", margin);
				if (margin > 0) currentRow = assembleRow(currentRow, "+");
				currentRow = assembleRow(currentRow, marginString);
				currentRow = assembleRow(currentRow, "]");
				break;
			}
			case ATOMIC_PARSE: {
				char numSTR[MAX_DIGIT_LENGTH] = "";
				switch (stack[top]->token->type) {
				case TOKEN_STRING:
					currentRow = assembleRow(currentRow, "OFFSET FLAT:.LC");
					sprintf(numSTR, "%d", getLCOffset(gen->lcList, stack[top]->token->lexeme));
					currentRow = assembleRow(currentRow, numSTR);
					LC_ListAdd(gen->lcList, newLC(LC_String, child->token->lexeme, child->token));
					break;
				case TOKEN_FLOAT:
					currentRow = assembleRow(currentRow, "DWORD PTR .LC");
					sprintf(numSTR, "%d", getLCOffset(gen->lcList, stack[top]->token->lexeme));
					currentRow = assembleRow(currentRow, numSTR);
					currentRow = assembleRow(currentRow, "[rip]");
					LC_ListAdd(gen->lcList, newLC(LC_Long, child->token->lexeme, child->token));
					break;
				case TOKEN_INT:
					currentRow = assembleRow(currentRow, stack[top]->token->lexeme);
					break;
				default:
					break;
				}
				break;
			case FULL_CALL_PARSE: {
				CaseFunctionCall(gen, heapList, stack[top], codeList);
				break;
			}	
			}
			}
			codeList->add(codeList, currentRow);
			currentRow = NULL;
		}
		
	}
	
	return;
}

void ExpressionFirst(CodeGen* gen, Heap_List* heapList, ParseTree* child, StringList* codeList) {
	switch (child->type) {
	case IDENTIFIER_PARSE: {
		char* currentRow = NULL, marginString[MAX_DIGIT_LENGTH] = "";
		int margin = getHeap(heapList, child->token->lexeme)->margin;
		currentRow = assembleRow(currentRow, "\tMOV eax, DWORD PTR [ebp");
		sprintf(marginString, "%d", margin);
		if (margin > 0) currentRow = assembleRow(currentRow, "+");
		currentRow = assembleRow(currentRow, marginString);
		currentRow = assembleRow(currentRow, "]");
		codeList->add(codeList, currentRow);
		break;
	}
	case ATOMIC_PARSE: {
		char* currentRow = NULL;
		char numSTR[MAX_DIGIT_LENGTH] = "";
		switch (child->token->type) {
		case TOKEN_STRING:
			currentRow = assembleRow(currentRow, "\tMOV eax, OFFSET FLAT:.LC");
			sprintf(numSTR, "%d", getLCOffset(gen->lcList, child->token->lexeme));
			currentRow = assembleRow(currentRow, numSTR);
			LC_ListAdd(gen->lcList, newLC(LC_String, child->token->lexeme, child->token));
			break;
		case TOKEN_FLOAT:
			currentRow = assembleRow(currentRow, "\tMOV eax, DWORD PTR .LC");
			sprintf(numSTR, "%d", getLCOffset(gen->lcList, child->token->lexeme));
			currentRow = assembleRow(currentRow, numSTR);
			currentRow = assembleRow(currentRow, "[rip]");
			LC_ListAdd(gen->lcList, newLC(LC_Long, child->token->lexeme, child->token));
			break;
		case TOKEN_INT:
			currentRow = assembleRow(currentRow, "\tMOV eax, ");
			currentRow = assembleRow(currentRow, child->token->lexeme);
			break;
		default:
			break;
		}
		codeList->add(codeList, currentRow);
		break;
	case FULL_CALL_PARSE: {
		CaseFunctionCall(gen, heapList, child, codeList);
		break;
	}
	}
	}
}

void CaseExpression(CodeGen* gen, Heap_List* heapList, ParseTree* current, StringList* codeList) {

	PostToAsmExp(gen, heapList, current, codeList);
	return;
}

char* getJmpCondition(ParseTreeType type) {
	// We return the opposite value of each jmp condition, so we can reverse the labels in the if block and reduce code size
	switch (type) {
	case PARSE_GREATER:
		return "\tjle"; // > jle was jg
	case PARSE_GREATER_EQUAL:
		return "\tjl"; // >= jl was jge
	case PARSE_LESS:
		return "\tjge"; // < jge was jl
	case PARSE_LESS_EQUAL:
		return "\tjg"; // <= jg was jle
	case PARSE_EQUAL_EQUAL:
		return "\tjne"; // == jne was je
	case PARSE_BANG_EQUAL:
		return "\tje"; // != je was jne
	}
}


void CaseVariable(CodeGen* gen, Heap_List* heapList , ParseTree* current, StringList* codeList) {

	switch (current->getChild(current, 0)->type) {
		case PARSE_INT_V: 
		case PARSE_FLOAT_V:
		case PARSE_STRING_V: {
			// Parsing the value of the variable
			CaseExpression(gen, heapList, current->getChild(current, 3), codeList);

			// making the actual row of creating the varialbe
			char* currentRow = NULL;
			int newMargin = getLast(heapList, SEARCH_VARIABLE);
			Heap_ListAdd(heapList, newHeap(HEAP_DWORD, current->getChild(current, 1)->token->lexeme, newMargin));
			currentRow = assembleRow(currentRow, "\tMOV DWORD PTR [ebp");
			if (newMargin > 0) currentRow = assembleRow(currentRow, "+");
			char* marginString = (char*)malloc((int)((ceil(log10(abs((int)newMargin))) + 2) * sizeof(char)));
			sprintf(marginString, "%d", newMargin);
			currentRow = assembleRow(currentRow, marginString);
			currentRow = assembleRow(currentRow, "], eax\n");
			free(marginString);
			codeList->add(codeList, currentRow);
			break;
		}
		
	}
	return;
}

void CaseAssign(CodeGen* gen, Heap_List* heapList, ParseTree* current, StringList* codeList) {
	/*
	* int
	    mov     DWORD PTR [ebp-4], 0
	* string
	    mov     DWORD PTR [ebp-16], OFFSET FLAT:.LC0
    * float    
		movss   xmm0, DWORD PTR .LC1[rip]
        movss   DWORD PTR [ebp-20], xmm0
	*/
	char* currentRow = NULL;
	char numSTR[MAX_DIGIT_LENGTH] = "";
	CaseExpression(gen, heapList, current->getChild(current, 2), codeList);
	
	ParseTree* firstChild = current->getChild(current, 0);
	int margin = getHeap(heapList, firstChild->token->lexeme)->margin;
	if (!strcmp(getValue(gen->table, (firstChild->token->lexeme))->variable->type, "string") || !strcmp(getValue(gen->table, (firstChild->token->lexeme))->variable->type, "int")){
		
		currentRow = assembleRow(currentRow, "\tMOV	DWORD PTR [ebp");
	}
	else if(!strcmp(getValue(gen->table, (firstChild->token->lexeme))->variable->type, "float")){
		
		currentRow = assembleRow(currentRow, "\tMOVSS	DWORD PTR [ebp");
	}
	if (margin > 0) currentRow = assembleRow(currentRow, "+");
	sprintf(numSTR, "%d", margin);
	currentRow = assembleRow(currentRow, numSTR);
	currentRow = assembleRow(currentRow, "], eax\n");
	codeList->add(codeList, currentRow);
}

void CaseLoop(CodeGen* gen, Heap_List* heapList, ParseTree* current, StringList* codeList) {
	/*
		start_loop:
		cmp eax, constant
		jmp condidtion end_loop
		*body*
		jmp start_loop
		end_loop:
		

	*/
	char numSTR[MAX_DIGIT_LENGTH] = "";
	char* currentRow = NULL;
	sprintf(numSTR, "%d", gen->conditionCounter);

	ParseTree* condition = current->getChild(current, 0);
	ParseTree* expression = condition->getChild(condition, 0);
	ParseTree* body = current->getChild(current, 1);

	
	currentRow = assembleRow(currentRow, "START_LOOP");
	currentRow = assembleRow(currentRow, numSTR);
	currentRow = assembleRow(currentRow, ":");
	codeList->add(codeList, currentRow);
	currentRow = NULL;

	CaseExpression(gen, heapList, expression, codeList);

	currentRow = assembleRow(currentRow, getJmpCondition(expression->getChild(expression, expression->amountOfChilds - 1)->type));
	currentRow = assembleRow(currentRow, " END_LOOP");
	currentRow = assembleRow(currentRow, numSTR);
	codeList->add(codeList, currentRow);
	currentRow = NULL;
	
	Generate(gen, heapList, body, codeList);

	currentRow = assembleRow(currentRow, "\tJMP START_LOOP");
	currentRow = assembleRow(currentRow, numSTR);
	currentRow = assembleRow(currentRow, "\n");
	codeList->add(codeList, currentRow);
	currentRow = NULL;

	currentRow = assembleRow(currentRow, "END_LOOP");
	currentRow = assembleRow(currentRow, numSTR);
	currentRow = assembleRow(currentRow, ":");
	codeList->add(codeList, currentRow);
	currentRow = NULL;

	gen->loopCounter++;
}

void CaseConditions(CodeGen* gen, Heap_List* heapList, ParseTree* current, StringList* codeList) {
	/*
		cmp     eax, consatnt
		jmp_condition else_body/end
		if_body
		jmp end_if
		*else:
			*else_body
		end_if:

	*/

	char numSTR[MAX_DIGIT_LENGTH] = "";
	char* currentRow = NULL;
	sprintf(numSTR, "%d", gen->conditionCounter);

	ParseTree* condition = current->getChild(current, 0);
	ParseTree* expression = condition->getChild(condition, 0);
	ParseTree* body = current->getChild(current, 1);


	CaseExpression(gen, heapList, expression, codeList);

	currentRow = assembleRow(currentRow, getJmpCondition(expression->getChild(expression, expression->amountOfChilds - 1)->type));
	currentRow = assembleRow(currentRow, current->amountOfChilds == 3 ? " MID_IF" : " END_IF");
	currentRow = assembleRow(currentRow, numSTR);
	codeList->add(codeList, currentRow);
	currentRow = NULL;

	// Converting Else Block
	

	Generate(gen, heapList, body, codeList);

	currentRow = assembleRow(currentRow, current->amountOfChilds == 3 ? "MID_IF" : "END_IF");
	currentRow = assembleRow(currentRow, numSTR);
	currentRow = assembleRow(currentRow, ":");
	codeList->add(codeList, currentRow);
	if (current->amountOfChilds == 3) {
		codeList->add(codeList, "\tJMP END_IF");
		Generate(gen, heapList, current->getChild(current, 2), codeList);
		codeList->add(codeList, "END_IF:");
	}
		

	// TODO : Reduce the extra variables that were created inside of the if/else block
	gen->conditionCounter++;
}


void writeLine(FILE* fp, const char* row) {
	//fputc('\t', fp);
	fwrite(row, sizeof(char), LENGTH(row), fp);
	fputc('\n', fp);
	return;
}

char* assembleRow(char* asmRow, char* newRow) {
	bool mallocFlag = asmRow ? true : false;
	asmRow = mallocFlag ? (char*)realloc(asmRow, sizeof(char) * strlen(asmRow) + LENGTH(newRow)) : (char*)malloc(sizeof(char) * LENGTH(newRow));
	mallocFlag ? strcat(asmRow, newRow) : strcpy(asmRow, newRow);
	return asmRow;
}

void toLower(char* string) {
	for (size_t i = 0; i < strlen(string); i++) {
		if (string[i] >= ASCII_CAPITAL_A && string[i] <= ASCII_CAPITAL_Z)
			string[i] = string[i] + (ASCII_SMALL_A - ASCII_CAPITAL_A);
	}
}


void CaseFunctionDef(CodeGen* gen, Heap_List* heapList, ParseTree* current, StringList* codeList) {
	FunctionDef* def = newFunctionDef(current->getChild(current, 0)->token->lexeme, codeList);
	// TODO : Generate function parameters code, will add when funciton calls are added, to know how the function argumants are calculated on the stack
	// getLast(heapList, SEARCH_VARIABLE);
	codeList->add(codeList, "\tPUSH ebp");
	codeList->add(codeList, "\tMOV ebp, esp\n");
	TABLE_VALUE* value = getValue(gen->table, current->getChild(current, 0)->token->lexeme);
	for (size_t i = 0; i < value->function->amount; i++) {
		Heap_ListAdd(heapList, newHeap(HEAP_DWORD, value->function->args[i].name, getLast(heapList, SEARCH_ARG)));
	}
	
	Generate(gen, heapList, current->getChild(current, 3), def->code); //  Generate code of the function block
	codeList->add(codeList, "\tPOP ebp");
	FunctionListAdd(gen->funcList, def);
	return;
}


void CaseFunctionCall(CodeGen* gen, Heap_List* heapList, ParseTree* current, StringList* codeList) {
	ParseTree* treeArgs = current->getChild(current, 1), * arg = treeArgs->getChild(treeArgs, 0);
	char* currentRow = NULL;
	for (size_t i = 0; i < treeArgs->amountOfChilds; i++, arg = treeArgs->getChild(treeArgs, i)) {
		CaseExpression(gen, heapList, arg, codeList);
		codeList->add(codeList, "\tPUSH eax");
	}
	currentRow = assembleRow(currentRow, "\tCALL ");
	currentRow = assembleRow(currentRow, current->getChild(current, 0)->token->lexeme);
	codeList->add(codeList, currentRow);
	return;
}




