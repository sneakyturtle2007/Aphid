#include "DB_Management.c"



int main(){

  sqlite3 *db;
  Error status = Start_DB(&db);
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT_SUCCESS!\n");
  }
  unsigned char test[] = "testing";
  DataBlob blob_example = {.username = "luis", .blob = test, .size = sizeof(test)};

  status = Insert_Storage_Blob(&db, &blob_example);
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT SUCCESS!\n");
  }

  DataBlob test1;
  status = Get_Storage_Blob(&db, &test1, "luis");
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT SUCCESS!\n");
  }
  status = Remove_Storage_Blob(&db, test1.username);
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT_SUCCESS!\n");
  }
  
  printf("Username: %s\nData Blob: %s\n", test1.username, test1.blob);
  Free_Blob(&test1);
  sqlite3_close(db);
  return status.type;
}
