#pragma once

typedef struct {
  const char *Buf;
  unsigned int BufSize;
  unsigned int CurPos;
  char CurChar;
} TshLex;

typedef enum {
  TK_Identifier,
  TK_Pipe,
  TK_Redir,
  TK_Quote,
  TK_DQuote,
  TK_EndOfFile
} TshTokenKind;

typedef struct {
  TshTokenKind Kind;
  const char *Buf;
  unsigned int BufSize;
} TshToken;

void tshLexInit(TshLex *, const char *, unsigned int);
void tshLexGetToken(TshLex *, TshToken *);
void tshLexClose(TshLex *);
