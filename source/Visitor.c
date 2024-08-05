#include "Visitor.h"

Error_Codes newVisitor(Visitor* vis, Parser* par) {
	//vis = (Visitor*)malloc(sizeof(Visitor));
	if (vis == NULL) return VIS_MALLOC_FAIL;
	if (par->error) return VIS_PARSER_ERROR;
	vis->error = false;
	vis->par = par;
	return VIS_OK;
}
void throwError(Token* token, const char* message) {
	fprintf(stderr, "[Line %d] Error", token->line);

	//check type
	if (token->type == TOKEN_EOF) {
		fprintf(stderr, " at end");
	}

	else {
		fprintf(stderr, " near '%.*s'", token->length, token->lexeme);
	}

	//finally
	fprintf(stderr, ": %s\n", message);
}
void visitAll(Visitor* vis) {
	while (getParseTree(vis->par)) {
		// Check current ast
		// Returns most recent parseTree structure
		vis->error = vis->error || !visitAst(vis->par->table, vis->par->mainTree->getChild(vis->par->mainTree, vis->par->mainTree->amountOfChilds - 1));
	}
}

bool visitAst(Table* table, ParseTree* tree) {
	// Works close to how the statement funciton worked in the parser, as checking current tree and going down in levels if necessary
	if (!tree) { return false; };
	switch (tree->type) {
	case FULL_CALL_PARSE:
		return visitCall(table, tree);
	case VARIABLE_PARSE:
		return visitVariable(table, tree);
	case ASSIGN_PARSE:
		return visitAssign(table, tree);
	case FULL_FUNCTION_PARSE:
	case FULL_CONDITIONAL_PARSE:
	case FULL_LOOP_PARSE:
	{
		ParseTree* body = tree->getChild(tree, tree->type == FULL_FUNCTION_PARSE ? 2 : 1);
		for (int i = 0; i < body->amountOfChilds; i++)
		{
			if (!visitAst(table, body->getChild(body, i))) {
				return false;
			}
		}
		return true;
	}
	default:
		return false;
	}
}

bool visitCall(Table* table, ParseTree* tree) {
	// Check wheter call tree is ok or not with args
	ParseTree* argsTree = tree->getChild(tree, 1);
	TABLE_VALUE* val = getValue(table, tree->getChild(tree, 0)->token->lexeme);

	if (argsTree->amountOfChilds != val->function->amount) {
		// Args amount don't match
		throwError(tree->getChild(tree, 0)->token, "Function parameters amount mismatched");
		return false;
	}

	ParseTree* currentArg = argsTree->getChild(argsTree, 0);
	bool return_ = true;
	for (size_t i = 0; i < argsTree->amountOfChilds; i++, currentArg = argsTree->getChild(argsTree, i)) {
		if (!compareTypes(getTypeOfExpression(table, currentArg), val->function->args[i].type)) {
			throwError(currentArg->getChild(currentArg, 0)->token, "Type mismatch on function call");
			return_ = false;
		}
	}
	return return_;


}

bool visitExperssion(Table* table, ParseTree* tree) {
	ParseTree* child = tree->getChild(tree, 0);
	char* type= getTypeAsString(table, child);
	for (int i = 1; i < tree->amountOfChilds; i++) {
		ParseTree* child = tree->getChild(tree, i);
		if (!compareTypes(getTypeAsString(table, child), type)) {
			throwError(child->token, "Types dont match");
			return false;
		}
	}
	return true;
}

bool visitVariable(Table* table, ParseTree* tree) {
	if (!visitExperssion(table, tree->getChild(tree, 3))) {
		return false;
	}
	if (!compareTypes(getValue(table, tree->getChild(tree, 1)->token->lexeme)->variable->type, getTypeOfExpression(table, tree->getChild(tree, 3)))) {
		throwError(tree->getChild(tree, 3)->getChild(tree->getChild(tree, 3),0)->token, "Value type and variable type don't much");
		return false;
	}
	return true;
}
bool visitAssign(Table* table, ParseTree* tree) {
	if (!visitExperssion(table, tree->getChild(tree, 2))) {
		return false;
	}
	if (!compareTypes(getValue(table, (tree->getChild(tree, 0))->token->lexeme)->variable->type, getTypeOfExpression(table, tree->getChild(tree, 2)))) {
		throwError(tree->getChild(tree, 2), "Value type and variable type don't much");
		return false;
	}
	return true;
}

bool compareTypes(char* type1, char* type2) {
	if(!(type1[0] && type2[0])) return true;
	int floatIntCounter = 0;
	floatIntCounter += !strcmp(type1, "float") ? 1 : !strcmp(type1, "int") ? 1 : 0;
	floatIntCounter += !strcmp(type2, "float") ? 1 : !strcmp(type2, "int") ? 1 : 0;
	if (strcmp(type1, type2) && floatIntCounter != 2) 
		return false;
	return true;
}

char* getTypeOfExpression(Table* table, ParseTree* tree) {
	ParseTree* first = tree->type == EXPRESSION_PARSE ? tree->getChild(tree, 0) : tree;
	switch (first->type) {
	case IDENTIFIER_PARSE:
		return getValue(table, first->token->lexeme)->variable->type;
	case ATOMIC_PARSE: {
		switch (first->token->type) {
		case TOKEN_INT:
			return "int";
		case TOKEN_FLOAT:
			return "float";
		case TOKEN_STRING:
			return "string";
		}
		break;
	}
	break;
	case FULL_CALL_PARSE:
		return getValue(table, first->getChild(first, 0)->token->lexeme)->function->returnType;
	}
	return "void";
}

char* getTypeAsString(Table* table, ParseTree* child) {
	switch (child->type) {
	case PARSE_IDENTIFIER:
		return getValue(table, child->token->lexeme)->variable->type;
	case ATOMIC_PARSE:
		switch (child->token->type) {
			case TOKEN_INT:
				return "int";
			case TOKEN_FLOAT:
				return "float";
			case TOKEN_STRING:
				return "string";
		}
	case FULL_CALL_PARSE:
		return getValue(table, child->getChild(child, 0)->token->lexeme)->function->returnType;
	
	default:
		return "\0";
		break;
	}
}

bool getParseTree(Parser* par) {
	synchronize(par);
	if (par->current->type != TOKEN_EOF) {
		if (par->current->type != TOKEN_END_LINE) scanParser(par, par->mainTree);
		else parserAdvance(par);
		return true;
	}
	return false;
}
