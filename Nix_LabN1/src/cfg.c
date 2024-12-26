#include "cfg.h"

CfgNode* createCfgNode(char* label) {
	CfgNode* createNode = malloc(sizeof(CfgNode));

	createNode->label = label;

	createNode->condJump = NULL;
	createNode->uncondJump = NULL;
	createNode->opTree = NULL;

	createNode->id = cfg_id_counter;
	cfg_id_counter++;

	return createNode;
}

void addNodesAndLinksCfg(xmlNodePtr nodes, xmlNodePtr links, CfgNode* node) {
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