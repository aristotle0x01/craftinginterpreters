#ifndef clox_vm_h
#define clox_vm_h

#include "object.h"
#include "chunk.h"
#include "value.h"
#include "table.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct __attribute__((__packed__)){
  ObjClosure* closure;
  uint8_t* ip;
  Value* slots;
} CallFrame;

typedef struct __attribute__((__packed__)){
  ValueArray constants;
  Table strings;

  CallFrame frames[FRAMES_MAX];
  int frameCount;

  Value stack[STACK_MAX];
  Value* stackTop;
  Value* manageStack[STACK_MAX]; // mark stack for block
  Value** manageStackTop;

  Table globals;
  ObjString* initString;
  ObjUpvalue* openUpvalues;

  Obj* objects;
  size_t bytesAllocated;
  size_t nextGC;
  int grayCount;
  int grayCapacity;
  Obj** grayStack;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
InterpretResult interpret(const char* source);
InterpretResult interpret_dump();
void push(Value value);
Value pop();

#endif