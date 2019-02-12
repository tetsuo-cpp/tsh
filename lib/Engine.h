#pragma once

#include <Cmd.h>

#include <stdbool.h>

typedef struct {
  int Status;
  bool Exiting;
} TshEngine;

void tshEngineInit(TshEngine *);
void tshEngineExec(TshEngine *, TshCmd *);
