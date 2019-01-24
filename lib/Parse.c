#include "Parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TSH_TOK_BUFSIZE 64
#define TSH_TOK_DELIM " \t\r\n\a"

char **tshSplitLine(char *Line) {
  unsigned int BufSize = TSH_TOK_BUFSIZE, Position = 0;
  char **Tokens = malloc(sizeof(char *) * BufSize);
  char *Token;

  if (!Tokens) {
    fprintf(stderr, "tsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  Token = strtok(Line, TSH_TOK_DELIM);
  while (Token) {
    Tokens[Position] = Token;
    ++Position;

    if (Position >= BufSize) {
      BufSize += TSH_TOK_BUFSIZE;
      Tokens = realloc(Tokens, sizeof(char *) * BufSize);
      if (!Tokens) {
        fprintf(stderr, "tsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    Token = strtok(NULL, TSH_TOK_DELIM);
  }

  Tokens[Position] = NULL;
  return Tokens;
}
