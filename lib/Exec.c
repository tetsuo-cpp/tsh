#include "Exec.h"

#include "BuiltIn.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int tshExecute(char **Args) {
  int I;

  if (!Args[0])
    return 1;

  for (I = 0; I < tshNumBuiltIns(); ++I) {
    if (strcmp(Args[0], BuiltInStr[I]) == 0) {
      return (*BuiltInFunc[I])(Args);
    }
  }

  return tshLaunch(Args);
}

int tshLaunch(char **Args) {
  pid_t Pid, WPid;
  int Status;

  Pid = fork();
  if (Pid == 0) {
    if (execvp(Args[0], Args) == -1) {
      perror("tsh");
    }

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
