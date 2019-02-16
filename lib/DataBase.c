#include "DataBase.h"

#include <stdio.h>

#define TSH_DB_QUERY_SIZE 256

bool tshDataBaseInit(TshDataBase *D, const char *Name) {
  if (sqlite3_open(Name, &D->DB) != 0) {
    fprintf(stderr, "tsh: failed to open database. Msg=%s",
            sqlite3_errmsg(D->DB));
    return false;
  }

  const char *CreateTable = "CREATE TABLE IF NOT EXISTS tsh_stats("
                            "cmd_name TEXT, "
                            "duration NUMERIC"
                            ");";

  if (sqlite3_exec(D->DB, CreateTable, NULL, NULL, NULL) != 0)
    return false;

  return true;
}

void tshDataBaseRecordDuration(TshDataBase *D, const char *CmdName,
                               float Duration) {
  // Put together SQL string.
  char Query[TSH_DB_QUERY_SIZE];
  const char *InsertDuration = "INSERT INTO tsh_stats(cmd_name, duration) "
                               "VALUES(\"%s\", %f);";
  snprintf(Query, sizeof(Query), InsertDuration, CmdName, Duration);

#ifndef NDDEBUG
  printf("tsh: inserting duration data. Query=\"%s\"\n", Query);
#endif

  if (sqlite3_exec(D->DB, Query, NULL, NULL, NULL) != 0)
    fprintf(stderr, "tsh: failed to insert duration data.\n");
}

void tshDataBaseClose(TshDataBase *D) { sqlite3_close(D->DB); }
