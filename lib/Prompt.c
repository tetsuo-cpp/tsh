#include "Prompt.h"

// Must be included before GNU ReadLine.
#include <stdio.h>

#include <readline/history.h>
#include <readline/readline.h>

#include <libgen.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <unistd.h>

#define TSH_HOSTNAME_SIZE 64
#define TSH_CWD_SIZE 64
#define TSH_PROMPT_SIZE 128

char *tshPrompt() {
  // Username.
  char *User = getenv("USER");

  // Hostname.
  struct utsname UtsName;
  uname(&UtsName);

  // UName gives a qualified hostname (localhost.localdomain).
  char HostName[TSH_HOSTNAME_SIZE];
  strncpy(HostName, UtsName.nodename, TSH_HOSTNAME_SIZE);
  char *Delim;
  if ((Delim = strrchr(HostName, '.')))
    *Delim = '\0';

  // Current working dir.
  char Cwd[TSH_CWD_SIZE];
  getcwd(Cwd, sizeof(Cwd));
  char *BaseName = basename(Cwd);

  // Format prompt.
  char Prompt[TSH_PROMPT_SIZE];
  snprintf(Prompt, sizeof(Prompt), "[%s@%s %s]$ ", User, HostName, BaseName);

  return readline(Prompt);
}
