#include "DB_Management.c"



int main(){

  sqlite3 *db;
  Error status = Start_DB(&db);
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT_SUCCESS opening database!\n");
  }
  unsigned char test[] = "testing";
  DataBlob blob_example = {.username = "luis", .blob = test, .size = sizeof(test), .host_file_index = 4};

  status = Insert_Storage_Blob(&db, &blob_example);
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT SUCCESS inserting storage blob!\n");
  }

  DataBlob test1;
  status = Get_Storage_Blob(&db, &test1, "luis");
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT SUCCESS getting storage blob!\n");
  }
  status = Remove_Storage_Blob(&db, test1.username);
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT_SUCCESS removing storage blob!\n");
  }
  printf("Username: %s\nData Blob: %s\n", test1.username, test1.blob);
  status = Insert_File_Alias(&db, "luis", "test.c", 1232, "testionsah");
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT SUCCESS inserting file alias!\n");
  } 
  
  AliasList a1 = {.aliases = calloc(1, sizeof(Alias)), .count = 0, .capacity = 1};
  //a1.aliases[0] = calloc(1, sizeof(Alias));
  /*a1.aliases[0] = (Alias) { .username = "testing", .name = "test.c", .file_size = 32, .aphid_path = "something, something"};*/
  status = Get_File_Aliases(&db, "luis", &a1);
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT_SUCCESS getting file aliases assigned to username 'luis'\n");
  }
  printf("Username: %s\nFile name: %s\n", a1.aliases[0].username, a1.aliases[0].name);
  status = Remove_File_Alias(&db, "luis", "test.c");
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT_SUCCESS removing file alias.\n");
  }
  
 
  Free_Blob(&test1);
  sqlite3_close(db);
  return status.type;
}
