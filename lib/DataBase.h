#pragma once

#include <sqlite3.h>

#include <stdbool.h>

typedef struct {
  sqlite3 *DB;
} TshDataBase;

bool tshDataBaseInit(TshDataBase *, const char *);
void tshDataBaseRecordDuration(TshDataBase *, const char *, float);
void tshDataBaseClose(TshDataBase *);
