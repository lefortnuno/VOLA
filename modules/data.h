#ifndef SCHEMA_H
#define SCHEMA_H

#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
  

void reset_data();
void init_data();
void read_schema();
void update_charge();
void update_revenu();

void add_charge_fixe();
void modifier_charge_fixe();
void supprimer_charge_fixe();

void add_revenu_fixe();
void modifier_revenu_fixe();
void supprimer_revenu_fixe();

#endif
