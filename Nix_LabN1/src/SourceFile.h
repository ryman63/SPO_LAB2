#pragma once
#include "Array.h"

static int id_counter = 0;

typedef struct SourceFile {
	int id;
	char* fileName;
	char* filePath;
} SourceFile;

SourceFile* GetSrcFile(char* fileName);