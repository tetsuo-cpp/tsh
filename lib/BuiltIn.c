#include "BuiltIn.h"

#include <DataBase.h>
#include <Util.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int tshBuiltInCd(TshEngine *, TshCmd *);
static int tshBuiltInHelp(TshEngine *, TshCmd *);
static int tshBuiltInExit(TshEngine *, TshCmd *);
static int tshBuiltInStats(TshEngine *, TshCmd *);

#define TSH_BUILTIN_REGISTER(BuiltInString, BuiltIn)                           \
  if (strcmp(kv_A(Cmd->Args, 0), BuiltInString) == 0) {                        \
    *Status = BuiltIn(E, Cmd);                                                 \
    return true;                                                               \
  }

bool tshBuiltInExec(TshEngine *E, TshCmd *Cmd, int *Status) {
  TSH_BUILTIN_REGISTER("cd", tshBuiltInCd);
  TSH_BUILTIN_REGISTER("help", tshBuiltInHelp);
  TSH_BUILTIN_REGISTER("exit", tshBuiltInExit);
  TSH_BUILTIN_REGISTER("tshstats", tshBuiltInStats);

  return false;
}

static int tshBuiltInCd(TshEngine *E, TshCmd *Cmd) {
  (void)E;
  if (kv_size(Cmd->Args) != 2) {
    fprintf(stderr,
            "tsh: incorrect number of args to cd. Expected=\"cd [DEST]\"\n");
    return -1;
  }

  return chdir(kv_A(Cmd->Args, 1));
}

static int tshBuiltInHelp(TshEngine *E, TshCmd *Cmd) {
  (void)E;
  if (kv_size(Cmd->Args) != 1) {
    fprintf(stderr,
            "tsh: incorrect number of args to help. Expected=\"help\"\n");
    return -1;
  }

  printf("Tetsuo's Shell (Tsh)\n");
  printf(
      "More information can be found at: https://github.com/tetsuo-cpp/tsh\n");

  return 0;
}

static int tshBuiltInExit(TshEngine *E, TshCmd *Cmd) {
  if (kv_size(Cmd->Args) != 1) {
    fprintf(stderr,
            "tsh: incorrect number of args to exit. Expected=\"exit\"\n");
    return -1;
  }

  E->Exiting = true;
  return 0;
}

static int tshBuiltInStats(TshEngine *E, TshCmd *Cmd) {
  if (kv_size(Cmd->Args) == 1) {
    if (!tshDataBaseGetTopDurations(E->DB))
      return -1;

    printf("Printing the 10 commands with the highest average runtime "
           "duration:\n");
  } else if (kv_size(Cmd->Args) == 2) {
    const char *CmdName = kv_A(Cmd->Args, 1);
    if (!tshDataBaseGetDuration(E->DB, CmdName))
      return -1;

    printf("Printing the average runtime duration for command \"%s\":\n",
           CmdName);
  } else {
    fprintf(stderr, "tsh: incorrect number of arguments to tshstats. "
                    "Top10=\"tshstats\" ForCmd=\"tshstats [CMD]\"\n");
    return -1;
  }

  if (kv_size(E->DB->Data) == 0)
    printf("No results found.\n");

  for (KV_FOREACH(Index, E->DB->Data)) {
    TshStatsData *S = &kv_A(E->DB->Data, Index);
    printf("%-20s%20s secs\n", S->CmdName, S->Duration);
  }

  return 0;
}
