#include "Postfix.h"



Token* pop(Token** stack, int* top) {
    if (top == EMPTY_STACK)
        return EMPTY_STACK;
    else
        return stack[(*top)--];
}

Token* peekPost(Token** stack, int* top) {
    if (top == EMPTY_STACK)
        return EMPTY_STACK;
    else
        return stack[(*top)];
}

int priority(Token* x) {
    if (x->type == TOKEN_LEFT_PAREN)
        return 0;
    if (x->type == TOKEN_GREATER || x->type == TOKEN_GREATER_EQUAL || x->type == TOKEN_LESS_EQUAL || x->type == TOKEN_LESS || x->type == TOKEN_EQUAL_EQUAL || x->type == TOKEN_BANG_EQUAL)
        return CON_PRIORITY;
    if (x->type == TOKEN_PLUS || x->type == TOKEN_MINUS)
        return PLUS_MINUS_PRIORITY;
    if (x->type == TOKEN_STAR || x->type == TOKEN_SLASH)
        return MUL_DIV_PRIORITY;
    return 0;
}

bool convertToPost(Parser* par, ParseTree* current, TokenType EO_Expr) {
    int openParenthesis = 0, numNiden = 0, operators = 0, top = EMPTY_STACK;
    Token* token = par->current, * stack[MAX_STACK_SIZE];
    ParseTree* child = NULL, * lastChild = NULL, * twoLastChild = NULL, * threeLastChild = NULL;
    int line = 0, column = 0, index = 0;
    char currentChar = 'a';

    while (par->current->type != EO_Expr && par->current->type != TOKEN_EOF) {
        if (par->current->type == TOKEN_INT || par->current->type == TOKEN_FLOAT || par->current->type == TOKEN_IDENTIFIER || par->current->type == TOKEN_STRING) {
            numNiden++;
            if ((numNiden - operators) >= 2) {
                error(par, par->current, "Too many operators");
                synchronize(par);
                return 0;
            }
            child = newTree(getType(par, par->current), par->current);
            child = foldTerms(current, child, stack, &top);
            current->addChild(current, child);
        }
        else if (par->current->type == TOKEN_LEFT_PAREN) {
            line = par->current->line, column = par->current->column, index = par->lex->index - strlen(par->current->lexeme), currentChar = par->current->lexeme[0];
            // Setting panic mode on, so no error calls will get printed
            // We only want to know if the case if valid or not
            par->panic = true;
            if (!parseCalls(par, current)) {
                
                numNiden = numNiden ? numNiden - 1 : numNiden;
                if (numNiden > operators) {
                    error(par, par->current, "Unexpected '('");
                    synchronize(par);
                    return false;
                }
                par->lex->line = line, par->lex->column = column, par->lex->index = index, par->lex->currentChar = currentChar;
                
                parserAdvance(par);
                openParenthesis++;
                stack[++top] = par->current;
            }
            else {
                current->delChild(current, current->getChild(current, current->amountOfChilds - 2), false);
            }
            // Unsetting back the panic flag so we get any additional error messages
            par->panic = false;
        }
        else if (par->current->type == TOKEN_RIGHT_PAREN)
        {
            if (openParenthesis == 0 || numNiden == operators) {
                error(par, par->current, "Unexpected ')'");
                synchronize(par);
                return false;
            }
            openParenthesis--;
            token = pop(stack, &top);
            while (top != -1 && token->type != TOKEN_LEFT_PAREN) {
                child = newTree(getType(par, token), token);
                current->addChild(current, child);
                token = pop(stack, &top);
            }
        }
        else {
            operators++;
            if (operators > numNiden) {
                error(par, par->current, "Invalid identifier");
                synchronize(par);
                return false;
            }
            while (top != EMPTY_STACK && priority(peekPost(stack, &top)) >= priority(par->current)) {
                token = pop(stack, &top);
                child = newTree(getType(par, token), token);
                child = foldTerms(current, child, stack, &top);
                current->addChild(current, child);
            }
            stack[++top] = par->current;
        }
        parserAdvance(par);
    }

    if (openParenthesis) {
        error(par, par->pre, "Unclosed parenthesis");
        synchronize(par);
        return false;
    }
    while (top != EMPTY_STACK) {
        token = pop(stack, &top);
        ParseTree* child = newTree(getType(par, token), token);
        child = foldTerms(current, child, stack, &top);
        current->addChild(current, child);
    }

    return true;
}

