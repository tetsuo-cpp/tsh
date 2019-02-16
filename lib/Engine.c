#include "Engine.h"

#include <BuiltIn.h>
#include <Util.h>

#include <stdio.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define TSH_BUF_INCREMENT 1024

static int _tshEngineExecImpl(TshEngine *, TshCmd *, bool);
static int _tshEngineExecCmd(TshEngine *, TshCmd *, bool);
static int _tshEngineExecPipe(TshEngine *, TshCmd *, bool);
static int _tshEngineExecRedir(TshEngine *, TshCmd *, bool);
static int _tshEngineExecReverseRedir(TshEngine *, TshCmd *, bool);
static void _tshEngineChildExec(TshCmd *, int, int, bool);
static int _tshEngineParentExec(TshCmd *, int, int, bool, pid_t);

void tshEngineInit(TshEngine *E, TshDataBase *DB) {
  E->Status = 0;
  E->Exiting = false;
  E->DB = DB;
}

void tshEngineExec(TshEngine *E, TshCmd *Cmd) {
  E->Status = _tshEngineExecImpl(E, Cmd, true);
}

static int _tshEngineExecImpl(TshEngine *E, TshCmd *Cmd, bool Interactive) {
  switch (Cmd->Op) {
  case TK_None:
    return _tshEngineExecCmd(E, Cmd, Interactive);
  case TK_Pipe:
    return _tshEngineExecPipe(E, Cmd, Interactive);
  case TK_Redir:
    return _tshEngineExecRedir(E, Cmd, Interactive);
  case TK_ReverseRedir:
    return _tshEngineExecReverseRedir(E, Cmd, Interactive);
  default:
    return -1;
  }
}

static int _tshEngineExecCmd(TshEngine *E, TshCmd *Cmd, bool Interactive) {
#ifndef NDEBUG
  printf("tsh: executing cmd. Cmd=%s Interactive=%d Args=[", kv_A(Cmd->Args, 0),
         Interactive);
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
  // CmdRead: Cmd -> Shell.
  // CmdWrite: Shell -> Cmd.
  pipe(CmdRead);
  pipe(CmdWrite);

  Pid = fork();
  if (Pid == 0) {
    // Parent will use these.
    close(CmdRead[0]);
    close(CmdWrite[1]);

    _tshEngineChildExec(Cmd, CmdWrite[0], CmdRead[1], Interactive);

    exit(EXIT_FAILURE);
  } else {
    // Child will use these.
    close(CmdRead[1]);
    close(CmdWrite[0]);

    struct timespec StartTime, EndTime;
    if (clock_gettime(CLOCK_REALTIME, &StartTime))
      return -1;

    Status =
        _tshEngineParentExec(Cmd, CmdRead[0], CmdWrite[1], Interactive, Pid);

    if (clock_gettime(CLOCK_REALTIME, &EndTime))
      return -1;

    double Duration = tshTimeDiff(&StartTime, &EndTime);
    tshDataBaseRecordDuration(E->DB, kv_A(Cmd->Args, 0), Duration);

    return Status;
  }
}

static int _tshEngineExecPipe(TshEngine *E, TshCmd *Cmd, bool Interactive) {
#ifndef NDEBUG
  printf("tsh: executing pipe.\n");
#endif

  Cmd->Left->In = Cmd->In;
  Cmd->Left->InSize = Cmd->InSize;

  TSH_RET(_tshEngineExecImpl(E, Cmd->Left, false));

  if (!Cmd->Left->Out)
    return -1;

  // Pipe left cmd's stdout to right cmd's stdin.
  Cmd->Right->In = Cmd->Left->Out;
  Cmd->Right->InSize = Cmd->Left->OutSize;

  TSH_RET(_tshEngineExecImpl(E, Cmd->Right, Interactive));
  return 0;
}

static int _tshEngineExecRedir(TshEngine *E, TshCmd *Cmd, bool Interactive) {
  (void)Interactive;
#ifndef NDEBUG
  printf("tsh: executing redir.\n");
#endif

  Cmd->Left->In = Cmd->In;
  Cmd->Left->InSize = Cmd->InSize;

  TSH_RET(_tshEngineExecImpl(E, Cmd->Left, false));

  if (kv_size(Cmd->Right->Args) != 1)
    return -1;

  FILE *RedirF = fopen(kv_A(Cmd->Right->Args, 0), "w");
  if (!RedirF)
    return -1;

  size_t Written =
      fwrite(Cmd->Left->Out, sizeof(char), Cmd->Left->OutSize, RedirF);
  fclose(RedirF);
  if (Written == 0)
    return -1;

  return 0;
}

static int _tshEngineExecReverseRedir(TshEngine *E, TshCmd *Cmd,
                                      bool Interactive) {
#ifndef NDEBUG
  printf("tsh: executing reverse redir.\n");
#endif

  if (kv_size(Cmd->Right->Args) != 1)
    return -1;

  FILE *RedirF = fopen(kv_A(Cmd->Right->Args, 0), "r");
  if (!RedirF)
    return -1;

  // Figure out file length.
  fseek(RedirF, 0, SEEK_END);
  size_t Length = (size_t)ftell(RedirF);
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
  Cmd->Right->Out = Buf;
  Cmd->Right->OutSize = Length;

  Cmd->Left->In = Buf;
  Cmd->Left->InSize = Length;

  TSH_RET(_tshEngineExecImpl(E, Cmd->Left, Interactive));
  return 0;
}

void _tshEngineChildExec(TshCmd *Cmd, int Reader, int Writer,
                         bool Interactive) {
  // Close reader.
  if (Cmd->In)
    dup2(Reader, STDIN_FILENO);

  close(Reader);

  // Redir stdout and stderr to pipe and close writer.
  if (!Interactive) {
    dup2(Writer, STDOUT_FILENO);
    dup2(Writer, STDERR_FILENO);
  }

  close(Writer);

  kv_push(char *, Cmd->Args, NULL);
  if (execvp(kv_A(Cmd->Args, 0), Cmd->Args.a) == -1)
    perror("tsh");
}

int _tshEngineParentExec(TshCmd *Cmd, int Reader, int Writer, bool Interactive,
                         pid_t Pid) {
  // If we have some stdin value to pipe into the current cmd.
  if (Cmd->In)
    write(Writer, Cmd->In, Cmd->InSize);

  // Close writer.
  close(Writer);

  if (!Interactive) {
    size_t BufSize = sizeof(char) * TSH_BUF_INCREMENT;
    char *Buf = malloc(BufSize);
    if (!Buf)
      return -1;

    size_t BufOffset = 0;
    while (1) {
      ssize_t AmtRead = read(Reader, Buf + BufOffset, TSH_BUF_INCREMENT);
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
  }

  close(Reader);

  // Spin until cmd has finished executing.
  int Status;
  do {
    waitpid(Pid, &Status, WUNTRACED);
  } while (!WIFEXITED(Status) && !WIFSIGNALED(Status));

  // Record last code.
  return Status;
}
