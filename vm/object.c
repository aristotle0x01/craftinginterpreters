#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;

  object->next = vm.objects;
  vm.objects = object;
  return object;
}

static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

#ifdef FLEXIBLE_ARRAY_MEMBER
    static ObjString* allocateString(char* chars, int length, uint32_t hash) {
        ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
        string->length = length;
        string->hash = hash;
        tableSet(&vm.strings, string, NIL_VAL);
        return string;
    }

    ObjString* takeString(char* chars, int length) {
        return NULL;
    }

    ObjString* copyString(const char* chars, int length) {
        uint32_t hash = hashString(chars, length);
        ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
        if (interned != NULL) return interned;

        // printf("size of ObjString %lu, %lu, %lu, %lu\n", sizeof(ObjString), sizeof(int), sizeof(ObjType), sizeof(Obj));
        Obj* obj = allocateObject(sizeof(ObjString) + length + 1, OBJ_STRING);
        ObjString* string = (ObjString *)obj;
        string->length = length;
        memcpy(string->chars, chars, length);
        string->chars[length] = '\0';
        string->hash = hash;
        tableSet(&vm.strings, string, NIL_VAL);
        return string;
    }
#else
    static ObjString* allocateString(char* chars, int length, uint32_t hash) {
        ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
        string->length = length;
        string->chars = chars;
        string->hash = hash;
        tableSet(&vm.strings, string, NIL_VAL);
        return string;
    }

    ObjString* takeString(char* chars, int length) {
        uint32_t hash = hashString(chars, length);
        ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
        if (interned != NULL) {
            FREE_ARRAY(char, chars, length + 1);
            return interned;
        }

        return allocateString(chars, length, hash);
    }

    ObjString* copyString(const char* chars, int length) {
        uint32_t hash = hashString(chars, length);
        ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
        if (interned != NULL) return interned;

        // printf("size of ObjString %lu, %lu, %lu, %lu\n", sizeof(ObjString), sizeof(int), sizeof(ObjType), sizeof(Obj));
        char* heapChars = ALLOCATE(char, length + 1);
        memcpy(heapChars, chars, length);
        heapChars[length] = '\0';
        return allocateString(heapChars, length, hash);
    }
#endif

void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING:
      printf("%s", AS_CSTRING(value));
      break;
  }
}