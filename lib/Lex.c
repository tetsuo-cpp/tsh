#include "Lex.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

bool _tshLexGetChar(TshLex *L);
bool _tshLexIsDelim(char C);
void _tshLexGetIdentifier(TshLex *L, TshToken *T);

void tshLexInit(TshLex *L, const char *Buf, unsigned int BufSize) {
  L->Buf = Buf;
  L->BufSize = BufSize;
  L->CurPos = 0;
  L->CurChar = 0;
  _tshLexGetChar(L);
}

void tshLexGetToken(TshLex *L, TshToken *T) {
  T->Buf = NULL;
  while (isspace(L->CurChar) && _tshLexGetChar(L)) {
  }
  if (!L->Buf || L->CurPos >= L->BufSize) {
    T->Kind = TK_EndOfFile;
    return;
  }
  switch (L->CurChar) {
  case '|':
    T->Kind = TK_Pipe;
    break;
  case '>':
    T->Kind = TK_Redir;
    break;
  case '\'':
    T->Kind = TK_Quote;
    break;
  case '\"':
    T->Kind = TK_DQuote;
    break;
  default:
    _tshLexGetIdentifier(L, T);
    return;
  }
  _tshLexGetChar(L);
}

void tshLexClose(TshLex *L) {
  L->Buf = NULL;
  L->BufSize = 0;
  L->CurPos = 0;
  L->CurChar = 0;
}

bool _tshLexGetChar(TshLex *L) {
  if (!L->Buf || L->CurPos >= L->BufSize)
    return false;
  L->CurChar = L->Buf[L->CurPos];
  ++L->CurPos;
  return true;
}

bool _tshLexIsDelim(char C) {
  if (isspace(C))
    return true;
  if (C == '|' || C == '>' || C == '\'' || C == '\"')
    return true;
  return false;
}

void _tshLexGetIdentifier(TshLex *L, TshToken *T) {
  T->Kind = TK_Identifier;
  T->Buf = &L->Buf[L->CurPos - 1];
  T->BufSize = 1;
  while (!_tshLexIsDelim(L->CurChar) && _tshLexGetChar(L)) {
    ++T->BufSize;
  }
}
