#pragma once
#include "Array.h"
#include <libxml/tree.h>
#include <libxml/parser.h>
//enum Operations {
//assign,
//
//};

static int cfg_id_counter = 0;
static int opNode_id_counter = 0;

typedef struct OpNode {
	int id;
	char* operation;
	struct Array* args;
} OpNode;

typedef struct CfgNode {
	int id;
	int line;
	char* label;
	struct OpNode* opTree;
	struct CfgNode* uncondJump;
	struct CfgNode* condJump;
} CfgNode;

CfgNode* createCfgNode(char* label, int line);

OpNode* createOpNode(char* operation);

int generateCfgDGML(CfgNode* root, const char* filename);

void addNodesAndLinksCfg(xmlNodePtr nodes, xmlNodePtr links, CfgNode* node);

int generateOpTreeDGML(OpNode* root, const char* filename);

char* strCpy(char* src);

void TraverseCfg(CfgNode* root, int(*operation)(OpNode*, char*));

void addNodesAndLinksOpTree(xmlNodePtr nodes, xmlNodePtr links, OpNode* node);

