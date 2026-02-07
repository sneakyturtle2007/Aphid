#include "DB_Management.c"



int main(){
  sqlite3 *db;
  Error status = Start_DB(&db);
  if(status.type != GREAT_SUCCESS){
    printf("%s", status.message);
  }else{
    printf("GREAT_SUCCESS!\n");
  }
  sqlite3_close(db);
  return status.type;
}
