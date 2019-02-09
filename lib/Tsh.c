#include "Tsh.h"

#include <Engine.h>
#include <Lex.h>
#include <Parse.h>
#include <Prompt.h>
#include <Util.h>

#include <klib/kvec.h>

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
    char *Buf = tshPrompt();
    size_t BufSize = strlen(Buf);

    // Lex into tokens.
    TshLex L;
    TshToken T;
    TshTokenVec Tokens;
    tshLexInit(&L, Buf, BufSize);
    kv_init(Tokens);

    while (1) {
      tshLexGetToken(&L, &T);
      kv_push(TshToken, Tokens, T);
      printf("Got token of kind %d.\n", T.Kind);

      if (T.Kind == TK_Identifier)
        printf("Value was %.*s\n", (int)T.BufSize, T.Buf);

      if (T.Kind == TK_EndOfFile)
        break;
    }

    TshParse P;
    tshParseInit(&P, Tokens);
    TshCmd *C = tshParseCmd(&P);
    if (!C)
      break;

    tshEngineExec(C);

    tshCmdClose(C);
    tshLexClose(&L);
    tshParseClose(&P);
    kv_destroy(Tokens);
    free(Buf);
  }

  return EXIT_SUCCESS;
}
