#include "FlowGraphBuilder.h"

char* strCpy(char* src) {
	size_t tokenLen = strlen(src);
	char* tokenCopy = malloc(tokenLen + 1);

	#ifdef _WIN32
		strcpy_s(tokenCopy, tokenLen + 1, src);
	#else
		strcpy(tokenCopy, src);
	#endif

	tokenCopy[tokenLen] = '\0';
	return tokenCopy;
}

void analysis(Array* srcFiles, char* outputDir) {
	for (size_t i = 0; i < srcFiles->size; i++) {
		char* buffErrors = malloc(sizeof(char) * 1024);
		buffErrors[0] = '\0';

		SourceFile* srcFile = getItem(srcFiles, i);

		AstNode* rootAst = parseCustomLang(srcFile->fileName, buffErrors);

		int generateResult = generateDGML(rootAst, outputDir);

		if (generateResult >= 0) printf("DGML file succesfully generated\n");
		else printf("DGML file generation error\n");

		//printAst(root, 0);

		Array* funcNodes = buildArray(sizeof(AstNode), 5);
		dfs(rootAst, funcNodes, "FUNC_DEF");

		if (funcNodes->size == 0) {
			return;
		}

		Array* programUnitsStorage = buildArray(sizeof(ProgramUnit), 5);

		for (size_t i = 0; i < funcNodes->size; i++) {
			AstNode* funcNode = getItem(funcNodes, i);
			ProgramUnit programUnit;
			programUnit.sourceFile = srcFile;
			programUnit.funcSignature = buildFuncSignature(funcNode);
			programUnit.cfg = handleFunctionBody(funcNode);

			char* buff = malloc(sizeof(char) * 8);
			itoa(i, buff, 10);
			generateCfgDGML(programUnit.cfg, strcat(buff, ".dgml"));

			pushBack(programUnitsStorage, &programUnit);
		}



		if (strlen(buffErrors) > 0)
			printf(buffErrors);

		freeArray(&programUnitsStorage);
		freeArray(&funcNodes);
		freeAst(&rootAst);
		free(buffErrors);
	}
}

TypeRef* buildTypeRef(AstNode* rootTypeRefAst) {
	return NULL;
}

FuncSignature buildFuncSignature(AstNode* rootFuncAst) {
	FuncSignature signature;
	AstNode* funcSignNode = getItem(rootFuncAst->children, 0);
	AstNode* NameNode = getItem(funcSignNode->children, 0);

	signature.name = strCpy(NameNode->token);

	AstNode* ListArgsNode = getItem(funcSignNode->children, 1);
	if (ListArgsNode) {
		Array* args = ListArgsNode->children;
		if (args) {
			for (size_t i = 0; i < args->size; i++) {
				FuncArg* funcArg = (FuncArg*)malloc(sizeof(FuncArg));

				AstNode* argNode = getItem(args, i);
				Array* argChildren = argNode->children;
				AstNode* ArgNameNode = getItem(argChildren, 0);
				
				funcArg->name = strCpy(ArgNameNode->token);
				funcArg->typeRef = buildTypeRef(getItem(argChildren, 1));
			}
		}
	}

	return signature;
}


CfgNode* handleFunctionBody(AstNode* functionBodyAst) {
	breakTargets = buildArray(sizeof(CfgNode), 4);

	if (!functionBodyAst)
		return NULL;
	

	CfgNode* entryNode = createCfgNode("function entry");
	CfgNode* currentNode = entryNode;
	CfgNode* lastCfgNode = entryNode;


	for (size_t i = 1; i < functionBodyAst->children->size; i++) {
		AstNode* statement = getItem(functionBodyAst->children, i);
		CfgNode* statementNode = handleStatement(statement, &lastCfgNode);

		if (statementNode) {
			currentNode->condJump = statementNode;
			currentNode = lastCfgNode;
		}
	}

	lastCfgNode = currentNode;

	CfgNode* exitNode = createCfgNode("function exit");
	lastCfgNode->condJump = exitNode;

	freeArray(&breakTargets);

	return entryNode;
}


