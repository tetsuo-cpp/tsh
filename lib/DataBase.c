#include "DataBase.h"

#include <Engine.h>
#include <Util.h>

#include <stdio.h>
#include <string.h>

#define TSH_DB_QUERY_SIZE 256

static const char *CreateTable = "CREATE TABLE IF NOT EXISTS tsh_stats("
                                 "cmd_name TEXT, "
                                 "duration NUMERIC"
                                 ");";

static const char *InsertDuration = "INSERT INTO tsh_stats(cmd_name, duration) "
                                    "VALUES(\"%s\", %f);";

static const char *SelectTopDurations =
    "SELECT cmd_name, avg(duration) as duration "
    "FROM tsh_stats "
    "GROUP BY cmd_name "
    "ORDER BY duration DESC "
    "LIMIT 10;";

static const char *SelectDuration =
    "SELECT cmd_name, avg(duration) as duration "
    "FROM tsh_stats "
    "WHERE cmd_name = '%s' "
    "GROUP BY cmd_name;";

static int _tshDataBaseSQLiteCallback(void *, int, char **, char **);
static bool _tshDataBaseQueryDurations(TshDataBase *, const char *);
static void _tshStatsDataInit(TshStatsData *);
static void _tshStatsDataDestroy(TshStatsData *);

bool tshDataBaseInit(TshDataBase *D, const char *Name) {
  if (sqlite3_open(Name, &D->DB) != 0) {
    fprintf(stderr, "tsh: failed to open database. Msg=%s\n",
            sqlite3_errmsg(D->DB));
    return false;
  }

  if (sqlite3_exec(D->DB, CreateTable, NULL, NULL, NULL) != 0)
    return false;

  D->Clear = false;
  return true;
}

void tshDataBaseRecordDuration(TshDataBase *D, const char *CmdName,
                               double Duration) {
  // Put together SQL string.
  char Query[TSH_DB_QUERY_SIZE];
  snprintf(Query, sizeof(Query), InsertDuration, CmdName, Duration);

#ifndef NDDEBUG
  printf("tsh: inserting duration data. Query=\"%s\"\n", Query);
#endif

  if (sqlite3_exec(D->DB, Query, NULL, NULL, NULL) != 0)
    fprintf(stderr, "tsh: failed to insert duration data.\n");
}

bool tshDataBaseGetTopDurations(TshDataBase *D) {
  return _tshDataBaseQueryDurations(D, SelectTopDurations);
}

bool tshDataBaseGetDuration(TshDataBase *D, const char *CmdName) {
  char Query[TSH_DB_QUERY_SIZE];
  snprintf(Query, sizeof(Query), SelectDuration, CmdName);
  return _tshDataBaseQueryDurations(D, Query);
}

void tshDataBaseDestroy(TshDataBase *D) {
  sqlite3_close(D->DB);
  if (D->Clear) {
    for (KV_FOREACH(Index, D->Data))
      _tshStatsDataDestroy(&kv_A(D->Data, Index));

    kv_destroy(D->Data);
    D->Clear = false;
  }
}

static int _tshDataBaseSQLiteCallback(void *CallbackArg, int ArgC, char **ArgV,
                                      char **ColumnNames) {
  TshDataBase *D = (TshDataBase *)CallbackArg;

  TshStatsData Stats;
  _tshStatsDataInit(&Stats);

  if (ArgC != 2) {
    fprintf(stderr,
            "tsh: received wrong number of columns from db. NumColumns=%d\n",
            ArgC);
    return -1;
  }

  for (int Index = 0; Index < ArgC; ++Index) {
    if (strcmp("cmd_name", ColumnNames[Index]) == 0) {
      if (Stats.CmdName) {
        fprintf(stderr, "tsh: received duplicate field \"cmd_name\" field.");
        _tshStatsDataDestroy(&Stats);
        return -1;
      }

      Stats.CmdName = malloc(sizeof(char) * (strlen(ArgV[Index]) + 1));
      strcpy(Stats.CmdName, ArgV[Index]);
    } else if (strcmp("duration", ColumnNames[Index]) == 0) {
      if (Stats.Duration) {
        fprintf(stderr, "tsh: received duplicate field \"duration\" field.");
        _tshStatsDataDestroy(&Stats);
        return -1;
      }

      Stats.Duration = malloc(sizeof(char) * (strlen(ArgV[Index]) + 1));
      strcpy(Stats.Duration, ArgV[Index]);
    } else {
      fprintf(stderr,
              "tsh: received unrecognised fieldname from db. Field=%s\n",
              ColumnNames[Index]);

      _tshStatsDataDestroy(&Stats);
      return -1;
    }
  }

  kv_push(TshStatsData, D->Data, Stats);
  return 0;
}

static bool _tshDataBaseQueryDurations(TshDataBase *D, const char *Query) {
  if (D->Clear) {
    for (KV_FOREACH(Index, D->Data))
      _tshStatsDataDestroy(&kv_A(D->Data, Index));

    kv_destroy(D->Data);
  }

  D->Clear = true;
  kv_init(D->Data);

  char *ErrorMsg;
  if (sqlite3_exec(D->DB, Query, _tshDataBaseSQLiteCallback, D, &ErrorMsg) !=
      0) {
    fprintf(stderr, "tsh: failed to get durations. Msg=%s\n", ErrorMsg);
    sqlite3_free(ErrorMsg);
    return false;
  }

  return true;
}

static void _tshStatsDataInit(TshStatsData *S) {
  S->CmdName = NULL;
  S->Duration = NULL;
}

static void _tshStatsDataDestroy(TshStatsData *S) {
  if (S->CmdName) {
    free(S->CmdName);
    S->CmdName = NULL;
  }

  if (S->Duration) {
    free(S->Duration);
    S->Duration = NULL;
  }
}
