#include "Engine.h"

#include <BuiltIn.h>
#include <Util.h>

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define TSH_BUF_INCREMENT 1024

static int _tshEngineExecImpl(TshEngine *, TshCmd *);
static int _tshEngineExecCmd(TshEngine *, TshCmd *);
static int _tshEngineExecPipe(TshEngine *, TshCmd *, TshCmd *);
static int _tshEngineExecRedir(TshEngine *, TshCmd *, TshCmd *);
static int _tshEngineExecReverseRedir(TshEngine *, TshCmd *, TshCmd *);

void tshEngineInit(TshEngine *E) {
  E->Status = 0;
  E->Exiting = false;
}

void tshEngineExec(TshEngine *E, TshCmd *Cmd) {
  E->Status = _tshEngineExecImpl(E, Cmd);
}

static int _tshEngineExecImpl(TshEngine *E, TshCmd *Cmd) {
  switch (Cmd->Op) {
  case TK_None:
    return _tshEngineExecCmd(E, Cmd);
  case TK_Pipe:
    return _tshEngineExecPipe(E, Cmd->Left, Cmd->Right);
  case TK_Redir:
    return _tshEngineExecRedir(E, Cmd->Left, Cmd->Right);
  case TK_ReverseRedir:
    return _tshEngineExecReverseRedir(E, Cmd->Left, Cmd->Right);
  default:
    return -1;
  }
}

static int _tshEngineExecCmd(TshEngine *E, TshCmd *Cmd) {
#ifndef NDEBUG
  printf("tsh: executing cmd. Cmd=%s Args=[", kv_A(Cmd->Args, 0));
  for (KV_FOREACH(Index, Cmd->Args)) {
    if (Index == 0)
      continue;
    if (Index != 1)
      printf(", ");

    printf("%s", kv_A(Cmd->Args, Index));
  }

  printf("]\n");
#endif

  // Check whether it's a built-in cmd first.
  int Status;
  if (tshBuiltInExec(E, Cmd, &Status))
    return Status;

  pid_t Pid;
  int CmdRead[2];
  int CmdWrite[2];

  // Open two pipes.
  // CmdRead is for the shell to read the child's stdout.
  // CmdWrite is for the shell to write to the child's stdin.
  pipe(CmdRead);
  pipe(CmdWrite);

  Pid = fork();
  if (Pid == 0) {
    close(CmdWrite[1]);

    // Close reader.
    if (Cmd->In)
      dup2(CmdWrite[0], STDIN_FILENO);

    close(CmdWrite[0]);
    close(CmdRead[0]);

    // Redir stdout and stderr to pipe and close writer.
    dup2(CmdRead[1], STDOUT_FILENO);
    dup2(CmdRead[1], STDERR_FILENO);
    close(CmdRead[1]);

    kv_push(char *, Cmd->Args, NULL);
    if (execvp(kv_A(Cmd->Args, 0), Cmd->Args.a) == -1)
      perror("tsh");

    exit(EXIT_FAILURE);
  } else {
    close(CmdRead[1]);
    close(CmdWrite[0]);

    // If we have some stdin value to pipe into the current cmd.
    if (Cmd->In)
      write(CmdWrite[1], Cmd->In, Cmd->InSize);

    // Close writer.
    close(CmdWrite[1]);

    size_t BufSize = sizeof(char) * TSH_BUF_INCREMENT;
    char *Buf = malloc(BufSize);
    if (!Buf)
      return -1;

    size_t BufOffset = 0;
    while (1) {
      ssize_t AmtRead = read(CmdRead[0], Buf + BufOffset, TSH_BUF_INCREMENT);
      if (AmtRead == 0)
        break;

      BufSize += sizeof(char) * TSH_BUF_INCREMENT;
      Buf = realloc(Buf, BufSize);
      if (!Buf)
        return -1;

      BufOffset += AmtRead;
    }

    Buf[BufOffset] = '\0';
    Cmd->Out = Buf;
    Cmd->OutSize = BufOffset;
    printf("%s", Cmd->Out);

    // Spin until cmd has finished executing.
    pid_t WaitPid;
    do {
      WaitPid = waitpid(Pid, &Status, WUNTRACED);
    } while (!WIFEXITED(Status) && !WIFSIGNALED(Status));

    // Record last code.
    return Status;
  }
}

static int _tshEngineExecPipe(TshEngine *E, TshCmd *Left, TshCmd *Right) {
#ifndef NDEBUG
  printf("tsh: executing pipe.\n");
#endif

  TSH_RET(_tshEngineExecImpl(E, Left));

  if (!Left->Out)
    return -1;

  // Pipe left cmd's stdout to right cmd's stdin.
  Right->In = Left->Out;
  Right->InSize = Left->OutSize;

  tshEngineExec(E, Right);
  return 0;
}

static int _tshEngineExecRedir(TshEngine *E, TshCmd *Left, TshCmd *Right) {
#ifndef NDEBUG
  printf("tsh: executing redir.\n");
#endif

  TSH_RET(_tshEngineExecImpl(E, Left));

  if (kv_size(Right->Args) != 1)
    return -1;

  FILE *RedirF = fopen(kv_A(Right->Args, 0), "w");
  if (!RedirF)
    return -1;

  size_t Written = fwrite(Left->Out, sizeof(char), Left->OutSize, RedirF);
  fclose(RedirF);
  if (Written == 0)
    return -1;

  return 0;
}

static int _tshEngineExecReverseRedir(TshEngine *E, TshCmd *Left,
                                      TshCmd *Right) {
#ifndef NDEBUG
  printf("tsh: executing reverse redir.\n");
#endif

  if (kv_size(Right->Args) != 1)
    return -1;

  FILE *RedirF = fopen(kv_A(Right->Args, 0), "r");
  if (!RedirF)
    return -1;

  // Figure out file length.
  fseek(RedirF, 0, SEEK_END);
  size_t Length = ftell(RedirF);
  rewind(RedirF);

  // Read entire file.
  char *Buf = malloc(sizeof(char) * (Length + 1));
  if (!Buf) {
    fclose(RedirF);
    return -1;
  }

  size_t Read = fread(Buf, sizeof(char), Length, RedirF);
  fclose(RedirF);
  if (Read == 0)
    return -1;

  Buf[Length] = '\0';
  Right->Out = Buf;
  Right->OutSize = Length;

  Left->In = Buf;
  Left->InSize = Length;

  TSH_RET(_tshEngineExecImpl(E, Left));
  return 0;
}
