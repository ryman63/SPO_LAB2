#include "myParser.h"

void fillAst(pANTLR3_BASE_TREE tree, AstNode* root) {
    AstNode* node = insert
        (root, 
        (const char*) tree->getText(tree)->chars, 
        (int)tree->getLine(tree));
    size_t childCount = tree->getChildCount(tree);

    for (size_t i = 0; i < childCount; i++) {
        fillAst(tree->getChild(tree, i), node);
    }
}

void handleParserError(pANTLR3_BASE_RECOGNIZER recognizer, char* buffer) {
    pANTLR3_EXCEPTION exception = recognizer->state->exception;

    if (exception) {
        snprintf(buffer,1024, "Erron on line %d, position %d: %s\n",
            exception->line,
            exception->charPositionInLine,
            (char*)exception->message);

        // Сброс ошибки после обработки
        recognizer->state->exception = NULL;
    }
}

AstNode* parseCustomLang (char* inputFilePath, char* buffErrors) {
    
    pANTLR3_INPUT_STREAM  input;
    pcustom_langLexer lexer;
    pcustom_langParser parser;
    pANTLR3_COMMON_TOKEN_STREAM tokens;

    input = antlr3FileStreamNew((pANTLR3_UINT8)inputFilePath, ANTLR3_ENC_UTF8);
    lexer = custom_langLexerNew(input);
    tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
    parser = custom_langParserNew(tokens);

    custom_langParser_source_return result = parser->source(parser);
    pANTLR3_BASE_TREE tree = result.tree;

    handleParserError(parser->pParser->rec, buffErrors);

    AstNode* root = createNode("source");

    fillAst(tree, root);
    //fixAst(root);

    parser->free(parser);
    tokens->free(tokens);
    lexer->free(lexer);
    input->close(input);

    return root;
}

void printAst(AstNode* root, size_t depth)
{
    for (size_t i = 0; i < depth; i++) {
        printf(" ");
    }

    printf(root->token);
    printf("\n");

    depth++;

    for (size_t j = 0; j < root->children->size; j++) {
        printAst(getItem(root->children, j), depth);
    }
}


void fixAst(AstNode* node) {

    Array* storage = buildArray(sizeof(AstNode), 1);
    dfs(node, storage, "RANGE_LIST");

    for (size_t i = 0; i < storage->size; i++) {

        AstNode* item = getItem(storage, i);
        Array* children = item->children;
        size_t dimensions = children->size;

        if (dimensions > 1)
        {
            // сохраняем название массива и удаляем его из ноды SLICE
            AstNode* parent = item->parent;
            AstNode* identifierOfArray = popBack(parent->children);
            
            //проходимся по всем аргументам листа в обратном порядке кроме последнего
            for (int j = children->size - 2; j >= 0; j--) {
                // забираем аргумент
                AstNode* expr = removeAt(children, j);
                
                // создаём недостающие ноды
                AstNode* createSliceNode = createNode("SLICE_EXPR");
                AstNode* createRangeListNode = createNode("RANGE_LIST");

                // делаем привязки
                addChild(createSliceNode, createRangeListNode);
                addChild(createRangeListNode, expr);
                addChild(parent, createSliceNode);

                // на предпоследней итерации привязываем название массива
                if (children->size == 1) {
                    addChild(createSliceNode, identifierOfArray);
                }

                // спускаемся ниже
                parent = createSliceNode;
            }
        }
    }

    freeArray(&storage);
}