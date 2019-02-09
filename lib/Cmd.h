#pragma once

#include <Lex.h>

#include <klib/kvec.h>

typedef struct TshCmd TshCmd;

struct TshCmd {
  kvec_t(char *) Args;
  TshTokenKind Op;
  char *Out;
  size_t OutSize;
  char *In;
  size_t InSize;
  TshCmd *Left;
  TshCmd *Right;
};

void tshCmdInit(TshCmd *);
void tshCmdAddArg(TshCmd *, const char *, size_t);
void tshCmdClose(TshCmd *);
