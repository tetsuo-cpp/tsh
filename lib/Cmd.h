#pragma once

#include <Lex.h>

#include <klib/kvec.h>

typedef struct {
  char *Cmd;
  kvec_t(char *) Args;
  TshTokenKind Op;
} TshCmd;

void tshCmdInit(TshCmd *, const char *, unsigned int);
void tshCmdAddArg(TshCmd *, const char *, unsigned int);
void tshCmdClose(TshCmd *);
