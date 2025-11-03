#ifndef SCHEMA_H
#define SCHEMA_H

#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <time.h>
 

void reset_data();
void init_data();
void read_schema(); 
void update_schema(); 
void sync_data_file(const char *filename); 

#endif
