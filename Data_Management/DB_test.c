#include "DB_Management.c"



int main(){
  sqlite3 *db;
  Error status = Start_DB(&db);
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT_SUCCESS!\n");
  }
  char *blob_example = calloc(8, sizeof(char));
  
  status = Insert_Storage_Blob(&db, "luis", "testing");
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT SUCCESS!\n");
  }
  DataBlob test;
  status = Get_Storage_Blob(&db, &test, "luis");
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT SUCCESS!\n");
  }
  printf("Username: %s\nData Blob: %s\n", test.username, test.blob);
  sqlite3_close(db);
  return status.type;
}
