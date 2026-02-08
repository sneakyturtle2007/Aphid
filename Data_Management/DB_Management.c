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

Error Insert_Storage_Blob(sqlite3 **db, DataBlob *blob){ 
  char sql[64 + blob->size + strlen(blob->username)];
  int status = snprintf(sql, sizeof(sql), "INSERT INTO SharedStorage VALUES ('%s', '%s', %d);", blob->username, blob->blob, blob->size);
  char* errmsg;
  printf("%s\n", blob->blob);
  status = sqlite3_exec(*db, sql, NULL, NULL, &errmsg);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %s\n", errmsg);
    return (Error) {"Failed to insert data blob into shared storage.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}

Error Get_Storage_Blob(sqlite3 **db, DataBlob* blob, char* username){
  char sql[64+strlen(username)];
  int status = snprintf(sql, 64+strlen(username), "SELECT * FROM SharedStorage WHERE Username = '%s' LIMIT 1;", username);

  char* errmsg;
  status = sqlite3_exec(*db, sql, Process_Storage_Blob, blob, &errmsg);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %s\n", errmsg);
    return (Error) {"Failed to get storage blob.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}

Error Insert_File_Aliases(sqlite3 **db, char* file_name, int file_size, char* aphid_path){
  sqlite3_stmt *stmt;
  char sql[] = "INSERT INTO FileAliases(FileName, FileSize, AphidPath) VALUES (?, ?);";
  int status = sqlite3_prepare_v2(*db, sql, -1, &stmt, NULL);
  sqlite3_bind_text(stmt, 1, file_name, strlen(file_name), SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, file_size);
  sqlite3_bind_text(stmt, 3, aphid_path, strlen(aphid_path), SQLITE_TRANSIENT);
  status = sqlite3_step(stmt);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %d\n", status);
    return (Error) {"Failed to insert file alias into alias storage.\n", DATABASE_ERROR};
  }
  sqlite3_finalize(stmt);
  return (Error) {"", GREAT_SUCCESS};
}

Error Get_File_Alias(sqlite3 **db, AliasList* aliases){
  char sql[64];
  int status = snprintf(sql, 64, "SELECT * File_Aliases;");  

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
  
  uint32_t username_len = strlen(col_values[0]) +1;

  blob->username = calloc(username_len, sizeof(char));
  snprintf(blob->username, username_len, "%s", col_values[0]);

  blob->size = strtol(col_values[2], NULL, 10);

  uint32_t blob_len = strlen(col_values[1]); 

  blob->blob = calloc( blob_len +1, sizeof(unsigned char));
  blob->size = blob_len+1;
  
  blob->blob = memcpy(blob->blob, col_values[1], blob->size);

  return 0;
}
