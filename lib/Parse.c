#include "Parse.h"

#include <klib/kvec.h>

#include <stdlib.h>

bool _tshParseReadToken(TshParse *);
bool _tshParseConsumeToken(TshParse *, TshTokenKind);

void tshParseInit(TshParse *P, TshTokenVec Tokens) {
  P->Tokens = Tokens;
  P->TokenPos = 0;
  P->CurTok = &kv_A(Tokens, 0);
}

bool tshParseCmd(TshParse *P, TshCmd *Cmd) {
  if (!P->CurTok || P->CurTok->Kind == TK_EndOfFile)
    return false;
  const TshToken *T = P->CurTok;
  if (_tshParseConsumeToken(P, TK_Identifier)) {
    tshCmdInit(Cmd, T->Buf, T->BufSize);
    T = P->CurTok;
    while (_tshParseConsumeToken(P, TK_Identifier)) {
      tshCmdAddArg(Cmd, T->Buf, T->BufSize);
    }
    Cmd->Op = P->CurTok->Kind;
    _tshParseReadToken(P);
    return true;
  }
  return false;
}

void tshParseClose(TshParse *P) {
  P->TokenPos = 0;
  P->CurTok = NULL;
}

bool _tshParseReadToken(TshParse *P) {
  if (P->TokenPos >= kv_size(P->Tokens)) {
    P->CurTok = NULL;
    return false;
  }
  P->CurTok = &kv_A(P->Tokens, ++P->TokenPos);
  return true;
}

bool _tshParseConsumeToken(TshParse *P, TshTokenKind Kind) {
  if (P && P->CurTok->Kind == Kind) {
    _tshParseReadToken(P);
    return true;
  }
  return false;
}
