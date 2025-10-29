#ifndef UTILS_H
#define UTILS_H

#define DATA_FILE "data/data.csv"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
extern int choix;

void get_current_date(char *date_str, size_t size);
void get_current_time(char *time_str, size_t size);
void format_date_affichage(const char *date_in, char *date_out, size_t size);
void format_heure_affichage(const char *time_in, char *time_out, size_t size);
void get_current_month_year(int *mois, int *annee);
int get_week_number(const char *date_str);
void choix_invalide();
void choix_de_quitter();

#endif
