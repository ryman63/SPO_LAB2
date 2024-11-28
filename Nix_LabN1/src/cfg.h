#pragma once
#include "Array.h"

//enum Operations {
//assign,
//
//};

typedef struct OpNode {
	int id;
	char* operation;
	Array* args;
} OpNode;

typedef struct CfgNode {
	int id;
	OpNode* opTree;
	CfgNode* uncondJump;
	CfgNode* condJump;
} CfgNode;