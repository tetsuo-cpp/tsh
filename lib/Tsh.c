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
    TshToken T;
    tshLexInit(&L, Buf, strlen(Buf));
    do {
      tshLexGetToken(&L, &T);
      printf("Got token of kind %d.\n", T.Kind);
      if (T.Kind == TK_Identifier)
        printf("Value was %.*s\n", T.BufSize - 1, T.Buf);
    } while (T.Kind != TK_EndOfFile);
    tshLexClose(&L);

    free(Buf);
  }
  return EXIT_SUCCESS;
}
