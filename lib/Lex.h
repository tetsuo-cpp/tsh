#pragma once

#include <stdlib.h>

typedef struct {
  const char *Buf;
  size_t BufSize;
  size_t CurPos;
  char CurChar;
} TshLex;

typedef enum {
  TK_Identifier,
  TK_Pipe,
  TK_Redir,
  TK_ReverseRedir,
  TK_EndOfFile,
  TK_None
} TshTokenKind;

typedef struct {
  TshTokenKind Kind;
  const char *Buf;
  size_t BufSize;
} TshToken;

void tshLexInit(TshLex *, const char *, size_t);
void tshLexGetToken(TshLex *, TshToken *);
void tshLexClose(TshLex *);
const char *tshTokenKindToString(TshTokenKind);
