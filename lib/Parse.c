#include "Parse.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool _tshParseReadToken(TshParse *);
static bool _tshParseConsumeToken(TshParse *, TshTokenKind);
static TshCmd *_tshParseCmdImpl(TshParse *, TshCmd *);

void tshParseInit(TshParse *P, TshTokenVec Tokens) {
  P->Tokens = Tokens;
  P->TokenPos = 0;

  _tshParseReadToken(P);
}

TshCmd *tshParseCmd(TshParse *P) { return _tshParseCmdImpl(P, NULL); }

void tshParseDestroy(TshParse *P) {
  P->TokenPos = 0;
  P->CurTok = NULL;
}

static bool _tshParseReadToken(TshParse *P) {
  if (P->TokenPos >= kv_size(P->Tokens)) {
    P->CurTok = NULL;
    return false;
  }

  P->CurTok = &kv_A(P->Tokens, P->TokenPos);
  ++P->TokenPos;
  return true;
}

static bool _tshParseConsumeToken(TshParse *P, TshTokenKind Kind) {
  if (P->CurTok && P->CurTok->Kind == Kind) {
    _tshParseReadToken(P);
    return true;
  }

  return false;
}

static TshCmd *_tshParseCmdImpl(TshParse *P, TshCmd *Prev) {
  if (!P->CurTok || P->CurTok->Kind == TK_EndOfFile)
    return Prev;

  TshCmd *Cmd = malloc(sizeof(TshCmd));
  if (!Cmd)
    return NULL;

  tshCmdInit(Cmd);

  const TshToken *T = P->CurTok;
  if (_tshParseConsumeToken(P, TK_Identifier)) {
    tshCmdAddArg(Cmd, T->Buf, T->BufSize);
    T = P->CurTok;

    while (_tshParseConsumeToken(P, TK_Identifier)) {
      tshCmdAddArg(Cmd, T->Buf, T->BufSize);
      T = P->CurTok;
    }

    return _tshParseCmdImpl(P, Cmd);
  } else if (_tshParseConsumeToken(P, TK_Pipe) ||
             _tshParseConsumeToken(P, TK_Redir) ||
             _tshParseConsumeToken(P, TK_ReverseRedir)) {
    Cmd->Op = T->Kind;
    Cmd->Left = Prev;
    Cmd->Right = _tshParseCmdImpl(P, NULL);

    return Cmd;
  } else {
    free(Cmd);
    return NULL;
  }
}
