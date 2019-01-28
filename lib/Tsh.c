#include "Tsh.h"

#include <Engine.h>
#include <Lex.h>
#include <Parse.h>

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
    char *Buf = NULL;
    size_t BufSize = 0;
    printf("> ");
    getline(&Buf, &BufSize, stdin);

    // Lex into tokens.
    TshLex L;
    TshToken T;
    TshTokenVec Tokens;
    tshLexInit(&L, Buf, strlen(Buf));
    kv_init(Tokens);
    do {
      tshLexGetToken(&L, &T);
      kv_push(TshToken, Tokens, T);
      printf("Got token of kind %d.\n", T.Kind);
      if (T.Kind == TK_Identifier)
        printf("Value was %.*s\n", T.BufSize, T.Buf);
    } while (T.Kind != TK_EndOfFile);

    TshParse P;
    TshCmd C;
    TshCmdVec Cmds;
    kv_init(Cmds);
    tshParseInit(&P, Tokens);
    while (tshParseCmd(&P, &C)) {
      kv_push(TshCmd, Cmds, C);
    }

    for (unsigned int Index = 0; Index < kv_size(Cmds); ++Index) {
      const TshCmd *Cmd = &kv_A(Cmds, Index);
      printf("Printing Cmd %d with and op %d.\n", Index, Cmd->Op);
      for (unsigned int ArgIndex = 0; ArgIndex < kv_size(Cmd->Args);
           ++ArgIndex) {
        printf("Arg %d with value %s.\n", ArgIndex, kv_A(Cmd->Args, ArgIndex));
      }
    }

    TshEngine E;
    tshEngineInit(&E, Cmds);
    tshEngineExec(&E);

    tshLexClose(&L);
    tshParseClose(&P);
    kv_destroy(Tokens);
    kv_destroy(Cmds);
    free(Buf);
  }

  return EXIT_SUCCESS;
}
