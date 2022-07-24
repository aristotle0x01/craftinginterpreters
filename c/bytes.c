
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "common.h"
#include "vm.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "bytes.h"

static void dump_code(Chunk* chunk) {
  FILE *write_ptr;
  write_ptr = fopen("bytes/code.bin", "wb+");
  BinCodeItem item;
  for (int i=0; i<chunk->count; i++) {
    item.code = chunk->code[i];
    item.line = chunk->lines[i];
    fwrite(&item, 1, sizeof(BinCodeItem), write_ptr);
  }
  fclose(write_ptr);
}

static void dump_strings() {
  FILE *file;
  file = fopen("bytes/strings.txt", "w+");
  int count = 1;
  for (int i=0; i<vm.strings.capacity; i++) {
    if (vm.strings.entries[i].key != NULL){
      if (0 == strncmp(vm.strings.entries[i].key->chars, "init", 4)) {
        continue;
      }
      if (0 == strncmp(vm.strings.entries[i].key->chars, "clock", 5)) {
        continue;
      }

      fprintf(file, "%s\n", vm.strings.entries[i].key->chars);
      tableSet(&vm.strings, vm.strings.entries[i].key, NUMBER_VAL(count));
      count++;
    }
  }
  fclose(file);
}

static void dump_const(uint8_t* code_seg) {
  FILE *write_ptr;
  write_ptr = fopen("bytes/const.bin", "wb+");
  printf("const count %d, sizeof(BinConstItem) %lu\n", vm.constants.count, sizeof(BinConstItem));

  BinConstItem item;
  for (int i=0; i<vm.constants.count; i++) {
    memset(&item, 0, sizeof(BinConstItem));

    printf("----%d ", i+1);
    Value value = vm.constants.values[i];
    switch (value.type) {
      case VAL_BOOL:
        {
          printf(AS_BOOL(value) ? "true" : "false");
          item.type = BIN_BOOL;
          item.as.boolean = AS_BOOL(value);
          fwrite(&item, 1, sizeof(BinConstItem), write_ptr);
          break;
        }
      case VAL_NIL: 
        {
          printf("nil"); 
          item.type = BIN_NIL;
          fwrite(&item, 1, sizeof(BinConstItem), write_ptr);
          break;
        }
      case VAL_NUMBER: 
        {
          printf("%g", AS_NUMBER(value)); 
          item.type = BIN_NUMBER;
          item.as.number = AS_NUMBER(value);
          fwrite(&item, 1, sizeof(BinConstItem), write_ptr);
          break;
        }
      case VAL_OBJ: 
        switch (OBJ_TYPE(value)) {
          case OBJ_FUNCTION:
            {
              ObjFunction *function = AS_FUNCTION(value);
              if (function->name == NULL) {
                printf("<script>");
              } else {
                printf("<fn %s()>", function->name->chars);
              }
              item.type = BIN_FUNCTION;
              item.as.func.arity = function->arity;
              item.as.func.codeLength = function->codeLength;
              item.as.func.upvalueCount = function->upvalueCount;
              item.as.func.ip_index = function->ip - code_seg;
              item.as.func.name.hash = function->name->hash;
              item.as.func.name.length = function->name->length;
              Value line_num;
              if (!tableGet(&vm.strings, function->name, &line_num)) {
                printf("****--function string %s not found!\n", AS_CSTRING(value));
                break;
              }
              item.as.func.name.string_line = AS_NUMBER(line_num);
              fwrite(&item, 1, sizeof(BinConstItem), write_ptr);
            }
            break;
          case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            item.type = BIN_STRING;
            item.as.str.hash = AS_STRING(value)->hash;
            item.as.str.length = AS_STRING(value)->length;
            Value line_num;
            if (!tableGet(&vm.strings, AS_STRING(value), &line_num)) {
              printf("****--string %s not found!\n", AS_CSTRING(value));
              break;
            }
            item.as.str.string_line = AS_NUMBER(line_num);
            fwrite(&item, 1, sizeof(BinConstItem), write_ptr);
            break;
          default:
            printf("****--unknown obj type found %d!\n", OBJ_TYPE(value));
            break;
        } 
        break;
      default:
        printf("****--unknown type found %d!\n", value.type);
    }
    printf("\n");
  }
  fclose(write_ptr);
}

InterpretResult dump(const char* source) {
  ObjFunction* function = compile(source);
  if (function == NULL) return INTERPRET_COMPILE_ERROR;
  function->ip = function->chunk.code;

  dump_code(&function->chunk);
  dump_strings();
  dump_const(function->ip);

  return INTERPRET_OK;
}

