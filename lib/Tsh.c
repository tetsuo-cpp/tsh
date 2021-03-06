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

static void tshRunInput(TshEngine *, const char *);

const char *TshDBName = "/tmp/tsh.db";

int tsh(int ArgC, char **ArgV) {
  TshDataBase DB;
  if (!tshDataBaseInit(&DB, TshDBName))
    return -1;

  TshEngine E;
  tshEngineInit(&E, &DB);

  // More than one arg is invalid.
  if (ArgC > 2) {
    tshDataBaseDestroy(&DB);
    return EXIT_FAILURE;
  }
  // If we have a single argument then execute it and exit.
  else if (ArgC == 2) {
    tshRunInput(&E, ArgV[1]);
    tshDataBaseDestroy(&DB);
    return E.Status;
  }

  // REPL.
  while (!E.Exiting) {
    // Read input.
    char *Buf = tshPrompt();
    if (!Buf)
      break;

    tshRunInput(&E, Buf);
    free(Buf);
  }

  tshDataBaseDestroy(&DB);
  printf("tsh: closing.\n");
  return EXIT_SUCCESS;
}

static void tshRunInput(TshEngine *E, const char *Buf) {
  size_t BufSize = strlen(Buf);

  TshLex L;
  tshLexInit(&L, Buf, BufSize);

  TshTokenVec Tokens;
  kv_init(Tokens);

  // Lex into tokens.
  while (1) {
    TshToken T;
    tshLexGetToken(&L, &T);
    if (T.Kind == TK_EndOfFile)
      break;

    kv_push(TshToken, Tokens, T);
  }

#ifndef NDEBUG
  for (KV_FOREACH(Index, Tokens)) {
    TshToken *T = &kv_A(Tokens, Index);
    TSH_DBG_PRINTF("tsh: read token. Kind=%d ", T->Kind);
    if (T->Kind == TK_Identifier)
      TSH_DBG_PRINTF("Value=%.*s\n", (int)T->BufSize, T->Buf);
    else
      TSH_DBG_PRINTF("Value=%s\n", tshTokenKindToString(T->Kind));
  }
#endif

  TshParse P;
  tshParseInit(&P, Tokens);

  TshCmd *C = tshParseCmd(&P);
  if (C) {
    tshEngineExec(E, C);
    if (E->Status != 0)
      fprintf(stderr, "tsh: tshEngineExec failed. Ret=%d\n", E->Status);

    tshCmdDestroy(C);
  } else {
    fprintf(stderr, "tsh: tshParseCmd failed.\n");
  }

  tshLexDestroy(&L);
  tshParseDestroy(&P);
  kv_destroy(Tokens);
}
