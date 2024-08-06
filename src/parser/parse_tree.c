#include "parse_tree.h"

ParseTree* newTree(ParseTreeType type, Token* toke) {
	// Reset Tree
	ParseTree* tree = (ParseTree*)malloc(sizeof(ParseTree));
	tree->type = type;
	tree->token = toke;

	// Setup childs
	tree->children = (ParseTree**)malloc(sizeof(ParseTree*));
	tree->amountOfChilds = 0;

	// Setup methods
	tree->addChild = &__ADDCHILD__;
	tree->getChild = &__GETCHILD__;
	tree->delChild = &__DELCHILD__;
	tree->freeParseTree = &__FREEPARSETREE__;

	return tree;
}


void __ADDCHILD__(ParseTree* tree, ParseTree* child) {
	// Add another child spot
	tree->children = (ParseTree**)realloc(tree->children, sizeof(ParseTree*) * (tree->amountOfChilds + 1));
	tree->children[tree->amountOfChilds] = (ParseTree*)malloc(sizeof(ParseTree));
	tree->children[tree->amountOfChilds] = child;
	tree->amountOfChilds++;
}
ParseTree* __GETCHILD__(ParseTree* tree, int index) {
	return index >= tree->amountOfChilds || index < 0 ? NULL : tree->childs[index];
}

void __DELCHILD__(ParseTree* tree, ParseTree* child, bool freeFlag) {
	bool foundFlag = false;
	for (size_t i = 0; i < tree->amountOfChilds && !foundFlag; i++) {
		if (tree->children[i]->token && tree->children[i]->token->line == child->token->line && tree->children[i]->token->column == child->token->column) {
			ParseTree* del = tree->children[i];
			for (size_t j = i; j < tree->amountOfChilds - 1; j++) {
				tree->children[j] = tree->children[j + 1];
			}
			if(freeFlag) del->freeParseTree(del);
			foundFlag = true;
		}
	}
	if (foundFlag) {
		tree->children = (ParseTree**)realloc(tree->children, sizeof(ParseTree*) * (tree->amountOfChilds - 1));
		tree->amountOfChilds--;
	}

	return;
}

void __FREEPARSETREE__(ParseTree* tree) {
	if (!tree) return;
	if (tree->amountOfChilds) {
		for (size_t i = 0; i < tree->amountOfChilds; i++) {
			tree->getChild(tree, i)->freeParseTree(tree->getChild(tree, i));
		}
	}

	freeToken(tree->token);
	free(tree);
}
