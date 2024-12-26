#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t capacity;
    size_t size;
    size_t element_size;
    void* data;
} Array;

void initArray(Array* arr, size_t initial_capacity, size_t element_size);
void resizeArray(Array* arr, size_t new_size);
void addElement(Array* arr, void* element);
void removeElement(Array* arr, int index);
void freeArray(Array* arr);
void printArrayInt(Array* arr);
void printElementInt(Array* arr, int index);
void checkArrayStatus(Array* arr, const char* message);
#endif