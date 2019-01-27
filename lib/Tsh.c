#include "Tsh.h"

#include <Lex.h>
#include <Parse.h>

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

    unsigned int TCount = 0;
    TshToken *Tokens = NULL;
    do {
      ++TCount;
      Tokens = realloc(Tokens, sizeof(TshToken) * TCount);
      TshToken *T = &Tokens[TCount - 1];
      tshLexGetToken(&L, T);
      printf("Got token of kind %d.\n", T->Kind);
      if (T->Kind == TK_Identifier)
        printf("Value was %.*s\n", T->BufSize - 1, T->Buf);
    } while (T.Kind != TK_EndOfFile);

    TshParse P;
    tshParseInit(&P, Tokens, TCount);
    tshParseCmd(&P);

    // Print out the cmds.
    for (unsigned int Index = 0; Index < P.CmdsSize; ++Index) {
      const TshCmd *Cmd = &P.Cmds[Index];
      printf("Printing Cmd %d with value %s.\n", Index, Cmd->Cmd);
      for (unsigned int ArgIndex = 0; ArgIndex < Cmd->ArgsSize; ++ArgIndex) {
        printf("Arg %d with value %s.\n", ArgIndex, Cmd->Args[ArgIndex]);
      }
    }

    tshLexClose(&L);
    tshParseClose(&P);
    free(Buf);
  }
  return EXIT_SUCCESS;
}
