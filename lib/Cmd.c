#include "Cmd.h"

#include <Util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tshCmdInit(TshCmd *C) {
  kv_init(C->Args);
  C->Op = TK_None;
  C->Out = NULL;
  C->OutSize = 0;
  C->In = NULL;
  C->InSize = 0;
  C->Left = NULL;
  C->Right = NULL;
}

void tshCmdAddArg(TshCmd *C, const char *Buf, unsigned int BufSize) {
  char *Arg = malloc(sizeof(char) * (BufSize + 1));
  strncpy(Arg, Buf, BufSize);
  Arg[BufSize] = '\0';
  kv_push(char *, C->Args, Arg);
}

void tshCmdClose(TshCmd *C) {
  if (C->Left)
    tshCmdClose(C->Left);

  if (C->Right)
    tshCmdClose(C->Right);

  for (KV_FOREACH(Index, C->Args))
    free(kv_A(C->Args, Index));

  kv_destroy(C->Args);
  C->Op = TK_None;

  // Free stdout buf.
  free(C->Out);
  C->Out = NULL;
  C->OutSize = 0;

  // Don't free stdin since this belongs to another cmd.
  C->In = NULL;
  C->InSize = 0;

  free(C);
}
