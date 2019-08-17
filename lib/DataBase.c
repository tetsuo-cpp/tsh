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

static const char *SelectHistory = "SELECT cmd_name "
                                   "FROM tsh_stats "
                                   "ORDER by ROWID desc "
                                   "LIMIT %ld;";

static int tshDataBaseDurationCallback(void *, int, char **, char **);
static int tshDataBaseHistoryCallback(void *, int, char **, char **);
static bool tshDataBaseQueryDurations(TshDataBase *, const char *);
static void tshStatsDataInit(TshStatsData *);
static void tshStatsDataDestroy(TshStatsData *);

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

  TSH_DBG_PRINTF("tsh: inserting duration data. Query=\"%s\"\n", Query);

  if (sqlite3_exec(D->DB, Query, NULL, NULL, NULL) != 0)
    fprintf(stderr, "tsh: failed to insert duration data.\n");
}

bool tshDataBaseGetTopDurations(TshDataBase *D) {
  return tshDataBaseQueryDurations(D, SelectTopDurations);
}

bool tshDataBaseGetDuration(TshDataBase *D, const char *CmdName) {
  char Query[TSH_DB_QUERY_SIZE];
  snprintf(Query, sizeof(Query), SelectDuration, CmdName);
  return tshDataBaseQueryDurations(D, Query);
}

bool tshDataBaseGetHistory(TshDataBase *D, long int NumResults) {
  if (NumResults <= 0)
    return false;

  if (D->Clear) {
    for (KV_FOREACH(Index, D->Data))
      tshStatsDataDestroy(&kv_A(D->Data, Index));

    kv_destroy(D->Data);
  }

  D->Clear = true;
  kv_init(D->Data);

  char Query[TSH_DB_QUERY_SIZE];
  snprintf(Query, sizeof(Query), SelectHistory, NumResults);
  char *ErrorMsg;
  if (sqlite3_exec(D->DB, Query, tshDataBaseHistoryCallback, D, &ErrorMsg) !=
      0) {
    fprintf(stderr, "tsh: failed to get history. Msg=%s\n", ErrorMsg);
    sqlite3_free(ErrorMsg);
    return false;
  }

  return true;
}

void tshDataBaseDestroy(TshDataBase *D) {
  sqlite3_close(D->DB);
  if (D->Clear) {
    for (KV_FOREACH(Index, D->Data))
      tshStatsDataDestroy(&kv_A(D->Data, Index));

    kv_destroy(D->Data);
    D->Clear = false;
  }
}

static int tshDataBaseDurationCallback(void *CallbackArg, int ArgC, char **ArgV,
                                       char **ColumnNames) {
  TshDataBase *D = (TshDataBase *)CallbackArg;

  TshStatsData Stats;
  tshStatsDataInit(&Stats);

  if (ArgC != 2) {
    fprintf(stderr,
            "tsh: received wrong number of duration columns from db. "
            "NumColumns=%d\n",
            ArgC);
    return -1;
  }

  for (int Index = 0; Index < ArgC; ++Index) {
    if (strcmp("cmd_name", ColumnNames[Index]) == 0) {
      if (Stats.CmdName) {
        fprintf(stderr, "tsh: received duplicate \"cmd_name\" field.");
        tshStatsDataDestroy(&Stats);
        return -1;
      }

      Stats.CmdName = malloc(sizeof(char) * (strlen(ArgV[Index]) + 1));
      strcpy(Stats.CmdName, ArgV[Index]);
    } else if (strcmp("duration", ColumnNames[Index]) == 0) {
      if (Stats.Duration) {
        fprintf(stderr, "tsh: received duplicate \"duration\" field.");
        tshStatsDataDestroy(&Stats);
        return -1;
      }

      Stats.Duration = malloc(sizeof(char) * (strlen(ArgV[Index]) + 1));
      strcpy(Stats.Duration, ArgV[Index]);
    } else {
      fprintf(stderr,
              "tsh: received unrecognised fieldname from db. Field=%s\n",
              ColumnNames[Index]);

      tshStatsDataDestroy(&Stats);
      return -1;
    }
  }

  kv_push(TshStatsData, D->Data, Stats);
  return 0;
}

static int tshDataBaseHistoryCallback(void *CallbackArg, int ArgC, char **ArgV,
                                      char **ColumnNames) {
  TshDataBase *D = (TshDataBase *)CallbackArg;

  TshStatsData Stats;
  tshStatsDataInit(&Stats);

  if (ArgC != 1) {
    fprintf(stderr,
            "tsh: received wrong number of history columns from db. "
            "NumColumns=%d\n",
            ArgC);
    return -1;
  }

  if (strcmp("cmd_name", ColumnNames[0]) != 0) {
    fprintf(stderr, "tsh: received unrecognised fieldname from db. Field=%s\n",
            ColumnNames[0]);
    tshStatsDataDestroy(&Stats);
    return -1;
  }

  Stats.CmdName = malloc(sizeof(char) * (strlen(ArgV[0]) + 1));
  strcpy(Stats.CmdName, ArgV[0]);

  kv_push(TshStatsData, D->Data, Stats);
  return 0;
}

static bool tshDataBaseQueryDurations(TshDataBase *D, const char *Query) {
  if (D->Clear) {
    for (KV_FOREACH(Index, D->Data))
      tshStatsDataDestroy(&kv_A(D->Data, Index));

    kv_destroy(D->Data);
  }

  D->Clear = true;
  kv_init(D->Data);

  char *ErrorMsg;
  if (sqlite3_exec(D->DB, Query, tshDataBaseDurationCallback, D, &ErrorMsg) !=
      0) {
    fprintf(stderr, "tsh: failed to get durations. Msg=%s\n", ErrorMsg);
    sqlite3_free(ErrorMsg);
    return false;
  }

  return true;
}

static void tshStatsDataInit(TshStatsData *S) {
  S->CmdName = NULL;
  S->Duration = NULL;
}

static void tshStatsDataDestroy(TshStatsData *S) {
  if (S->CmdName) {
    free(S->CmdName);
    S->CmdName = NULL;
  }

  if (S->Duration) {
    free(S->Duration);
    S->Duration = NULL;
  }
}
