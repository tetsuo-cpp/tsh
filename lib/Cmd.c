#include "Cmd.h"

#include <stdlib.h>
#include <string.h>

void tshCmdInit(TshCmd *C, const char *Buf, unsigned int BufSize) {
  C->Cmd = malloc(sizeof(char) * (BufSize + 1));
  strncpy(C->Cmd, Buf, BufSize);
  C->Cmd[BufSize] = '\0';
  kv_init(C->Args);
  C->Op = -1;
}

void tshCmdAddArg(TshCmd *C, const char *Buf, unsigned int BufSize) {
  char *Arg = malloc(sizeof(char) * (BufSize + 1));
  strncpy(Arg, Buf, BufSize);
  Arg[BufSize] = '\0';
  kv_push(char *, C->Args, Arg);
}

void tshCmdClose(TshCmd *C) {
  free(C->Cmd);
  C->Cmd = NULL;
  kv_destroy(C->Args);
  C->Op = -1;
}
