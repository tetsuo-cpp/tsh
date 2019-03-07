#include "Cmd.h"

#include <Util.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool _tshCmdAddEnvVar(TshCmd *, const char *, size_t);

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

void tshCmdAddArg(TshCmd *C, const char *Buf, size_t BufSize) {
  // Environment variable.
  if (_tshCmdAddEnvVar(C, Buf, BufSize))
    return;

  char *Arg = malloc(sizeof(char) * (BufSize + 1));
  strncpy(Arg, Buf, BufSize);
  Arg[BufSize] = '\0';
  kv_push(char *, C->Args, Arg);
}

void tshCmdDestroy(TshCmd *C) {
  if (C->Left)
    tshCmdDestroy(C->Left);

  if (C->Right)
    tshCmdDestroy(C->Right);

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

static bool _tshCmdAddEnvVar(TshCmd *C, const char *Buf, size_t BufSize) {
  if (Buf[0] != '$')
    return false;

  char *EnvVar = malloc(BufSize);
  if (!EnvVar)
    return false;

  strncpy(EnvVar, Buf + 1, BufSize - 1);
  EnvVar[BufSize - 1] = '\0';

  char *Result = getenv(EnvVar);
  free(EnvVar);
  if (!Result)
    return false;

  char *Arg = malloc(sizeof(char) * (strlen(Result) + 1));
  if (!Arg)
    return false;

  strcpy(Arg, Result);
  kv_push(char *, C->Args, Arg);
  return true;
}
