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

static void _tshRunInput(TshEngine *, const char *);

int tsh(int ArgC, char **ArgV) {
  TshEngine E;
  tshEngineInit(&E);

  // More than one arg is invalid.
  if (ArgC > 2) {
    return EXIT_FAILURE;
  }
  // If we have a single argument then execute it and exit.
  else if (ArgC == 2) {
    _tshRunInput(&E, ArgV[1]);
    return EXIT_SUCCESS;
  }

  // REPL.
  while (!E.Exiting) {
    // Read input.
    char *Buf = tshPrompt();
    if (!Buf)
      break;

    _tshRunInput(&E, Buf);
    free(Buf);
  }

  printf("tsh: closing.\n");
  return EXIT_SUCCESS;
}

static void _tshRunInput(TshEngine *E, const char *Buf) {
  size_t BufSize = strlen(Buf);

  // Lex into tokens.
  TshLex L;
  TshToken T;
  TshTokenVec Tokens;
  tshLexInit(&L, Buf, BufSize);
  kv_init(Tokens);

  while (1) {
    tshLexGetToken(&L, &T);
    if (T.Kind == TK_EndOfFile)
      break;

    kv_push(TshToken, Tokens, T);
  }

#ifndef NDEBUG
  for (KV_FOREACH(Index, Tokens)) {
    TshToken *T = &kv_A(Tokens, Index);
    printf("tsh: read token. Kind=%d ", T->Kind);
    if (T->Kind == TK_Identifier)
      printf("Value=%.*s\n", (int)T->BufSize, T->Buf);
    else
      printf("Value=%s\n", tshTokenKindToString(T->Kind));
  }
#endif

  TshParse P;
  tshParseInit(&P, Tokens);

  TshCmd *C = tshParseCmd(&P);
  if (C) {
    tshEngineExec(E, C);
    if (E->Status != 0)
      fprintf(stderr, "tsh: tshEngineExec failed. Ret=%d\n", E->Status);

    tshCmdClose(C);
  } else {
    fprintf(stderr, "tsh: tshParseCmd failed.\n");
  }

  tshLexClose(&L);
  tshParseClose(&P);
  kv_destroy(Tokens);
}
