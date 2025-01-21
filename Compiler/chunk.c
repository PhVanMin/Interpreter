#include <stdlib.h>

#include "chunk.h"

void initChunk(Chunk* chunk) {
	chunk->count = 0;
	chunk->capacity = 0;
	chunk->code = NULL;
    chunk->line.count = 0;
    chunk->line.capacity = 0;
    chunk->line.lines = NULL;
    initValueArray(&chunk->constants);
}

void freeChunk(Chunk* chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->line.lines, chunk->line.capacity);
    freeValueArray(&chunk->constants);
    initChunk(chunk);
}

int getLine(Chunk* chunk, int offset) {
    int start = 0;
    int end = chunk->line.count - 1;

    for (;;) {
        int mid = (start + end) / 2;
        int lineOffset = chunk->line.lines[mid * 2];
        if (offset < lineOffset)
            end = mid - 1;
        else if ((mid == chunk->line.count - 1) || (offset < chunk->line.lines[(mid + 1) * 2]))
            return chunk->line.lines[mid * 2 + 1];
        else
            start = mid + 1;
    }
}

void writeConstant(Chunk* chunk, Value value, int line)
{
    int index = addConstant(chunk, value);
    if (index < 256) {
        writeChunk(chunk, OP_CONSTANT, line);
        writeChunk(chunk, (uint8_t)index, line);
    }
    else {
        writeChunk(chunk, OP_CONSTANT_LONG, line);
        writeChunk(chunk, (uint8_t)(index & 0xff), line);
        writeChunk(chunk, (uint8_t)((index >> 8) & 0xff), line);
        writeChunk(chunk, (uint8_t)((index >> 16) & 0xff), line);
    }
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
    // Write chunk
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code,
            oldCapacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->count++;

    // Process line with RLE
    int lineCount = chunk->line.count;
    if (lineCount > 0 && chunk->line.lines[(lineCount - 1) * 2 + 1] == line) {
        return;
    }

    if (chunk->line.capacity < (lineCount + 1) * 2) {
        int oldCapacity = chunk->line.capacity;
        chunk->line.capacity = GROW_CAPACITY(oldCapacity);
        chunk->line.lines = GROW_ARRAY(int, chunk->line.lines,
            oldCapacity, chunk->line.capacity);
    }

    chunk->line.lines[lineCount * 2] = chunk->count - 1;
    int lineOffset = chunk->line.lines[lineCount * 2];
    chunk->line.lines[lineCount * 2 + 1] = line;
    int lines = chunk->line.lines[lineCount * 2 + 1];
    chunk->line.count++;
}

int addConstant(Chunk* chunk, Value value) {
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}