#pragma once

#include <Cmd.h>

typedef struct {
  int Status;
} TshEngine;

int tshEngineExec(TshEngine *, TshCmd *);
