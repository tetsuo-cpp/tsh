#pragma once

#include <Lex.h>

#include <klib/kvec.h>

typedef struct {
  kvec_t(char *) Args;
  TshTokenKind Op;
  char *Buf;
  unsigned int BufSize;
} TshCmd;

void tshCmdInit(TshCmd *);
void tshCmdAddArg(TshCmd *, const char *, unsigned int);
void tshCmdClose(TshCmd *);
