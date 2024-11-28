#include <stdio.h>
#include "myParser.h"

int string_exists(const char* str) {
	return str && str[0] != '\0'; // ѕровер€ем, что указатель не NULL и строка не пуста€
}

int main(int argc, char* argv[]) {
	//#ifdef _WIN32
	//	freopen("NUL", "w", stderr);
	//#else
	//	freopen("/dev/null", "w", stderr);
	//#endif

	char* inputFilePath = argv[1];
	char* outputFilePath = argv[2];
	
	if (!string_exists(inputFilePath) || !string_exists(outputFilePath)) {
		printf("invalid args");
		return -1;
	}

	char* buffErrors = malloc(sizeof(char) * 1024);
	buffErrors[0] = '\0';
	
	AstNode* root = parseCustomLang(inputFilePath, buffErrors);

	//printAst(root, 0);

	if(strlen(buffErrors) > 0)
		printf(buffErrors);

	int generateResult = generateDGML(root, outputFilePath);

	if (generateResult >= 0) printf("DGML file succesfully generated\n");
	else printf("DGML file generation error\n");

	freeAst(&root);

	system("pause");

	return 0;
}