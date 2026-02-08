#include "DB_Management.h"

// Local Utility Function Declarations
static int Process_File_Alias(void *data, int num_cols, char **col_values, char **col_names);
static int Process_Storage_Blob(void *data, int num_cols, char **col_values, char **col_names);

Error Start_DB(sqlite3 **db){
  int status = sqlite3_open("Database.db", db);
  if(status != SQLITE_OK){
    return (Error) {"Failed to open Database.db.\n", DATABASE_ERROR};
  }
  return (Error) {"\n", GREAT_SUCCESS};
}
Error Insert_Storage_Blob(sqlite3 **db, char* username, char *data){ 
  sqlite3_stmt *stmt;
  char sql[] = "INSERT INTO Shared_Storage(DeviceID, Data) (?, ?)";
  int status = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, username, strlen(username), SQLITE_STATIC);
  sqlite3_bind_blob(stmt, 2, data, strlen(data), SQLITE_STATIC);
  status = sqlite3_step(stmt);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %d\n", status);
    return (Error) {"Failed to insert data blob into shared storage.\n", DATABASE_ERROR};
  }
  sqlite3_finalize(stmt);
  return (Error) {"", GREAT_SUCCESS};
}

Error Get_Storage_Blob(sqlite3 **db, DataBlob* blob, char* username){
  sqlite3_stmt *stmt;
  char sql[64] ;
  int status = snprintf(sql, 64, "SELECT * Shared_Storage WHERE Username = '%s'", username);
  char* errmsg;
  status = sqlite3_exec(*db, sql, Process_Storage_Blob, blob, &errmsg);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %d\n", status);
    return (Error) {"Failed to get storage blob.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}

Error Insert_File_Aliases(sqlite3 **db, char* file_name, int file_size, char* aphid_path){
  sqlite3_stmt *stmt;
  char sql[] = "INSERT INTO File_Aliases(FileName, FileSize, AphidPath) (?, ?)";
  int status = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, file_name, strlen(file_name), SQLITE_STATIC);
  sqlite3_bind_int(stmt, 2, file_size);
  sqlite3_bind_text(stmt, 3, aphid_path, strlen(aphid_path), SQLITE_STATIC);
  status = sqlite3_step(stmt);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %d\n", status);
    return (Error) {"Failed to insert file alias into alias storage.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}

Error Get_File_Alias(sqlite3 **db, AliasList* aliases){
  sqlite3_stmt *stmt;
  char sql[64];
  int status = snprintf(sql, 64, "SELECT * File_Aliases");  

  char* errmsg;
  status = sqlite3_exec(*db, sql, Process_File_Alias, aliases, &errmsg);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %d\n", status);
    return (Error) {"Failed to retrieve file alias.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}

static int Process_File_Alias(void *data, int num_cols, char **col_values, char **col_names){
  AliasList* aliases = (AliasList*) data;

  if(aliases->count <= aliases->capacity){
    aliases->capacity *= 2;
    aliases->aliases = realloc(aliases->aliases, aliases->capacity);
    if(aliases == NULL){
      return 1;
    }
  }
  aliases->aliases[aliases->count +1] = (Alias) {col_values[0], strtol(col_values[1],NULL, 10), col_values[2]};
  aliases->count ++;  
  return 0;
}

static int Process_Storage_Blob(void *data, int num_cols, char **col_values, char **col_names){
  DataBlob* blob = (DataBlob*) data;

  blob->username = col_values[0];
  blob->blob = col_values[1];
  return 0;
}