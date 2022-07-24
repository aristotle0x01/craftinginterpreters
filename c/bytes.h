#ifndef clox_bytes_h
#define clox_bytes_h

#include "common.h"

typedef struct __attribute__((__packed__)){
  uint8_t code;
  int line;
} BinCodeItem;

typedef enum {
  BIN_BOOL,
  BIN_NIL, 
  BIN_NUMBER,
  BIN_STRING,
  BIN_FUNCTION
} BinValueType;

typedef struct __attribute__((__packed__)){
  int length;
  int string_line;
  uint32_t hash;
} BinString;

typedef struct __attribute__((__packed__)){
  BinString name;
  int arity;
  int upvalueCount;
  int codeLength;
  int ip_index;
} BinFunction;

typedef struct __attribute__((__packed__)){
  BinValueType type;
  union {
    bool boolean;
    double number;
    BinString str;
    BinFunction func;
  } as; 
} BinConstItem;

InterpretResult dump(const char* source);
void from_dump(ObjFunction* function);

#endif