#ifndef UTILS_H
#define UTILS_H

#define MAX_ITEMS 50
#define MAX_NAME_LEN 64

#define DATA_FILE "data/data.csv"
#define DEPENSES_FILE "data/depenses.csv"
#define CHARGES_FIXES_FILE "data/charges_fixes.csv"
#define REVENUS_FIXES_FILE "data/revenu_fixes.csv"
#define REVENUS_FILE "data/revenu.csv" 
#define TMP_FILE "data/tmp.csv" 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
extern int choix;
extern char *spc;


void get_current_date(char *date_str, size_t size);
void get_current_time(char *time_str, size_t size);
void format_date_affichage(const char *date_in, char *date_out, size_t size);
void format_heure_affichage(const char *time_in, char *time_out, size_t size);
void get_current_month_year(int *mois, int *annee);
int get_week_number(const char *date_str);
void choix_invalide();
void choix_de_quitter();

#endif
