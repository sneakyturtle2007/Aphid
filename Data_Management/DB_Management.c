#include "DB_Management.h"

Error Start_DB(sqlite3 **db){
  int status = sqlite3_open("Database.db", db);
  if(status != SQLITE_OK){
    return (Error) {"Failed to open Database.db.\n", DATABASE_ERROR};
  }
  return (Error) {"\n", GREAT_SUCCESS};
}



