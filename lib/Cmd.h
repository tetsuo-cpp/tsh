#pragma once

#include <Lex.h>

#include <klib/kvec.h>

typedef struct TshCmd TshCmd;

struct TshCmd {
  kvec_t(char *) Args;
  TshTokenKind Op;
  char *Buf;
  unsigned int BufSize;
  TshCmd *Left;
  TshCmd *Right;
};

void tshCmdInit(TshCmd *);
void tshCmdAddArg(TshCmd *, const char *, unsigned int);
void tshCmdClose(TshCmd *);
void tshCmdPrint(TshCmd *);
