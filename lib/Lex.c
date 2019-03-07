#include "Lex.h"

#include <ctype.h>
#include <stdbool.h>

static bool _tshLexGetChar(TshLex *);
static bool _tshLexIsDelim(char);
static void _tshLexGetIdentifier(TshLex *, TshToken *);
static void _tshLexGetIdentifierUntilQuote(TshLex *, TshToken *, char);

void tshLexInit(TshLex *L, const char *Buf, size_t BufSize) {
  L->Buf = Buf;
  L->BufSize = BufSize;
  L->CurPos = 0;
  L->CurChar = 0;

  _tshLexGetChar(L);
}

void tshLexGetToken(TshLex *L, TshToken *T) {
  T->Buf = NULL;
  T->BufSize = 0;
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
  case '<':
    T->Kind = TK_ReverseRedir;
    break;
  case '\'':
  case '\"':
    _tshLexGetIdentifierUntilQuote(L, T, L->CurChar);
    break;
  default:
    _tshLexGetIdentifier(L, T);
    return;
  }

  _tshLexGetChar(L);
}

void tshLexDestroy(TshLex *L) {
  L->Buf = NULL;
  L->BufSize = 0;
  L->CurPos = 0;
  L->CurChar = 0;
}

const char *tshTokenKindToString(TshTokenKind Kind) {
  switch (Kind) {
  case TK_Identifier:
    return "Identifier";
  case TK_Pipe:
    return "Pipe";
  case TK_Redir:
    return "Redir";
  case TK_ReverseRedir:
    return "ReverseRedir";
  case TK_EndOfFile:
    return "EndOfFile";
  case TK_None:
    return "None";
  }

  return "Unknown";
}

static bool _tshLexGetChar(TshLex *L) {
  if (!L->Buf || L->CurPos >= L->BufSize)
    return false;

  L->CurChar = L->Buf[L->CurPos];
  ++L->CurPos;
  return true;
}

static bool _tshLexIsDelim(char C) {
  if (isspace(C))
    return true;

  if (C == '|' || C == '>' || C == '\'' || C == '\"')
    return true;

  return false;
}

static void _tshLexGetIdentifier(TshLex *L, TshToken *T) {
  T->Kind = TK_Identifier;
  T->Buf = &L->Buf[L->CurPos - 1];
  T->BufSize = 1;

  while (_tshLexGetChar(L) && !_tshLexIsDelim(L->CurChar))
    ++T->BufSize;
}

static void _tshLexGetIdentifierUntilQuote(TshLex *L, TshToken *T, char QChar) {
  T->Kind = TK_Identifier;
  T->Buf = &L->Buf[L->CurPos];
  T->BufSize = 0;

  while (_tshLexGetChar(L) && L->CurChar != QChar)
    ++T->BufSize;
}
