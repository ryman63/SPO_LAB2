#include "cfg.h"

CfgNode* createCfgNode(char* label, int line) {
	CfgNode* createNode = malloc(sizeof(CfgNode));

	createNode->label = label;
	createNode->line = line;

	createNode->condJump = NULL;
	createNode->uncondJump = NULL;
	createNode->opTree = NULL;

	createNode->id = cfg_id_counter;
	cfg_id_counter++;

	return createNode;
}

OpNode* createOpNode(char* operation)
{
	OpNode* createNode = malloc(sizeof(OpNode));

	createNode->operation = operation;
	createNode->args = buildArray(sizeof(OpNode), 2);
	createNode->id = opNode_id_counter;

	opNode_id_counter++;
	
	return createNode;
}

void addNodesAndLinksCfg(xmlNodePtr nodes, xmlNodePtr links, CfgNode* node) {

	if (visited[node->id])
		return;

	visited[node->id] = true;

	// Создаем элемент Node для текущего узла
	char id_str[12];
	snprintf(id_str, 12, "%d", node->id); // Преобразуем id в строку
	xmlNodePtr node_elem = xmlNewChild(nodes, NULL, BAD_CAST "Node", NULL);
	xmlNewProp(node_elem, BAD_CAST "Id", BAD_CAST id_str);
	xmlNewProp(node_elem, BAD_CAST "Label", BAD_CAST node->label);

	// Проверка, что у узла есть дочерние элементы
	if (node->condJump) {
		CfgNode* child = node->condJump;

		// Создаем элемент Link для связи между узлом и его дочерним узлом
		char child_id_str[12];
		snprintf(child_id_str, 12, "%d", child->id);
		xmlNodePtr link_elem = xmlNewChild(links, NULL, BAD_CAST "Link", NULL);
		xmlNewProp(link_elem, BAD_CAST "Source", BAD_CAST id_str);
		xmlNewProp(link_elem, BAD_CAST "Target", BAD_CAST child_id_str);

		// Рекурсивно обрабатываем дочерний узел
		addNodesAndLinksCfg(nodes, links, child);
	}
	if (node->uncondJump) {
		CfgNode* child = node->uncondJump;

		// Создаем элемент Link для связи между узлом и его дочерним узлом
		char child_id_str[12];
		snprintf(child_id_str, 12, "%d", child->id);
		xmlNodePtr link_elem = xmlNewChild(links, NULL, BAD_CAST "Link", NULL);
		xmlNewProp(link_elem, BAD_CAST "Source", BAD_CAST id_str);
		xmlNewProp(link_elem, BAD_CAST "Target", BAD_CAST child_id_str);

		// Рекурсивно обрабатываем дочерний узел
		addNodesAndLinksCfg(nodes, links, child);
	}
}

int generateCfgDGML(CfgNode* root, const char* filename) {
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_elem = xmlNewNode(NULL, BAD_CAST "DirectedGraph");
	xmlNewProp(root_elem, BAD_CAST "xmlns", BAD_CAST "http://schemas.microsoft.com/vs/2009/dgml");
	xmlDocSetRootElement(doc, root_elem);

	// Создаем элементы Nodes и Links
	xmlNodePtr nodes_elem = xmlNewChild(root_elem, NULL, BAD_CAST "Nodes", NULL);
	xmlNodePtr links_elem = xmlNewChild(root_elem, NULL, BAD_CAST "Links", NULL);

	// Добавляем узлы и связи
	addNodesAndLinksCfg(nodes_elem, links_elem, root);

	// Сохраняем DGML файл
	int result = xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return result;
}

void addNodesAndLinksOpTree(xmlNodePtr nodes, xmlNodePtr links, OpNode* node) {
	// Создаем элемент Node для текущего узла
	char id_str[12];
	snprintf(id_str, 12, "%d", node->id); // Преобразуем id в строку
	xmlNodePtr node_elem = xmlNewChild(nodes, NULL, BAD_CAST "Node", NULL);
	xmlNewProp(node_elem, BAD_CAST "Id", BAD_CAST id_str);
	xmlNewProp(node_elem, BAD_CAST "Label", BAD_CAST node->operation);

	// Проверка, что у узла есть дочерние элементы
	if (node->args != NULL) {
		for (size_t i = 0; i < node->args->size; i++) {
			AstNode* child = getItem(node->args, i);
			if (child == NULL) continue;

			// Создаем элемент Link для связи между узлом и его дочерним узлом
			char child_id_str[12];
			snprintf(child_id_str, 12, "%d", child->id);
			xmlNodePtr link_elem = xmlNewChild(links, NULL, BAD_CAST "Link", NULL);
			xmlNewProp(link_elem, BAD_CAST "Source", BAD_CAST id_str);
			xmlNewProp(link_elem, BAD_CAST "Target", BAD_CAST child_id_str);

			// Рекурсивно обрабатываем дочерний узел
			addNodesAndLinksOpTree(nodes, links, child);
		}
	}
}

int generateOpTreeDGML(OpNode* root, const char* filename) {
	char saveDirectory[64] = "OpTreeDGML\\";
	strcat_s(saveDirectory, 64, filename);
	xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNodePtr root_elem = xmlNewNode(NULL, BAD_CAST "DirectedGraph");
	xmlNewProp(root_elem, BAD_CAST "xmlns", BAD_CAST "http://schemas.microsoft.com/vs/2009/dgml");
	xmlDocSetRootElement(doc, root_elem);

	// Создаем элементы Nodes и Links
	xmlNodePtr nodes_elem = xmlNewChild(root_elem, NULL, BAD_CAST "Nodes", NULL);
	xmlNodePtr links_elem = xmlNewChild(root_elem, NULL, BAD_CAST "Links", NULL);

	// Добавляем узлы и связи
	addNodesAndLinksOpTree(nodes_elem, links_elem, root);

	// Сохраняем DGML файл
	int result = xmlSaveFormatFileEnc(saveDirectory, doc, "UTF-8", 1);
	xmlFreeDoc(doc);
	xmlCleanupParser();
	return result;
}

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

void TraverseCfg(CfgNode* root, int(*operation)(OpNode*, const char*)) {
	if (visited2[root->id])
		return;

	visited2[root->id] = true;

	if (root) {
		if (root->condJump) TraverseCfg(root->condJump, operation);
		if (root->uncondJump) TraverseCfg(root->uncondJump, operation);
	}
	if (root->opTree)
	{
		char fileName[64];
		strcpy_s(fileName, 64, root -> label);
		strcat_s(fileName, 64, "_");
		char buff[8];
		_itoa_s(root->line, buff, 8, 10);
		strcat_s(fileName, 64, buff);
		strcat_s(fileName, 64, ".dgml");
		operation(root->opTree, fileName);
	}
}