static void from_dump_code(ObjFunction* function){
  printf("-->code: \n");

  FILE *file;
  file = fopen("bytes/code.bin", "rb");
  BinCodeItem item;
  int count = 0;
  while (1 == fread(&item, sizeof(BinCodeItem), 1, file)) {
    writeChunk(&function->chunk, item.code, item.line);
    count++;
  }
  function->arity = 0;
  function->upvalueCount = 0;
  function->name = NULL;
  function->codeLength = count;
  function->ip = function->chunk.code;
  printf("read %lu bytes code\n", count*sizeof(BinCodeItem));
  fclose(file);

  printf("\n\n");
}

static void from_dump_strings(ObjFunction* function){
  printf("-->strings: \n");

  char *path;
  char line[256] = {0};

  FILE *file = fopen("bytes/strings.txt", "r");
  if (!file){
      perror(path);
      exit(EXIT_FAILURE);
  }

  // unsigned int line_count = 0;
  while (fgets(line, 256, file)){
    // printf("line[%06d]: %s", ++line_count, line);
    if (line[strlen(line) - 1] == '\n'){
      copyString(line, strlen(line)-1);
    } else {
      copyString(line, strlen(line));
    }
  }
    
  if (fclose(file)){
    exit(EXIT_FAILURE);
    perror(path);
  }

  int c = 0;
  for (int i=0; i<vm.strings.capacity; i++) {
    if (vm.strings.entries[i].key != NULL){
      c++;
      printf("%d: %s\n", c, vm.strings.entries[i].key->chars);
    }
  }

  printf("\n\n");
}

static void from_dump_const(ObjFunction* function){
  printf("-->constants: \n");

  FILE *file;
  file = fopen("bytes/const.bin", "rb");
  BinConstItem item;
  int count = 0;
  Value value;
  while (1 == fread(&item, sizeof(BinConstItem), 1, file)) {
    printf("----%d \n", ++count);
    switch (item.type) {
      case BIN_BOOL:
        {
          value.type = VAL_BOOL;
          value.as.boolean = item.as.boolean;
          addConstant(&vm.constants, value);
          break;
        }
      case BIN_NIL: 
        {
          value.type = VAL_NIL;
          addConstant(&vm.constants, value);
          break;
        }
      case BIN_NUMBER: 
        {
          value.type = VAL_NUMBER;
          value.as.number = item.as.number;
          addConstant(&vm.constants, value);
          break;
        }
      case BIN_STRING:
        {
          value.type = VAL_OBJ;
          
          bool found = false;
          for (int i=0; i<vm.strings.capacity; i++) {
            if (vm.strings.entries[i].key == NULL){
              continue;
            }
            // 投机取巧了，实际应该根据加载strings的留存信息来进行查找定位
            if (vm.strings.entries[i].key->length == item.as.str.length && 
                vm.strings.entries[i].key->hash == item.as.str.hash){
                  value = OBJ_VAL(vm.strings.entries[i].key);
                  found = true;
                  break;
            }
          }
          if (!found) {
            printf("const %d not found\n", count);
          }
          addConstant(&vm.constants, value);
          break;
        }
      case BIN_FUNCTION:
          {
            value.type = VAL_OBJ;

            ObjFunction *newFunc = newFunction();
            bool found = false;
            for (int i=0; i<vm.strings.capacity; i++) {
              if (vm.strings.entries[i].key == NULL){
                continue;
              }
              if (vm.strings.entries[i].key->length == item.as.func.name.length && 
                  vm.strings.entries[i].key->hash == item.as.func.name.hash){
                    newFunc->name = vm.strings.entries[i].key;
                    found = true;
                    break;
              }
            }
            if (!found) {
              printf("const func %d not found\n", count);
            }
            newFunc->arity = item.as.func.arity;
            newFunc->codeLength = item.as.func.codeLength;
            newFunc->ip = function->chunk.code + item.as.func.ip_index;
            newFunc->upvalueCount = item.as.func.upvalueCount;
            value = OBJ_VAL(newFunc);
          }
          addConstant(&vm.constants, value);
          break;
      default:
        printf("****--unknown type found %d!\n", value.type);
    }
  }
  fclose(file);

  printf("\n\n");
}

void from_dump(ObjFunction* function) {
  from_dump_code(function);
  from_dump_strings(function);
  from_dump_const(function);
}
