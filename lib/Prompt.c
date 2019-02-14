#include "Prompt.h"

// Must be included before GNU ReadLine.
#include <stdio.h>

#include <readline/history.h>
#include <readline/readline.h>

#include <assert.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

#define TSH_HOSTNAME_SIZE 64
#define TSH_CWD_SIZE 64
#define TSH_PROMPT_SIZE 128

char *tshPrompt() {
  // Username.
  char *User = getenv("USER");
  if (!User)
    return NULL;

  // Hostname.
  struct utsname UtsName;
  if (uname(&UtsName) != 0)
    return NULL;

  // UName gives a qualified hostname (localhost.localdomain).
  char HostName[TSH_HOSTNAME_SIZE];
  strncpy(HostName, UtsName.nodename, TSH_HOSTNAME_SIZE);
  char *Delim;
  if ((Delim = strrchr(HostName, '.')))
    *Delim = '\0';

  // Current working dir.
  char Cwd[TSH_CWD_SIZE];
  if (!getcwd(Cwd, sizeof(Cwd)))
    return NULL;

  char *BaseName = basename(Cwd);
  assert(BaseName);

  // Format prompt.
  char Prompt[TSH_PROMPT_SIZE];
  snprintf(Prompt, sizeof(Prompt), "<%s@%s %s>$ ", User, HostName, BaseName);

  return readline(Prompt);
}
