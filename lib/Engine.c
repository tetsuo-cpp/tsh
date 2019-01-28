#include "Engine.h"

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

int _tshEngineExecCmd(TshCmd *);

void tshEngineInit(TshEngine *E, TshCmdVec Cmds) {
  E->Cmds = Cmds;
  E->CurPos = 0;
}

void tshEngineExec(TshEngine *E) {
  // TODO: Implement ops.
  for (unsigned int Index = 0; Index < kv_size(E->Cmds); ++Index) {
    TshCmd *C = &kv_A(E->Cmds, Index);
    _tshEngineExecCmd(C);
  }
}

void tshEngineClose(TshEngine *E) { E->CurPos = 0; }

int _tshEngineExecCmd(TshCmd *Cmd) {
  pid_t Pid, WPid;
  int Status;

  Pid = fork();
  if (Pid == 0) {
    if (execvp(kv_A(Cmd->Args, 0), Cmd->Args.a) == -1)
      perror("tsh");

    exit(EXIT_FAILURE);
  } else if (Pid < 0) {
    perror("tsh");
  } else {
    do {
      WPid = waitpid(Pid, &Status, WUNTRACED);
    } while (!WIFEXITED(Status) && !WIFSIGNALED(Status));
  }

  return 1;
}
