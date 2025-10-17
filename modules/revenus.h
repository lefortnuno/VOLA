#ifndef REVENUS_H
#define REVENUS_H

#define REVENUS_FILE "data/revenu.csv" 
#define REVENUS_FIXES_FILE "data/revenu_fixes.csv"

#include <stdio.h>
#include <stdlib.h> 

extern float revenus[];
extern char *noms_revenus[];
extern int nb_revenus;
extern int nb_noms_revenus;

void ensure_revenus_fixes_current_month(); 
void set_revenus_fixes(); 

#endif
