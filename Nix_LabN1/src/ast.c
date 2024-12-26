#include "ast.h"

AstNode* insert(AstNode* root, const char* token, int line)
{
	AstNode* newNode = createNode(token);
	newNode->line = line;

	newNode->parent = root;

	pushBack(root->children, newNode);

	return newNode;
}

void addChild(AstNode* parent, AstNode* child)
{
	child->parent = parent;
	pushBack(parent->children, child);
}

AstNode* createNode(const char* token)
{
	AstNode* newNode = (AstNode*) malloc(sizeof(AstNode));

	size_t tokenLen = strlen(token);
	char* tokenCopy = malloc(tokenLen + 1);

	#ifdef _WIN32
		strcpy_s(tokenCopy, tokenLen + 1, token);
	#else
		strcpy(tokenCopy, token);
	#endif
	
	tokenCopy[tokenLen] = '\0';
	newNode->token = tokenCopy;

	newNode->parent = NULL;

	newNode->id = global_id_counter;

	newNode->children = buildArray(sizeof(AstNode), 1);

	global_id_counter++;
	return newNode;
}

void freeAst(AstNode** root)
{
	freeNode(*root);
	*root = NULL;
}

void freeNode(AstNode* node)
{
	Array* children = (node)->children;

	if (children->size > 0) {
		for (size_t i = 0; i < children->size; i++) {
			freeNode(getItem(children, i));
		}
		freeArray(&children);
		free(node->token);
	}

	//free(node);
}

void dfs(AstNode* root, Array* arr, char* value)
{
	if (!strcmp(root->token, value)) {
		pushBack(arr, root);
	}

	Array* children = root->children;
	for (size_t i = 0; i < children->size; i++) {
		AstNode* child = getItem(children, i);

		dfs(child, arr, value);
	}
}

void addNodesAndLinks(xmlNodePtr nodes, xmlNodePtr links, AstNode* node) {
	// Создаем элемент Node для текущего узла
	char id_str[12];
	snprintf(id_str, 12, "%d", node->id); // Преобразуем id в строку
	xmlNodePtr node_elem = xmlNewChild(nodes, NULL, BAD_CAST "Node", NULL);
	xmlNewProp(node_elem, BAD_CAST "Id", BAD_CAST id_str);
	xmlNewProp(node_elem, BAD_CAST "Label", BAD_CAST node->token);

	// Проверка, что у узла есть дочерние элементы
	if (node->children != NULL) {
		for (size_t i = 0; i < node->children->size; i++) {
			AstNode* child = getItem(node->children, i);
			if (child == NULL) continue;

			// Создаем элемент Link для связи между узлом и его дочерним узлом
			char child_id_str[12];
			snprintf(child_id_str, 12, "%d", child->id);
			xmlNodePtr link_elem = xmlNewChild(links, NULL, BAD_CAST "Link", NULL);
			xmlNewProp(link_elem, BAD_CAST "Source", BAD_CAST id_str);
			xmlNewProp(link_elem, BAD_CAST "Target", BAD_CAST child_id_str);

			// Рекурсивно обрабатываем дочерний узел
			addNodesAndLinks(nodes, links, child);
		}
	}
}

int generateDGML(AstNode* root, const char* filename) {
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_elem = xmlNewNode(NULL, BAD_CAST "DirectedGraph");
	xmlNewProp(root_elem, BAD_CAST "xmlns", BAD_CAST "http://schemas.microsoft.com/vs/2009/dgml");
	xmlDocSetRootElement(doc, root_elem);

	// Создаем элементы Nodes и Links
	xmlNodePtr nodes_elem = xmlNewChild(root_elem, NULL, BAD_CAST "Nodes", NULL);
	xmlNodePtr links_elem = xmlNewChild(root_elem, NULL, BAD_CAST "Links", NULL);

	// Добавляем узлы и связи
	addNodesAndLinks(nodes_elem, links_elem, root);

	// Сохраняем DGML файл
	int result = xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return result;
}
