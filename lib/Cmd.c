#include "Cmd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tshCmdInit(TshCmd *C) {
  kv_init(C->Args);
  C->Op = -1;
}

void tshCmdAddArg(TshCmd *C, const char *Buf, unsigned int BufSize) {
  char *Arg = malloc(sizeof(char) * (BufSize + 1));
  strncpy(Arg, Buf, BufSize);
  kv_push(char *, C->Args, Arg);
}

void tshCmdClose(TshCmd *C) {
  kv_destroy(C->Args);
  C->Op = -1;
}
