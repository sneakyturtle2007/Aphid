#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#define MEMORY_ERROR 1
#define DATABASE_ERROR 2
#define INVALID_FILE 3
#define FILE_SIZE 4
#define CORRUPT_DATA 5
#define INVALID_HOSTNAME 6
#define GREAT_SUCCESS 0


typedef struct{
  char message[255];
  int type;
}Error;

#endif // ERROR_HANDLING_H
