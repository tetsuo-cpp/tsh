#pragma once

#include <Lex.h>

typedef struct {
  char *Cmd;
  char **Args;
  unsigned int ArgsSize;
  TshTokenKind Op;
} TshCmd;

void tshCmdInit(TshCmd *, const char *, unsigned int);
void tshCmdAddArg(TshCmd *, const char *, unsigned int);
void tshCmdClose(TshCmd *);
