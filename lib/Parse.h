#pragma once

#include <Cmd.h>
#include <Lex.h>

#include <stdbool.h>

typedef struct {
  const TshToken *Tokens;
  unsigned int TokensSize;
  unsigned int TokenPos;
  const TshToken *CurTok;
  TshCmd *Cmds;
  unsigned int CmdsSize;
} TshParse;

void tshParseInit(TshParse *, const TshToken *, unsigned int);
bool tshParseCmd(TshParse *);
void tshParseClose(TshParse *);
