#pragma once

#include "Array.h"
#include "cfg.h"
#include "SourceFile.h"

typedef enum BuiltIn {
	E_BOOL,
	E_BYTE,
	E_INT,
	E_UINT,
	E_LONG,
	E_ULONG,
	E_CHAR,
	E_STRING
};

typedef enum Type {
	E_BUILTIN,
	E_CUSTOM,
	E_ARRAY
};

typedef struct CustomType {
	char* nameOfType;
	size_t sizeOfType;
};

typedef struct BuiltInType {
	enum BuiltIn type;
	size_t sizeOfType;
} BuiltInType;

typedef struct ArrayType {
	size_t dim;
	struct BuiltInType basicType;
};

typedef struct TypeRef {
	void* ref;
	enum Type type;
} TypeRef;

typedef struct FuncArg {
	char* name;
	struct TypeRef* typeRef;
} FuncArg;

typedef struct FuncSignature {
	char* name;
	struct Array* FuncArgs;
} FuncSignature;

typedef struct ProgramUnit {
	struct FuncSignature funcSignature;
	struct CfgNode* cfg;
	struct TypeRef* typeRef;

	struct SourceFile* sourceFile;
} ProgramUnit;