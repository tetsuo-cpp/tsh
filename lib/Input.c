#include "Input.h"

#include <stdio.h>

char *tshReadLine() {
  char *Line = NULL;
  size_t BufSize = 0;

  getline(&Line, &BufSize, stdin);

  return Line;
}
