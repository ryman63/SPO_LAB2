
#include "Array.h"

void* popBack(Array* arr) {
	if (arr->size == 0) {
		return NULL;
	}
	arr->size--;
	void* item = (char*)arr->items + (arr->size * arr->itemSize);
	void* result = malloc(arr->itemSize);
	memcpy(result, item, arr->itemSize);
	return result;
}

void* removeAt(Array* arr, size_t index) {
	if (index >= arr->size) {
		return NULL; // Индекс вне диапазона
	}

	void* item = malloc(arr->itemSize);
	if (item == NULL) {
		printf("removeAt: malloc failed");
		return NULL;
	}

	void* ptr = (char*)arr->items + (index * arr->itemSize);
	memcpy(item, ptr, arr->itemSize);

	// Сдвигаем элементы массива на одну позицию влево, начиная с удаленного элемента
	memmove(ptr, (char*)ptr + arr->itemSize, (arr->size - index - 1) * arr->itemSize);

	arr->size--;

	return item;
}
void* popFirst(Array* arr) {
	if (arr->size == 0) {
		return NULL;
	}

	void* item = malloc(arr->itemSize);
	if (item == NULL) {
		printf("popFirst: malloc failed");
		return NULL;
	}

	memcpy(item, arr->items, arr->itemSize);

	// Сдвигаем элементы массива на одну позицию влево
	memmove(arr->items, (char*)arr->items + arr->itemSize, (arr->size - 1) * arr->itemSize);

	arr->size--;

	return item;
}

size_t pushBack(Array* arr, void* item) {
	if (arr->size == arr->capacity) {
		resizeArray(arr);
	}
	memcpy((char*)arr->items + (arr->size * arr->itemSize), item, arr->itemSize);
	arr->size++;
}

Array* buildArray(size_t itemSize, size_t initialCapacity)
{
	Array* array = malloc(sizeof(Array));
	array->items = (void*)malloc(itemSize * initialCapacity);

	if (array->items == NULL) {
		printf("buildArray: malloc failed");
	}

	array->capacity = initialCapacity;
	array->itemSize = itemSize;
	array->size = 0;

	return array;
}

void resizeArray(Array* array)
{
	array->capacity *= 2;
	array->items = realloc(array->items, array->itemSize * array->capacity);

	if (array->items == NULL)
	{
		printf("resizeArray: realloc failed");
	}
}

void* getItem(Array* arr,size_t index)
{
	if (index >= arr->size)
		return NULL;
	return (AstNode*)((char*)arr->items + (index * arr->itemSize));
}

void freeArray(Array** arr) {
	free((*arr)->items);
	free(*arr);
	*arr = NULL;
}