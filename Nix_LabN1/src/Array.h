#pragma once
#include "ast.h"
#include <stdio.h>
#include <string.h>

typedef struct Array {
	void* items;
	size_t itemSize;
	size_t capacity;
	size_t size;

} Array;

void* popBack(Array* arr);

void* removeAt(Array* arr, size_t index);

void* popFirst(Array* arr);

size_t pushBack(Array* arr, void* item);

Array* buildArray(size_t itemSize, size_t initialCapacity);

void resizeArray(Array* array);

void* getItem(Array* arr, size_t index);

void freeArray(Array** arr);

