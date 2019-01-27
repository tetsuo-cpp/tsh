#include "Cmd.h"

#include <stdlib.h>
#include <string.h>

void tshCmdInit(TshCmd *C, const char *Buf, unsigned int BufSize) {
  C->Cmd = malloc(sizeof(char) * (BufSize + 1));
  strncpy(C->Cmd, Buf, BufSize);
  C->Cmd[BufSize] = '\0';
  C->Args = NULL;
  C->ArgsSize = 0;
  C->Op = -1;
}

void tshCmdAddArg(TshCmd *C, const char *Buf, unsigned int BufSize) {
  unsigned int NewSize = C->ArgsSize + 1;
  C->Args = realloc(C->Args, sizeof(char *) * NewSize);
  C->Args[C->ArgsSize] = malloc(sizeof(char) * (BufSize + 1));
  strncpy(C->Args[C->ArgsSize], Buf, BufSize);
  C->Args[C->ArgsSize][BufSize] = '\0';
  C->ArgsSize = NewSize;
}

void tshCmdClose(TshCmd *C) {
  free(C->Cmd);
  C->Cmd = NULL;
  if (C->Args)
    free(C->Args);
  C->Args = NULL;
  C->ArgsSize = 0;
  C->Op = -1;
}
