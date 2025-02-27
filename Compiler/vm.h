#pragma once
#ifndef clox_vm_h
#define clox_vm_h


#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef enum {
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR
} InterpretResult;

typedef struct {
	Chunk* chunk;
	uint8_t* ip;
	Value* stack;
	int stackCount;
	int stackCapacity;
	Obj* objects;
} VM;

extern VM vm;

void initVM();
void freeVM();
//InterpretResult interpret(Chunk* chunk);
InterpretResult interpret(const char* source);
void push(Value value);
Value pop();

#endif