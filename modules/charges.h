#ifndef CHARGES_H
#define CHARGES_H

#define DEPENSES_FILE "VOLA/data/depenses.csv"
#define CHARGES_FIXES_FILE "VOLA/data/charges_fixes.csv"

#include <stdio.h>
#include <stdlib.h> 

void ensure_charges_fixes_current_month(); 
void set_charges_fixes();
void afficher_les_charges();

#endif