CfgNode* handleStatement(AstNode* rootStatementAst, CfgNode** lastCfgNode) {
	if (!rootStatementAst) 
		return NULL;

	AstNode* typeOfStatement = getItem(rootStatementAst->children, 0);

	if(typeOfStatement == NULL) {
		// error

		return NULL;
	}

	if (!strcmp(typeOfStatement->token, "BLOCK_STATEMENT")) {
		return handleBlockStatement(typeOfStatement, lastCfgNode);
	}
	else if (!strcmp(typeOfStatement->token, "IF_STATEMENT")) {
		return handleConditionStatement(typeOfStatement, lastCfgNode);
	}
	else if (!strcmp(typeOfStatement->token, "LOOP_STATEMENT")) {
		return handleLoopStatement(typeOfStatement, lastCfgNode);
	}
	else if (!strcmp(typeOfStatement->token, "BREAK_STATEMENT")) {
		handleBreakStatement(typeOfStatement, lastCfgNode);
	}
	else if (!strcmp(typeOfStatement->token, "REPEAT_STATEMENT")) {

	}
	else if (!strcmp(typeOfStatement->token, "EXPRESSION_STATEMENT")) {
		handleExpressionStatement(typeOfStatement, lastCfgNode);
	}
	else {
		// обработка ошибки

		return NULL;
	}
}

CfgNode* handleBlockStatement(AstNode* statementNodeAst, CfgNode** lastCfgNode) {
	CfgNode* newBlock = createCfgNode("block statement");
	CfgNode* currentBlock = newBlock;
	if (statementNodeAst) {
		for (size_t i = 0; i < statementNodeAst->children->size; i++) {
			AstNode* childStatement = getItem(statementNodeAst->children, i);
			CfgNode* result = handleStatement(childStatement, lastCfgNode);
			currentBlock->condJump = result;
			currentBlock = *lastCfgNode;
		}
	}
	else {
		// обработка ошибки

		return NULL;
	}

	*lastCfgNode = currentBlock;

	return newBlock;
}

CfgNode* handleConditionStatement(AstNode* statementNodeAst, CfgNode** lastCfgNode) {
	CfgNode* emptyBlock = createCfgNode("empty");
	CfgNode* newBlock = createCfgNode("condition statement");

	Array* children = statementNodeAst->children;

	AstNode* exprNode = getItem(children, 0);
	newBlock->opTree = handleExpression(exprNode);

	if (children->size == 2) {
		AstNode* condStatement = getItem(children, 1);
		newBlock->condJump = handleStatement(condStatement, lastCfgNode);
	}
	else if (children->size == 3) {
		AstNode* condStatement = getItem(children, 1);
		AstNode* uncondStatement = getItem(children, 2);
		newBlock->condJump = handleStatement(condStatement, lastCfgNode);
		newBlock->uncondJump = handleStatement(uncondStatement, lastCfgNode);
	}
	else {
		// обработка ошибки

		return NULL;
	}

	newBlock->condJump->condJump = emptyBlock;
	newBlock->uncondJump->condJump = emptyBlock;

	*lastCfgNode = emptyBlock;

	return newBlock;
}

CfgNode* handleLoopStatement(AstNode* statementNodeAst, CfgNode** lastCfgNode) {
	CfgNode* newBlock = createCfgNode("loop statement");
	CfgNode* exitBlock = createCfgNode("exit loop");
	CfgNode* currentBlock = newBlock;
	Array* children = statementNodeAst->children;

	AstNode* exprNode = getItem(children, 0);
	newBlock->opTree = handleExpression(exprNode);
	pushBack(breakTargets, exitBlock);
	if (children->size > 1) {
		for (size_t i = 1; i < children->size; i++) {
			AstNode* statement = getItem(children, i);
			CfgNode* result = handleStatement(statement, lastCfgNode);
			currentBlock->condJump = result;
			currentBlock = *lastCfgNode;
			if (result->label == "break statement") {
				break;
			}
		}
	}
	

	

	*lastCfgNode = currentBlock;

	return newBlock;
}

CfgNode* handleBreakStatement(AstNode* statementNodeAst, CfgNode** lastCfgNode) {
	CfgNode* newBlock = createCfgNode("break statement");

	CfgNode* result = popBack(breakTargets);
	newBlock->condJump = result;
	*lastCfgNode = result;

	return newBlock;
}

CfgNode* handleExpressionStatement(AstNode* statementNodeAst, CfgNode** lastCfgNode) {
	CfgNode* newBlock = createCfgNode("expression statement");

	newBlock->opTree = handleExpression(getItem(statementNodeAst->children, 0));
	
	*lastCfgNode = newBlock;

	return newBlock;
}


OpNode* handleExpression(AstNode* exprNode) {
	return NULL;
}