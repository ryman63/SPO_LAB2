// ast.h
#pragma once

#include <stdio.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <string.h>
#include "malloc.h"
#include "Array.h"

static int global_id_counter = 0;

typedef struct AstNode {
	int id;
	char* token;
	struct AstNode* parent;
	struct Array* children;
} AstNode;

AstNode* insert(AstNode* root, const char* token);

void AddChild(AstNode* parent, AstNode* child);

AstNode* createNode(const char*);

void freeAst(AstNode** root);

void freeNode(AstNode* node);

AstNode* dfs(AstNode* root, struct Array* arr, char* value);

void addNodesAndLinks(xmlNodePtr nodes, xmlNodePtr links, AstNode* node);

int generateDGML(AstNode* root, const char* filename);
