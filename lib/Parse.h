#pragma once

#include <Cmd.h>
#include <Lex.h>

#include <klib/kvec.h>

#include <stdbool.h>

struct TshCmd;

typedef kvec_t(TshToken) TshTokenVec;

typedef struct {
  TshTokenVec Tokens;
  unsigned int TokenPos;
  const TshToken *CurTok;
} TshParse;

void tshParseInit(TshParse *, TshTokenVec);
TshCmd *tshParseCmd(TshParse *, TshCmd *);
void tshParseClose(TshParse *);
