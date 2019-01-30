#include "Parse.h"

#include <stdio.h>
#include <stdlib.h>

static bool _tshParseReadToken(TshParse *);
static bool _tshParseConsumeToken(TshParse *, TshTokenKind);

void tshParseInit(TshParse *P, TshTokenVec Tokens) {
  P->Tokens = Tokens;
  P->TokenPos = 0;

  _tshParseReadToken(P);
}

TshCmd *tshParseCmd(TshParse *P, TshCmd *Prev) {
  if (!P->CurTok || P->CurTok->Kind == TK_EndOfFile)
    return Prev;

  TshCmd *Cmd = malloc(sizeof(TshCmd));
  tshCmdInit(Cmd);

  const TshToken *T = P->CurTok;
  if (_tshParseConsumeToken(P, TK_Identifier)) {
    tshCmdAddArg(Cmd, T->Buf, T->BufSize);
    T = P->CurTok;

    while (_tshParseConsumeToken(P, TK_Identifier)) {
      tshCmdAddArg(Cmd, T->Buf, T->BufSize);
      T = P->CurTok;
    }

    return tshParseCmd(P, Cmd);
  } else if (_tshParseConsumeToken(P, TK_Pipe) ||
             _tshParseConsumeToken(P, TK_Redir)) {
    Cmd->Op = T->Kind;
    Cmd->Left = Prev;
    Cmd->Right = tshParseCmd(P, Cmd);
    return Cmd;
  } else {
    printf("Unrecognised token kind.\n");
    return NULL;
  }
}

void tshParseClose(TshParse *P) {
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
