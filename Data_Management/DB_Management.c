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
  int status = snprintf(sql, sizeof(sql), "INSERT INTO SharedStorage VALUES ('%s', '%s', %d, %d);", blob->username, blob->blob, blob->size, blob->host_file_index);
  char* errmsg;
  printf("%s\n", blob->blob);
  status = sqlite3_exec(*db, sql, NULL, NULL, &errmsg);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %s\n", errmsg);
    sqlite3_free(errmsg);
    return (Error) {"Failed to insert data blob into shared storage.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}
Error Remove_Storage_Blob(sqlite3 **db, char *username){
  char sql[64 + strlen(username)];
  snprintf(sql, sizeof(sql), "DELETE FROM SharedStorage WHERE Username = '%s';", username);
  char * errmsg;
  int status = sqlite3_exec(*db, sql, NULL, NULL, &errmsg);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %s\n", errmsg);
    sqlite3_free(errmsg);
    return (Error) {"Failed to remove storage blob.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}

Error Get_Storage_Blob(sqlite3 **db, DataBlob* blob, char* username){
  char sql[64+strlen(username)];
  int status = snprintf(sql, sizeof(sql), "SELECT * FROM SharedStorage WHERE Username = '%s' LIMIT 1;", username);

  char* errmsg;
  status = sqlite3_exec(*db, sql, Process_Storage_Blob, blob, &errmsg);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %s\n", errmsg);
    sqlite3_free(errmsg);
    return (Error) {"Failed to get storage blob.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}

Error Insert_File_Alias(sqlite3 **db, char* username, char* file_name, int file_size, char* aphid_path){
  int sql_size = 64 + strlen(username) + strlen(file_name) + 32 + strlen(aphid_path);
  char sql[sql_size];
  snprintf(sql, sql_size, "INSERT INTO FileAliases(Username, FileName, FileSize, AphidPath) VALUES ('%s', '%s', %d, '%s');", username, file_name, file_size, aphid_path);
  char *err_msg;
  int status = sqlite3_exec(*db, sql, NULL, NULL, &err_msg);
  
  if(status != SQLITE_OK){
    printf("ERROR CODE: %s\n", err_msg);
    sqlite3_free(err_msg);
    return (Error) {"Failed to insert file alias into alias storage.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}

Error Remove_File_Alias(sqlite3 **db, char *username, char* file_name){
  char sql[64 + strlen(username) +strlen(file_name)];
  snprintf(sql, sizeof(sql), "DELETE FROM FileAliases WHERE Username = '%s' AND FileName = '%s'", username, file_name);
  char *errmsg;
  int status = sqlite3_exec(*db, sql, NULL, NULL, &errmsg);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %s\n", errmsg);
    sqlite3_free(errmsg);
    return (Error) {"Failed to remove file alias.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}

Error Get_File_Aliases(sqlite3 **db, char *username, AliasList* aliases){
  char sql[64 + strlen(username)];
  int status;
  if(strcmp(username, "all")){
    status= snprintf(sql, sizeof(sql), "SELECT * FROM FileAliases;");  
  }else{
    status = snprintf(sql, sizeof(sql), "SELECT * FROM FileAliases WHERE Username = '%s';", username);
  }
  char* errmsg;
  status = sqlite3_exec(*db, sql, Process_File_Alias, aliases, &errmsg);
  if(status != SQLITE_OK){
    printf("ERROR CODE: %s\n", errmsg);
    sqlite3_free(errmsg);
    return (Error) {"Failed to retrieve file alias.\n", DATABASE_ERROR};
  }
  return (Error) {"", GREAT_SUCCESS};
}

int Process_File_Alias(void *data, int num_cols, char **col_values, char **col_names){
  AliasList* aliases = (AliasList*) data;
  if(aliases->aliases == NULL){
    printf("NULL AliasList.\n");
    return 1;
  }
  int curr_count = 0;
  if(aliases->count > 0 ){
    curr_count = aliases->count + 1;
  }
  if(curr_count > aliases->capacity){
    if(aliases->capacity == 0){
      aliases->capacity = 2;
    }
    aliases->capacity *= 2;
    aliases->aliases = realloc(aliases->aliases, (size_t) aliases->capacity * sizeof(Alias));
    if(aliases == NULL){
      printf("Failed to reallocate memory for aliases list\n");
      return 1;
    }
  }
  printf("username: %s \nfile name: %s\nfile size: %ld\naphid path: %s\n", col_values[0], col_values[1], strtol(col_values[2], NULL, 10), col_values[3]);
  // +1 to the length of each variable to account for null termination.
  int username_len = strlen(col_values[0]) +1;
  int file_name_len = strlen(col_values[1]) +1;
  int aphid_path_len = strlen(col_values[3]) +1;
  
  aliases->aliases[curr_count] = (Alias){ .username = calloc(username_len, sizeof(char)), .name = calloc(file_name_len, sizeof(char)), .file_size = strtol(col_values[2], NULL, 10), .aphid_path = calloc(aphid_path_len, sizeof(char))};
  int status = snprintf(aliases->aliases[curr_count].username, username_len, "%s", col_values[0]);
  // -1 to account for null termination
  if(status != username_len -1){
    printf("Failed to copy file alias identifying username from database to copy of file alias.\n Copied string & status number: %s, %d\n", aliases->aliases[curr_count].username, status);
    return 1;
  }
  status = snprintf(aliases->aliases[curr_count].name, file_name_len, "%s", col_values[1]);
  if(status != file_name_len -1){
    printf("Failed to copy file alias name from database to copy of file alias.\n");
    return 1;
  }
  status = snprintf(aliases->aliases[curr_count].aphid_path, aphid_path_len, "%s", col_values[3]);
  if(status != aphid_path_len -1){
    printf("Failed to copy over file aliase aphid path from database to copy of file aliase.\n");
    return 1;
  }
  /*
  aliases->aliases[curr_count].username = (char*) memcpy(aliases->aliases[curr_count].username, col_values[0], strlen(col_values[0]));
  aliases->aliases[curr_count].name = (char*) memcpy(aliases->aliases[curr_count].name, col_values[1], strlen(col_values[1]));
  aliases->aliases[curr_count].file_size = strtol(col_values[2], NULL, 10);
  aliases->aliases[curr_count].aphid_path = (char*) memcpy(aliases->aliases[curr_count].aphid_path, col_values[3], strlen(col_values[3]));*/
  aliases->count = curr_count;
  return 0;
}
 
int Process_Storage_Blob(void *data, int num_cols, char **col_values, char **col_names){
  DataBlob* blob = (DataBlob*) data;

  uint32_t username_len = strlen(col_values[0]) +1;
  blob->username = calloc(username_len, sizeof(char));
  snprintf(blob->username, username_len, "%s", col_values[0]);

  blob->size = strtol(col_values[2], NULL, 10) +1;
  blob->host_file_index = strtol(col_values[3],NULL, 10);

  blob->blob = calloc( blob->size, sizeof(unsigned char));
  blob->blob = memcpy(blob->blob, col_values[1], blob->size);

  return 0;
}

void Free_Blob(DataBlob *blob){
  free(blob->username);
  free(blob->blob);
}
