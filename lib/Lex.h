#pragma once

#include <stdbool.h>

typedef struct {
  char *Buf;
  unsigned int BufSize;
  unsigned int CurPos;
  char CurChar;
} TshLex;

typedef enum {
  IDENTIFIER,
  PIPE,
  REDIR,
  QUOTE,
  DQUOTE,
  END_OF_FILE
} TshLexTokenKind;

typedef struct {
  TshLexTokenKind Kind;
  char *Buf;
  unsigned int BufSize;
} TshLexToken;

void tshLexInit(TshLex *L, char *Buf, unsigned int BufSize);
void tshLexClose(TshLex *L);
void tshLexGetToken(TshLex *L, TshLexToken *T);