ParseTreeType getType(Parser* par, Token* token) {
    switch (token->type)
    {
    case TOKEN_IDENTIFIER:
        return IDENTIFIER_PARSE;

    case TOKEN_INT:
    case TOKEN_FLOAT:
    //case TOKEN_STRING:
        return ATOMIC_PARSE;
    case TOKEN_STRING:
        return PARSE_STRING;

    case TOKEN_PLUS:
        return PARSE_PLUS;
    case TOKEN_MINUS:
        return PARSE_MINUS;
    case TOKEN_STAR:
        return PARSE_STAR;
    case TOKEN_SLASH:
        return PARSE_SLASH;

    case TOKEN_GREATER:
        return PARSE_GREATER;
    case TOKEN_GREATER_EQUAL:
        return PARSE_GREATER_EQUAL;
    case TOKEN_LESS:
        return PARSE_LESS;
    case TOKEN_LESS_EQUAL:
        return PARSE_LESS_EQUAL;
    case TOKEN_EQUAL_EQUAL:
        return PARSE_EQUAL_EQUAL;
    case TOKEN_BANG_EQUAL:
        return PARSE_BANG_EQUAL;

    default:
        error(par, token, "Unexpected operator");
        synchronize(par);
        return PARSE_ERROR;
    }
}

ParseTree* foldTerms(ParseTree* currentTree, ParseTree* child, Token* stack[], int* top) {
    ParseTree* lastChild = currentTree->getChild(currentTree, currentTree->amountOfChilds - 1);
    ParseTree* twoLastChild = currentTree->getChild(currentTree, currentTree->amountOfChilds - 2);
    ParseTree* threeLastChild = currentTree->getChild(currentTree, currentTree->amountOfChilds - 3);
    bool foldFlag = false;
    ParseTree* first = NULL, * second = NULL, * sign = NULL;
    if (!(lastChild && twoLastChild)) return child;
    // Check here if last child is a constant and then fold it with top stack
    if (lastChild->type == ATOMIC_PARSE && child->type != ATOMIC_PARSE && twoLastChild->type == ATOMIC_PARSE) {
        first = lastChild;
        second = twoLastChild;
        sign = child;
        foldFlag = true;
    }
    else if (threeLastChild && lastChild->type != ATOMIC_PARSE && child->type == ATOMIC_PARSE && (twoLastChild->type == ATOMIC_PARSE || threeLastChild->type == ATOMIC_PARSE) && priority(peekPost(stack, top)) == priority(lastChild->token)) {
        first = twoLastChild->type == ATOMIC_PARSE ? twoLastChild : threeLastChild;
        second = child;
        sign = lastChild;
        foldFlag = true;
    }
    else {
        sign = child;
    }

    if (foldFlag) {
        // Fold constants
        // 3 + 5
        // 35+
        // twoLastChild 5
        // lastChild 3
        // child +
        float firstConst = atof(second->token->lexeme);
        float secondConst = atof(first->token->lexeme);
        float out = 0;
        switch (sign->type) {

            // Arithmetic
        case PARSE_PLUS:
            out = firstConst + secondConst;
            break;
        case PARSE_MINUS:
            out = firstConst - secondConst;
            break;
        case PARSE_STAR:
            out = firstConst * secondConst;
            break;
        case PARSE_SLASH:
            out = firstConst / secondConst;
            break;

            // Boolean
        case PARSE_AND_AND:
            out = firstConst && secondConst;
            break;
        case PARSE_OR_OR:
            out = firstConst || secondConst;
            break;
        case PARSE_GREATER:
            out = (int)(firstConst - secondConst) > 0;
            break;
        case PARSE_GREATER_EQUAL:
            out = (int)(firstConst - secondConst) >= EPSILON_PRECISION;
            break;
        case PARSE_LESS:
            out = (int)(firstConst - secondConst) < 0;
            break;
        case PARSE_LESS_EQUAL:
            out = (int)(firstConst - secondConst) <= EPSILON_PRECISION;
            break;
        case PARSE_EQUAL_EQUAL:
            out = (int)(firstConst - secondConst) < EPSILON_PRECISION;
            break;
        case PARSE_BANG_EQUAL:
            out = (int)(firstConst - secondConst) > EPSILON_PRECISION;
            break;
        }
        Token* foldToken = (Token*)malloc(sizeof(Token));
        foldToken->column = first->token->column;
        foldToken->line = first->token->line;
        foldToken->type = out == (int)out ? TOKEN_INT : TOKEN_FLOAT;
        foldToken->lexeme = (char*)malloc((int)((ceil(log10((int)out)) + 1) * sizeof(char)) + 3);
        out == (int)out ? sprintf(foldToken->lexeme, "%d", (int)out) : sprintf(foldToken->lexeme, "%.3f", out);
        foldToken->length = strlen(foldToken->lexeme);

        currentTree->delChild(currentTree, sign);

        sign = newTree(ATOMIC_PARSE, foldToken);
        currentTree->delChild(currentTree, first); // Del specific child
        currentTree->delChild(currentTree, second);
    }
    return sign;
}
