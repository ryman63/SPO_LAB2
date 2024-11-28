// myParser.h

#pragma once


#include <antlr3.h>
#include <antlr3defs.h>
#include <antlr3collections.h>
#include "custom_langParser.h"
#include "custom_langLexer.h"
#include "ast.h"

void fillAst(pANTLR3_BASE_TREE tree, AstNode* root);

AstNode* parseCustomLang(char* inputFilePath, char* buffErrors);

void handleParserError(pANTLR3_BASE_RECOGNIZER recognizer, char* buffer);

void printAst(AstNode* root, size_t depth);

void fixAst(AstNode* root);