#include "Parse.h"

#include <stdlib.h>

void tshParseInit(TshParse *P, const TshToken *Tokens,
                  unsigned int TokensSize) {
  P->Tokens = Tokens;
  P->TokensSize = TokensSize;
  P->Commands = NULL;
  P->CommandsSize = 0;
}

bool tshParseExec(TshParse *P) {
  (void)P;
  return false;
}

void tshParseClose(TshParse *P) {
  P->Tokens = NULL;
  P->TokensSize = 0;
  if (P->Commands)
    free(P->Commands);
  P->Commands = NULL;
  P->CommandsSize = 0;
}
