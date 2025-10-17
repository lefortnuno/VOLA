#ifndef CHARGES_H
#define CHARGES_H

#define DEPENSES_FILE "data/depenses.csv"
#define CHARGES_FIXES_FILE "data/charges_fixes.csv"
#define REVENUS_FIXES_FILE "data/revenu_fixes.csv"

#include <stdio.h>
#include <stdlib.h> 

extern char *spc;
extern float charges[];
extern char *noms_charges[];
extern int nb_charges;
extern int nb_noms_charges;

void ensure_charges_fixes_current_month(); 
void set_charges_fixes();
void afficher_les_charges();
void init_data();
void lire_data_csv();
void reset_data() ;

#endif
