
#ifndef DB_MANAGEMENT_H
#define DB_MANAGEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "sqlite3.h"
#include "../Error_Handling/Error_Handling.h"

typedef struct{
  char* name;
  uint32_t file_size;
  char* aphid_path;
}Alias;

typedef struct{
  Alias* aliases;
  int count;
  int capacity;
}AliasList;

typedef struct{
  char *username;
  unsigned char *blob;
  uint32_t size;
}DataBlob;

static Error Start_DB(sqlite3 **db);
static Error Insert_Storage_Blob(sqlite3 **db, DataBlob *data);
static Error Remove_Storage_Blob(sqlite3 **db, char *username);
static Error Get_Storage_Blob(sqlite3 **db, DataBlob *blob, char* username);
static Error Insert_File_Aliases(sqlite3 **db, char* file_name, int file_size, char* aphid_path);
static Error Remove_File_Alias(sqlite3 **db, char *file_name);
static Error Get_File_Alias(sqlite3 **db, AliasList* aliases);
static void Free_Blob(DataBlob *blob);

#endif // DB_Management_H
