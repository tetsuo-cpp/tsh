#pragma once

#include <Lex.h>

typedef struct {
  char *Cmd;
  char **Args;
  unsigned int ArgsSize;
  TshTokenKind Op;
} TshCommand;

void tshCommandInit(TshCommand *Command, const char *Buf, unsigned int BufSize);
void tshCommandAddArg(TshCommand *Command, const char *Buf,
                      unsigned int BufSize);
void tshCommandClose(TshCommand *Command);
