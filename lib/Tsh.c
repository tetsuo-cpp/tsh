#include "Tsh.h"

#include <Lex.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tsh(int ArgC, char **ArgV) {
  (void)ArgV;
  if (ArgC != 1)
    return EXIT_FAILURE;

  int Status = 1;
  while (Status) {
    // Read input.
    char *Buf = NULL;
    size_t BufSize = 0;
    printf("> ");
    getline(&Buf, &BufSize, stdin);

    // Lex into tokens.
    TshLex L;
    TshLexToken T;
    tshLexInit(&L, Buf, strlen(Buf));
    do {
      tshLexGetToken(&L, &T);
      printf("Got token of kind %d.\n", T.Kind);
      if (T.Kind == IDENTIFIER)
        printf("Value was %.*s\n", T.BufSize, T.Buf);
    } while (T.Kind != END_OF_FILE);
    tshLexClose(&L);
  }
  return EXIT_SUCCESS;
}
