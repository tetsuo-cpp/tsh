#include "Parse.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool tshParseReadToken(TshParse *);
static bool tshParseConsumeToken(TshParse *, TshTokenKind);
static TshCmd *tshParseCmdImpl(TshParse *, TshCmd *);

void tshParseInit(TshParse *P, TshTokenVec Tokens) {
  P->Tokens = Tokens;
  P->TokenPos = 0;

  tshParseReadToken(P);
}

TshCmd *tshParseCmd(TshParse *P) { return tshParseCmdImpl(P, NULL); }

void tshParseDestroy(TshParse *P) {
  P->TokenPos = 0;
  P->CurTok = NULL;
}

static bool tshParseReadToken(TshParse *P) {
  if (P->TokenPos >= kv_size(P->Tokens)) {
    P->CurTok = NULL;
    return false;
  }

  P->CurTok = &kv_A(P->Tokens, P->TokenPos);
  ++P->TokenPos;
  return true;
}

static bool tshParseConsumeToken(TshParse *P, TshTokenKind Kind) {
  if (P->CurTok && P->CurTok->Kind == Kind) {
    tshParseReadToken(P);
    return true;
  }

  return false;
}

static TshCmd *tshParseCmdImpl(TshParse *P, TshCmd *Prev) {
  if (!P->CurTok || P->CurTok->Kind == TK_EndOfFile)
    return Prev;

  TshCmd *Cmd = malloc(sizeof(TshCmd));
  if (!Cmd)
    return NULL;

  tshCmdInit(Cmd);

  const TshToken *T = P->CurTok;
  if (tshParseConsumeToken(P, TK_Identifier)) {
    tshCmdAddArg(Cmd, T->Buf, T->BufSize);
    T = P->CurTok;

    while (tshParseConsumeToken(P, TK_Identifier)) {
      tshCmdAddArg(Cmd, T->Buf, T->BufSize);
      T = P->CurTok;
    }

    return tshParseCmdImpl(P, Cmd);
  } else if (tshParseConsumeToken(P, TK_Pipe) ||
             tshParseConsumeToken(P, TK_Redir) ||
             tshParseConsumeToken(P, TK_ReverseRedir)) {
    Cmd->Op = T->Kind;
    Cmd->Left = Prev;
    Cmd->Right = tshParseCmdImpl(P, NULL);

    return Cmd;
  } else {
    free(Cmd);
    return NULL;
  }
}
