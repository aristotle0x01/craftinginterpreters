#ifndef clox_table_h
#define clox_table_h

#include "common.h"
#include "value.h"

typedef struct __attribute__((__packed__)){
  ObjString* key;
  Value value;
} Entry;

typedef struct __attribute__((__packed__)){
  int count;
  int capacity;
  Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);
bool tableGet(Table* table, ObjString* key, Value* value);
bool tableSet(Table* table, ObjString* key, Value value);
bool tableDelete(Table* table, ObjString* key);
void tableAddAll(Table* from, Table* to);
ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash);
void tableRemoveWhite(Table* table);
void markTable(Table* table);
#endif