#pragma once

#include "Array.h"
#include "cfg.h"
#include "SourceFile.h"

typedef struct TypeRef {
	
};

typedef struct FuncArg {
	char* name;
	TypeRef typeRef;
} FuncArg;

typedef struct FuncSignature {
	char* name;
	Array* FuncArgs;
} FuncArg;

typedef struct ProgramUnit {
	FuncSignature funcSignature;
	CfgNode* cfg;

	SourceFile* sourceFile;
} ProgramUnit;