#include "Lex.h"

#include <ctype.h>
#include <stdbool.h>

static bool tshLexGetChar(TshLex *);
static bool tshLexIsDelim(char);
static void tshLexGetIdentifier(TshLex *, TshToken *);
static void tshLexGetIdentifierUntilQuote(TshLex *, TshToken *, char);

void tshLexInit(TshLex *L, const char *Buf, size_t BufSize) {
  L->Buf = Buf;
  L->BufSize = BufSize;
  L->CurPos = 0;
  L->CurChar = 0;

  tshLexGetChar(L);
}

void tshLexGetToken(TshLex *L, TshToken *T) {
  T->Buf = NULL;
  T->BufSize = 0;
  if (!L->Buf || L->CurPos >= L->BufSize) {
    T->Kind = TK_EndOfFile;
    return;
  }

  while (isspace(L->CurChar) && tshLexGetChar(L))
    ;

  if (isspace(L->CurChar)) {
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
    tshLexGetIdentifierUntilQuote(L, T, L->CurChar);
    break;
  default:
    tshLexGetIdentifier(L, T);
    return;
  }

  tshLexGetChar(L);
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

static bool tshLexGetChar(TshLex *L) {
  if (!L->Buf || L->CurPos >= L->BufSize)
    return false;

  L->CurChar = L->Buf[L->CurPos];
  ++L->CurPos;
  return true;
}

static bool tshLexIsDelim(char C) {
  if (isspace(C))
    return true;

  if (C == '|' || C == '>' || C == '\'' || C == '\"')
    return true;

  return false;
}

static void tshLexGetIdentifier(TshLex *L, TshToken *T) {
  T->Kind = TK_Identifier;
  T->Buf = &L->Buf[L->CurPos - 1];
  T->BufSize = 1;

  while (tshLexGetChar(L) && !tshLexIsDelim(L->CurChar))
    ++T->BufSize;
}

static void tshLexGetIdentifierUntilQuote(TshLex *L, TshToken *T, char QChar) {
  T->Kind = TK_Identifier;
  T->Buf = &L->Buf[L->CurPos];
  T->BufSize = 0;

  while (tshLexGetChar(L) && L->CurChar != QChar)
    ++T->BufSize;
}
