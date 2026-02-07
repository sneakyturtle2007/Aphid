
#ifndef DB_MANAGEMENT_H
#define DB_MANAGEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "../Error_Handling/Error_Handling.h"

static Error Start_DB(sqlite3 **db);

#endif // DB_Management_H
