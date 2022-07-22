#include <stdlib.h>
#include <string.h>

#include "chunk.h"
#include "memory.h"
#include "vm.h"

void initChunk(Chunk* chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  chunk->lines = NULL;
}

void freeChunk(Chunk* chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  FREE_ARRAY(int, chunk->lines, chunk->capacity);
  initChunk(chunk);
}

void writeChunk(Chunk* chunk, uint8_t byte, int line) {
  if (chunk->capacity < chunk->count + 1) {
    int oldCapacity = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    chunk->lines = GROW_ARRAY(int, chunk->lines, oldCapacity, chunk->capacity);
  }
  
  chunk->code[chunk->count] = byte;
  chunk->lines[chunk->count] = line;
  chunk->count++;
}

void copyChunkCode(Chunk* from, Chunk* to) {
  if (to->capacity < (from->count + to->count + 1)) {
    int oldCapacity = to->capacity;
    to->capacity = GROW_CAPACITY(from->count + to->count + 1);
    to->code = GROW_ARRAY(uint8_t, to->code, oldCapacity, to->capacity);
    to->lines = GROW_ARRAY(int, to->lines, oldCapacity, to->capacity);
  }

  for (int i=0; i < from->count; i++) {
    to->code[to->count] = from->code[i];
    to->lines[to->count] = from->lines[i];
    to->count++;
  }
}

int addConstant(ValueArray *va, Value value) {
  push(value);
  writeValueArray(va, value);
  pop();
  return va->count - 1;
}