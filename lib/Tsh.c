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

static void _tshRunInput(const char *);

int tsh(int ArgC, char **ArgV) {
  // More than one arg is invalid.
  if (ArgC > 2) {
    return EXIT_FAILURE;
  }
  // If we have a single argument then execute it and exit.
  else if (ArgC == 2) {
    _tshRunInput(ArgV[1]);
    return EXIT_SUCCESS;
  }

  // REPL.
  int Status = 1;
  while (Status) {
    // Read input.
    char *Buf = tshPrompt();
    if (!Buf) {
      fprintf(stderr, "tsh: closing.\n");
      return EXIT_SUCCESS;
    }

    _tshRunInput(Buf);
    free(Buf);
  }

  return EXIT_SUCCESS;
}

static void _tshRunInput(const char *Buf) {
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
    if (tshEngineExec(C) != 0)
      fprintf(stderr, "tsh: tshEngineExec failed.\n");

    tshCmdClose(C);
  } else {
    fprintf(stderr, "tsh: tshParseCmd failed.\n");
  }

  tshLexClose(&L);
  tshParseClose(&P);
  kv_destroy(Tokens);
}
