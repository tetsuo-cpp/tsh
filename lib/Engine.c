#include "Engine.h"

#include <Util.h>

#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define TSH_BUF_SIZE 1024

static int _tshEngineExecCmd(TshCmd *);
static void _tshEngineExecPipe(TshCmd *, TshCmd *);
static void _tshEngineExecRedir(TshCmd *, TshCmd *);
static void _tshEngineExecReverseRedir(TshCmd *, TshCmd *);

void tshEngineExec(TshCmd *Cmd) {
  switch (Cmd->Op) {
  case TK_None:
    _tshEngineExecCmd(Cmd);
    break;
  case TK_Pipe:
    _tshEngineExecPipe(Cmd->Left, Cmd->Right);
    break;
  case TK_Redir:
    _tshEngineExecRedir(Cmd->Left, Cmd->Right);
    break;
  case TK_ReverseRedir:
    _tshEngineExecReverseRedir(Cmd->Left, Cmd->Right);
    break;
  default:
    printf("Unrecognised operator.");
  }
}

static int _tshEngineExecCmd(TshCmd *Cmd) {
  printf("Executing cmd %s.\n", kv_A(Cmd->Args, 0));

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
  } else if (Pid < 0) {
    perror("tsh");
  } else {
    close(CmdRead[1]);
    close(CmdWrite[0]);

    // If we have some stdin value to pipe into the current cmd.
    if (Cmd->In)
      write(CmdWrite[1], Cmd->In, Cmd->InSize);

    // Close writer.
    close(CmdWrite[1]);

    unsigned int BufSize = sizeof(char) * TSH_BUF_SIZE;
    char *Buf = malloc(BufSize);
    unsigned int BufOffset = 0;
    while (1) {
      unsigned int AmtRead = read(CmdRead[0], Buf + BufOffset, TSH_BUF_SIZE);
      if (AmtRead == 0)
        break;

      BufSize += sizeof(char) * TSH_BUF_SIZE;
      Buf = realloc(Buf, BufSize);
      BufOffset += AmtRead;
    }

    Buf[BufOffset] = '\0';
    Cmd->Out = Buf;
    Cmd->OutSize = BufOffset + 1;
    printf("%s.\n", Cmd->Out);
  }

  return 1;
}

static void _tshEngineExecPipe(TshCmd *Left, TshCmd *Right) {
  printf("Executing pipe.\n");
  tshEngineExec(Left);

  if (!Left->Out)
    return;

  // Pipe left cmd's stdout to right cmd's stdin.
  Right->In = Left->Out;
  Right->InSize = Left->OutSize;

  tshEngineExec(Right);
}

static void _tshEngineExecRedir(TshCmd *Left, TshCmd *Right) {
  printf("Executing redir.\n");
  tshEngineExec(Left);

  if (kv_size(Right->Args) != 1)
    return;

  FILE *RedirF = fopen(kv_A(Right->Args, 0), "w");
  fwrite(Left->Out, sizeof(char), Left->OutSize, RedirF);
  fclose(RedirF);
}

static void _tshEngineExecReverseRedir(TshCmd *Left, TshCmd *Right) {
  printf("Executing reverse redir.\n");
  if (kv_size(Right->Args) != 1)
    return;

  FILE *RedirF = fopen(kv_A(Right->Args, 0), "r");

  // Figure out file length.
  fseek(RedirF, 0, SEEK_END);
  size_t Length = ftell(RedirF);
  rewind(RedirF);

  // Read entire file.
  char *Buf = malloc(sizeof(char) * (Length + 1));
  fread(Buf, sizeof(char), Length, RedirF);
  fclose(RedirF);

  Buf[Length] = '\0';
  Right->Out = Buf;
  Right->OutSize = Length;

  Left->In = Buf;
  Left->InSize = Length;
  tshEngineExec(Left);
}
