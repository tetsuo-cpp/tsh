#include "Engine.h"

#include <Util.h>

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define TSH_BUF_SIZE 1024

static int _tshEngineExecCmd(TshCmd *);

void tshEngineInit(TshEngine *E, TshCmdVec Cmds) {
  E->Cmds = Cmds;
  E->CurPos = 0;
}

void tshEngineExec(TshEngine *E) {
  // TODO: Implement ops.
  for (KV_FOREACH(Index, E->Cmds)) {
    TshCmd *C = &kv_A(E->Cmds, Index);
    printf("Executing command %d\n", Index);
    _tshEngineExecCmd(C);
  }
}

void tshEngineClose(TshEngine *E) { E->CurPos = 0; }

static int _tshEngineExecCmd(TshCmd *Cmd) {
  pid_t Pid;
  int FD[2];

  pipe(FD);
  Pid = fork();
  if (Pid == 0) {
    // Close reader.
    close(FD[0]);

    // Redir stdout and stderr to pipe and close writer.
    dup2(FD[1], 1);
    dup2(FD[1], 2);
    close(FD[1]);

    kv_push(char *, Cmd->Args, NULL);
    if (execvp(kv_A(Cmd->Args, 0), Cmd->Args.a) == -1)
      perror("tsh");

    exit(EXIT_FAILURE);
  } else if (Pid < 0) {
    perror("tsh");
  } else {
    close(FD[1]);

    unsigned int BufSize = sizeof(char) * TSH_BUF_SIZE;
    char *Buf = malloc(BufSize);
    unsigned int BufOffset = 0;
    while (1) {
      unsigned int AmtRead = read(FD[0], Buf + BufOffset, TSH_BUF_SIZE);
      if (AmtRead == 0)
        break;

      BufSize += sizeof(char) * TSH_BUF_SIZE;
      Buf = realloc(Buf, BufSize);
      BufOffset += AmtRead;
    }

    Buf[BufOffset] = '\0';
    Cmd->Buf = Buf;
    Cmd->BufSize = BufOffset + 1;
    printf("%s.\n", Cmd->Buf);
  }

  return 1;
}
