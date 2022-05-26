#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

//1 * 2 + 3
//1 + 2 * 3
//3 - 2 - 1
//1 + 2 * 3 - 4 / -5
static void another(){
  initVM();
  Chunk chunk2;
  initChunk(&chunk2);
  for (int i=0; i < 300; i ++) {
      writeConstant(&chunk2, 100+i, 1000+i);
  }
  writeConstant(&chunk2, 1, 2000);
  writeConstant(&chunk2, 2, 2000);
  writeChunk(&chunk2, OP_MULTIPLY, 2000);
  writeConstant(&chunk2, 3, 2000);
  writeChunk(&chunk2, OP_ADD, 2000);

  writeConstant(&chunk2, 1, 2000);
  writeConstant(&chunk2, 2, 2000);
  writeConstant(&chunk2, 3, 2000);
  writeChunk(&chunk2, OP_MULTIPLY, 2000);
  writeChunk(&chunk2, OP_ADD, 2000);
  writeConstant(&chunk2, 4, 2000);
  writeConstant(&chunk2, 5, 2000);
  writeChunk(&chunk2, OP_NEGATE, 2000);
  writeChunk(&chunk2, OP_DIVIDE, 2000);
  writeChunk(&chunk2, OP_SUBTRACT, 2000);

  writeChunk(&chunk2, OP_RETURN, 2001);
  disassembleChunk(&chunk2, "test long constant");
  interpret(&chunk2);
  freeVM();
  freeChunk(&chunk2);
}

int main(int argc, const char* argv[]) {
//  initVM();
//  Chunk chunk;
//  initChunk(&chunk);
//  int constant = addConstant(&chunk, 1.2);
//  writeChunk(&chunk, OP_CONSTANT, 123);
//  writeChunk(&chunk, constant, 123);
//  constant = addConstant(&chunk, 3.4);
//  writeChunk(&chunk, OP_CONSTANT, 123);
//  writeChunk(&chunk, constant, 123);
//  writeChunk(&chunk, OP_ADD, 123);
//  constant = addConstant(&chunk, 5.6);
//  writeChunk(&chunk, OP_CONSTANT, 123);
//  writeChunk(&chunk, constant, 123);
//  writeChunk(&chunk, OP_DIVIDE, 123);
//  writeChunk(&chunk, OP_NEGATE, 123);
//  writeChunk(&chunk, OP_RETURN, 123);
//  disassembleChunk(&chunk, "test chunk");
//  interpret(&chunk);
//  freeVM();
//  freeChunk(&chunk);

  another();
  return 0;
}