#pragma once

#include "ProgramUnit.h"
#include "myParser.h"

Array* breakTargets;

char* strCpy(char* src);

void analysis(Array* srcFiles, char* outputDir);

TypeRef* buildTypeRef(AstNode* rootTypeRefAst);

FuncSignature buildFuncSignature(AstNode* rootFuncAst);

//CfgNode* buildCfg(AstNode* rootFuncAst);

CfgNode* handleStatement(AstNode* rootStatementAst, CfgNode** lastCfgNode);

CfgNode* handleBlockStatement(AstNode* statementNodeAst, CfgNode** lastCfgNode);

CfgNode* handleConditionStatement(AstNode* statementNodeAst, CfgNode** lastCfgNode);

CfgNode* handleLoopStatement(AstNode* statementNodeAst, CfgNode** lastCfgNode);

CfgNode* handleExpressionStatement(AstNode* statementNodeAst, CfgNode** lastCfgNode);

CfgNode* handleBreakStatement(AstNode* statementNodeAst, CfgNode** lastCfgNode);

OpNode* handleExpression(AstNode* exprNode);

//CfgNode* handleFunction(AstNode* functionAst);

CfgNode* handleFunctionBody(AstNode* functionBodyAst);
