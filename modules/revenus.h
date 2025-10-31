#ifndef REVENUS_H
#define REVENUS_H

#include <stdio.h>
#include <stdlib.h> 

extern float revenus[];
extern char *noms_revenus[];
extern int nb_revenus;
extern int nb_noms_revenus;
extern int i_epargne;

void ensure_revenus_fixes_current_month(); 
void set_revenus_fixes(); 

#endif
