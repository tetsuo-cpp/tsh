#pragma once

extern char *BuiltInStr[];
extern int (*BuiltInFunc[])(char **);

int tshNumBuiltIns();
int tshCd(char **Args);
int tshHelp(char **Args);
int tshExit(char **Args);
