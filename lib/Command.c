#include "Command.h"

#include <stdlib.h>
#include <string.h>

void tshCommandInit(TshCommand *Command, const char *Buf,
                    unsigned int BufSize) {
  Command->Cmd = malloc(sizeof(sizeof(char) * (BufSize + 1)));
  strncpy(Command->Cmd, Buf, BufSize);
  Command->Cmd[BufSize] = '\0';
  Command->Args = NULL;
  Command->ArgsSize = 0;
  Command->Op = -1;
}

void tshCommandAddArg(TshCommand *Command, const char *Buf,
                      unsigned int BufSize) {
  unsigned int NewSize = Command->ArgsSize + 1;
  Command->Args = realloc(Command->Args, sizeof(char *) * NewSize);
  Command->Args[Command->ArgsSize] = malloc(sizeof(char) * (BufSize + 1));
  strncpy(Command->Args[Command->ArgsSize], Buf, BufSize);
  Command->Args[Command->ArgsSize][BufSize] = '\0';
  Command->ArgsSize = NewSize;
}

void tshCommandClose(TshCommand *Command) {
  free(Command->Cmd);
  Command->Cmd = NULL;
  if (Command->Args)
    free(Command->Args);
  Command->Args = NULL;
  Command->ArgsSize = 0;
  Command->Op = -1;
}
