#include "Lex.h"

#include <ctype.h>
#include <stdlib.h>

bool _tshLexGetChar(TshLex *L);

void tshLexInit(TshLex *L, char *Buf, unsigned int BufSize) {
  L->Buf = Buf;
  L->BufSize = BufSize;
  L->CurPos = 0;
  L->CurChar = 0;
  _tshLexGetChar(L);
}

void tshLexClose(TshLex *L) {
  free(L->Buf);
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

void _tshLexGetIdentifier(TshLex *L, TshLexToken *T) {
  T->Kind = IDENTIFIER;
  T->Buf = &L->Buf[L->CurPos - 1];
  T->BufSize = 0;
  while (!_tshLexIsDelim(L->CurChar) && _tshLexGetChar(L)) {
    ++T->BufSize;
  }
}

void tshLexGetToken(TshLex *L, TshLexToken *T) {
  T->Buf = NULL;
  while (isspace(L->CurChar) && _tshLexGetChar(L)) {
  }
  if (!L->Buf || L->CurPos >= L->BufSize) {
    T->Kind = END_OF_FILE;
    return;
  }
  switch (L->CurChar) {
  case '|':
    T->Kind = PIPE;
    break;
  case '>':
    T->Kind = REDIR;
    break;
  case '\'':
    T->Kind = QUOTE;
    break;
  case '\"':
    T->Kind = DQUOTE;
    break;
  default:
    _tshLexGetIdentifier(L, T);
    return;
  }
  _tshLexGetChar(L);
}
