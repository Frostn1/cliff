#include "Debug.h"



void printToken(Token* token) {
	if (token->type == TOKEN_ERROR) {
		printf("Error\t%d-%d\t%.*s\n", token->line, token->column, token->length, token->lexeme);
		return;
	}

	printf("%d\t%d-%d\t", token->type, token->line, token->column);
	printf("%d-%d\t", token->line, token->column + token->length - 1);

	if (token->type == TOKEN_IDENTIFIER || token->type == TOKEN_INT || token->type == TOKEN_FLOAT || token->type == TOKEN_STRING) {
		printf("%.*s\n", token->length, token->lexeme);
	}
	else {
		char* keyword = findKeywordByType(token->type);

		if (keyword != NULL) {
			printf("%s\n", keyword);
		}
		else {
			printf("-\n");
		}
	}
}



void printTableValue(TABLE_VALUE* val) {
	switch (val->tag) {
	case FUNCTION_TAG:
		printf("FN << %s >>\t", val->function->returnType);
		for (size_t i = 0; i < val->function->amount; i++) {
			printf("%s (%s), ", val->function->args[i].type, val->function->args[i].name);
		}
		break;
	case VARIABLE_TAG:
		printf("VAR << %s >> [ %s ]\t", val->variable->type, val->variable->value);
		break;
	case ERROR_TAG:
		printf("error << %s >>\t", val->error->msg);
	default:
		return;
	}

	printf("%d-%d\n", val->line, val->column);
	return;
}

void printParseTree(ParseTree* tree) {
	if (!tree) return;

	printf("Type: %d\n", tree->type);
	if (tree->token) printToken(tree->token);
	printf("=> ");
	for (size_t i = 0; i < tree->amountOfChilds; i++) {
		if (tree->type == MAIN_PARSE) printf("\n");
		printParseTree(tree->getChild(tree, i));
	}
}




void printData(ParseTree* s) {
	printf(" %d", s->type);
}

void printAllChildrenData(ParseTree* s) {
	for (int i = 0; i < s->amountOfChilds; i++)
		printData(s->childs[i]);
}

void printBFS(ParseTree* s) {
	putchar('\n');
	printAllChildrenData(s);

	for (int i = 0; i < s->amountOfChilds; i++) {
		ParseTree* childCurr = s->getChild(s, i);
		printBFS(childCurr);
	}
}
void printTree(ParseTree* s) {
	printData(s);
	printBFS(s);
	putchar('\n');
}

/*
**             1
**       2        7        8
**    3    6            9     12
**   4 5              10 11
*/

/*static Tree nodes[] =
{
	[1] = {  1, 3, { &nodes[2], &nodes[7], &nodes[8] } },
	[2] = {  2, 2, { &nodes[3], &nodes[6], 0          } },
	[3] = {  3, 2, { &nodes[4], &nodes[5], 0          } },
	[4] = {  4, 0, { 0,          0,          0          } },
	[5] = {  5, 0, { 0,          0,          0          } },
	[6] = {  6, 0, { 0,          0,          0          } },
	[7] = {  7, 0, { 0,          0,          0          } },
	[8] = {  8, 2, { &nodes[9], &nodes[12], 0          } },
	[9] = {  9, 2, { &nodes[10], &nodes[11], 0          } },
	[10] = { 10, 0, { 0,          0,          0          } },
	[11] = { 11, 0, { 0,          0,          0          } },
	[12] = { 12, 0, { 0,          0,          0          } },
};

int main(void)
{
	printTree(&nodes[1]);
	return(0);
}*/
