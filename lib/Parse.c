#include "Parse.h"

#include <stdlib.h>

bool _tshParseReadToken(TshParse *);
bool _tshParseConsumeToken(TshParse *, TshTokenKind);
void _tshParseAddCmd(TshParse *);

void tshParseInit(TshParse *P, const TshToken *Tokens,
                  unsigned int TokensSize) {
  P->Tokens = Tokens;
  P->TokensSize = TokensSize;
  P->TokenPos = 0;
  P->CurTok = &Tokens[0];
  P->Cmds = NULL;
  P->CmdsSize = 0;
}

bool tshParseCmd(TshParse *P) {
  // Parse a single cmd.
  while (P->CurTok && P->CurTok->Kind != TK_EndOfFile) {
    _tshParseAddCmd(P);
    TshCmd *Cmd = &P->Cmds[P->CmdsSize - 1];
    const TshToken *T = P->CurTok;
    if (_tshParseConsumeToken(P, TK_Identifier)) {
      tshCmdInit(Cmd, T->Buf, T->BufSize);
      T = P->CurTok;
      while (_tshParseConsumeToken(P, TK_Identifier)) {
        tshCmdAddArg(Cmd, T->Buf, T->BufSize);
      }
      Cmd->Op = P->CurTok->Kind;
      _tshParseReadToken(P);
    } else {
      return false;
    }
  }
  return false;
}

void tshParseClose(TshParse *P) {
  P->Tokens = NULL;
  P->TokensSize = 0;
  P->TokenPos = 0;
  P->CurTok = NULL;
  if (P->Cmds)
    free(P->Cmds);
  P->Cmds = NULL;
  P->CmdsSize = 0;
}

bool _tshParseReadToken(TshParse *P) {
  if (P->TokenPos >= P->TokensSize) {
    P->CurTok = NULL;
    return false;
  }
  P->CurTok = &P->Tokens[++P->TokenPos];
  return true;
}

bool _tshParseConsumeToken(TshParse *P, TshTokenKind Kind) {
  if (P && P->CurTok->Kind == Kind) {
    _tshParseReadToken(P);
    return true;
  }
  return false;
}

void _tshParseAddCmd(TshParse *P) {
  ++P->CmdsSize;
  P->Cmds = realloc(P->Cmds, sizeof(TshCmd) * P->CmdsSize);
}
