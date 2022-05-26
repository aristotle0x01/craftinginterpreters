#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
  Chunk chunk;
  initChunk(&chunk);

  int constant = addConstant(&chunk, 5);
  writeChunk(&chunk, OP_CONSTANT, 123);
  writeChunk(&chunk, constant, 123);

  writeChunk(&chunk, OP_RETURN, 123);

  disassembleChunk(&chunk, "test chunk");
  freeChunk(&chunk);

  Chunk chunk2;
  initChunk(&chunk2);
  writeChunk(&chunk, OP_RETURN, 1);
  for (int i=0; i < 300; i ++) {
      writeConstant(&chunk2, 100+i, 1000+i);
  }
  writeChunk(&chunk, OP_RETURN, 100);
  disassembleChunk(&chunk2, "test long constant");
  freeChunk(&chunk2);
  return 0;
}