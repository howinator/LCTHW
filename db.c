#include <unistd.h>
#include <apr_errno.h>
#include <apr_file_io.h>

#include "db.h"
#include "bstrlib.h"
#include "dbg.h"

static FILE *DB_open(const char *path, const char *mode)
{
  return fopen(path, mode);
}


static void DB_close(FILE *db)
{
  fclose(db);
}

static bstring DB_load()
{
  FILE *db = NULL;
  bstring data = NULL;

  db = DB_open(DB_FILE, "r");
  check(db, "Failed to open database: %s", DB_FILE);

  data = bread((bNread)fread, db);
  check(data, "Failed to read from db file: %s", DB_FILE);

  DB_close(db);
  return data;

error:
  if(db) DB_close(db);
  if(data) bdestroy(data);
  return NULL;
}

int DB_update(const char *url)
{

