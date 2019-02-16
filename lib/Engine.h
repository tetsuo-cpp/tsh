#pragma once

#include <Cmd.h>
#include <DataBase.h>

#include <stdbool.h>

typedef struct {
  int Status;
  bool Exiting;
  TshDataBase *DB;
} TshEngine;

void tshEngineInit(TshEngine *, TshDataBase *);
void tshEngineExec(TshEngine *, TshCmd *);
