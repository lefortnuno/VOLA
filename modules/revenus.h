#ifndef REVENUS_H
#define REVENUS_H

#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>

extern float revenus[];
extern char *noms_revenus[];
extern int nb_revenus;
extern int nb_noms_revenus;
extern int i_epargne;


void sync_revenu_file();  

#endif
