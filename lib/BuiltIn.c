#include "BuiltIn.h"

#include <stdio.h>
#include <unistd.h>

char *BuiltInStr[] = {"cd", "help", "exit"};

int (*BuiltInFunc[])(char **) = {&tshCd, &tshHelp, &tshExit};

int tshNumBuiltIns() { return sizeof(BuiltInStr) / sizeof(char *); }

int tshCd(char **Args) {
  if (!Args[1]) {
    fprintf(stderr, "tsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(Args[1]) != 0) {
      perror("tsh");
    }
  }

  return 1;
}

int tshHelp(char **Args) {
  (void)Args;
  int I;
  printf("Tetsuo's TSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (I = 0; I < tshNumBuiltIns(); ++I) {
    printf("  %s\n", BuiltInStr[I]);
  }

  printf("Use the man command for information on other programs.\n");

  return 1;
}

int tshExit(char **Args) {
  (void)Args;
  return 0;
}
