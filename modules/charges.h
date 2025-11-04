#ifndef CHARGES_H
#define CHARGES_H

#include <stdio.h>
#include <stdlib.h> 

extern float charges[];
extern char *noms_charges[];
extern int nb_charges;
extern int nb_noms_charges;


void sync_charge_file(); 

#endif
