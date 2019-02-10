#include "Engine.h"

#include <Util.h>

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define TSH_BUF_INCREMENT 1024

static int _tshEngineExecCmd(TshCmd *);
static int _tshEngineExecPipe(TshCmd *, TshCmd *);
static int _tshEngineExecRedir(TshCmd *, TshCmd *);
static int _tshEngineExecReverseRedir(TshCmd *, TshCmd *);

int tshEngineExec(TshCmd *Cmd) {
  switch (Cmd->Op) {
  case TK_None:
    return _tshEngineExecCmd(Cmd);
    break;
  case TK_Pipe:
    return _tshEngineExecPipe(Cmd->Left, Cmd->Right);
    break;
  case TK_Redir:
    return _tshEngineExecRedir(Cmd->Left, Cmd->Right);
    break;
  case TK_ReverseRedir:
    return _tshEngineExecReverseRedir(Cmd->Left, Cmd->Right);
    break;
  default:
    return -1;
  }
}

static int _tshEngineExecCmd(TshCmd *Cmd) {
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
    dup2(CmdRead[1], 1);
    dup2(CmdRead[1], 2);
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
    Cmd->OutSize = BufOffset + 1;
    printf("%s", Cmd->Out);
  }

  return 0;
}

static int _tshEngineExecPipe(TshCmd *Left, TshCmd *Right) {
#ifndef NDEBUG
  printf("tsh: executing pipe.\n");
#endif

  TSH_RET(tshEngineExec(Left));

  if (!Left->Out)
    return -1;

  // Pipe left cmd's stdout to right cmd's stdin.
  Right->In = Left->Out;
  Right->InSize = Left->OutSize;

  tshEngineExec(Right);
  return 0;
}

static int _tshEngineExecRedir(TshCmd *Left, TshCmd *Right) {
#ifndef NDEBUG
  printf("tsh: executing redir.\n");
#endif

  TSH_RET(tshEngineExec(Left));

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

static int _tshEngineExecReverseRedir(TshCmd *Left, TshCmd *Right) {
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
  if (!Buf)
    return -1;

  size_t Read = fread(Buf, sizeof(char), Length, RedirF);
  fclose(RedirF);
  if (Read == 0)
    return -1;

  Buf[Length] = '\0';
  Right->Out = Buf;
  Right->OutSize = Length;

  Left->In = Buf;
  Left->InSize = Length;

  TSH_RET(tshEngineExec(Left));
  return 0;
}
