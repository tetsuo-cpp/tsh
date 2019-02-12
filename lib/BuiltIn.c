#include "BuiltIn.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int _tshBuiltInCd(TshEngine *E, TshCmd *Cmd);
static int _tshBuiltInHelp(TshEngine *E, TshCmd *Cmd);
static int _tshBuiltInExit(TshEngine *E, TshCmd *Cmd);

#define TSH_BUILTIN_REGISTER(BuiltInString, BuiltIn)                           \
  if (strcmp(kv_A(Cmd->Args, 0), BuiltInString) == 0) {                        \
    *Status = BuiltIn(E, Cmd);                                                 \
    return true;                                                               \
  }

bool tshBuiltInExec(TshEngine *E, TshCmd *Cmd, int *Status) {
  TSH_BUILTIN_REGISTER("cd", _tshBuiltInCd);
  TSH_BUILTIN_REGISTER("help", _tshBuiltInHelp);
  TSH_BUILTIN_REGISTER("exit", _tshBuiltInExit);

  return false;
}

static int _tshBuiltInCd(TshEngine *E, TshCmd *Cmd) {
  (void)E;
  if (kv_size(Cmd->Args) != 2) {
    fprintf(stderr,
            "tsh: incorrect number of args to cd. Expected=\"cd [DEST]\"");
    return -1;
  }

  int Status;
  if ((Status = chdir(kv_A(Cmd->Args, 1))) != 0)
    return Status;

  return 0;
}

static int _tshBuiltInHelp(TshEngine *E, TshCmd *Cmd) {
  (void)E;
  if (kv_size(Cmd->Args) != 1) {
    fprintf(stderr, "tsh: incorrect number of args to help. Expected=\"help\"");
    return -1;
  }

  printf("Tetsuo's Shell (Tsh)\n");
  printf(
      "More information can be found at: https://github.com/tetsuo-cpp/tsh\n");

  return 0;
}

static int _tshBuiltInExit(TshEngine *E, TshCmd *Cmd) {
  if (kv_size(Cmd->Args) != 1) {
    fprintf(stderr, "tsh: incorrect number of args to exit. Expected=\"exit\"");
    return -1;
  }

  E->Exiting = true;
  return 0;
}
