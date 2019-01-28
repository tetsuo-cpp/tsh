#pragma once

#include <Cmd.h>

#include <klib/kvec.h>

typedef kvec_t(TshCmd) TshCmdVec;

typedef struct {
  TshCmdVec Cmds;
  unsigned int CurPos;
} TshEngine;

void tshEngineInit(TshEngine *, TshCmdVec);
void tshEngineExec(TshEngine *);
void tshEngineClose(TshEngine *);
