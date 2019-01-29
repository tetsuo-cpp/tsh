#include "Cmd.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tshCmdInit(TshCmd *C) {
  kv_init(C->Args);
  C->Op = -1;
  C->Buf = NULL;
  C->BufSize = 0;
}

void tshCmdAddArg(TshCmd *C, const char *Buf, unsigned int BufSize) {
  char *Arg = malloc(sizeof(char) * (BufSize + 1));
  strncpy(Arg, Buf, BufSize);
  Arg[BufSize] = '\0';
  kv_push(char *, C->Args, Arg);
}

void tshCmdClose(TshCmd *C) {
  for (unsigned int Index = 0; Index < kv_size(C->Args); ++Index)
    free(kv_A(C->Args, Index));

  kv_destroy(C->Args);
  C->Op = -1;
  free(C->Buf);
  C->Buf = NULL;
  C->BufSize = 0;
}
