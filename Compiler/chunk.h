#pragma once
#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "memory.h"
#include "value.h"

typedef enum {
	OP_CONSTANT,
	OP_RETURN,
	OP_CONSTANT_LONG,
	OP_NEGATE,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
} OpCode;

typedef struct {
	int count;
	int capacity;
	int* lines;
} Line;

typedef struct {
	int count;
	int capacity;
	uint8_t* code;
	ValueArray constants;
	Line line;
} Chunk;


void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);
int getLine(Chunk* chunk, int offset);
void writeConstant(Chunk* chunk, Value value, int line);

#endif