#include "Tsh.h"

#include "BuiltIn.h"
#include "Exec.h"
#include "Input.h"
#include "Parse.h"

#include <stdio.h>
#include <stdlib.h>

void tsh() {
  char *Line;
  char **Args;
  int Status;

  do {
    printf("> ");

    Line = tshReadLine();
    Args = tshSplitLine(Line);
    Status = tshExecute(Args);

    free(Line);
    free(Args);
  } while (Status);
}
