#pragma once

#include <Command.h>
#include <Lex.h>

#include <stdbool.h>

typedef struct {
  const TshToken *Tokens;
  unsigned int TokensSize;
  TshToken CurTok;
  TshCommand *Commands;
  unsigned int CommandsSize;
} TshParse;

void tshParseInit(TshParse *P, const TshToken *Tokens, unsigned int TokensSize);
bool tshParseExec(TshParse *P);
void tshParseClose(TshParse *P);
