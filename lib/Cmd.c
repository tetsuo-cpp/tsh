#include "Cmd.h"

#include <Util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tshCmdInit(TshCmd *C) {
  kv_init(C->Args);
  C->Op = TK_None;
  C->Buf = NULL;
  C->BufSize = 0;
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
  free(C->Buf);
  C->Buf = NULL;
  C->BufSize = 0;
  free(C);
}

void tshCmdPrint(TshCmd *C) {
  if (!C)
    return;

  if (C->Op == TK_None) {
    printf("Command node: %s\n", kv_A(C->Args, 0));
  } else {
    printf("Op node: %d\n", C->Op);

    printf("Walking left node:\n");
    tshCmdPrint(C->Left);

    printf("Walking right node:\n");
    tshCmdPrint(C->Right);
  }
}
