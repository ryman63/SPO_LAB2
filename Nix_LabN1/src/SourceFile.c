#include "SourceFile.h"

SourceFile* GetSrcFile(char* fileName) {
	SourceFile* srcFile = malloc(sizeof(SourceFile));
	srcFile->fileName = fileName;
	srcFile->id = id_counter;
	id_counter++;
	return srcFile;
}