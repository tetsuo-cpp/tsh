#pragma once

#include <klib/kvec.h>
#include <sqlite3.h>

#include <stdbool.h>

typedef struct {
  char *CmdName;
  char *Duration;
} TshStatsData;

typedef struct {
  sqlite3 *DB;
  kvec_t(TshStatsData) Data;
  bool Clear;
} TshDataBase;

bool tshDataBaseInit(TshDataBase *, const char *);
void tshDataBaseRecordDuration(TshDataBase *, const char *, double);
bool tshDataBaseGetTopDurations(TshDataBase *);
bool tshDataBaseGetDuration(TshDataBase *, const char *);
bool tshDataBaseGetHistory(TshDataBase *, long int);
void tshDataBaseDestroy(TshDataBase *);
