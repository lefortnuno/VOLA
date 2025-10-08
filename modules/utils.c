#include "utils.h"

void get_current_date(char *date_str, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(date_str, size, "%Y-%m-%d", t);
}

void get_current_time(char *time_str, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(time_str, size, "%H:%M:%S", t);
}

void format_date_affichage(const char *date_in, char *date_out, size_t size) {
    int y, m, d;
    sscanf(date_in, "%d-%d-%d", &y, &m, &d);
    const char *mois_fr[] = {
        "Janvier","Fevrier","Mars","Avril","Mai","Juin",
        "Juillet","Aout","Septembre","Octobre","Novembre","Decembre"
    };
    snprintf(date_out, size, "%02d %s %d", d, mois_fr[m - 1], y);
}

void format_heure_affichage(const char *time_in, char *time_out, size_t size) {
    int h, m, s;
    sscanf(time_in, "%d:%d:%d", &h, &m, &s);
    snprintf(time_out, size, "%02dh%02d", h, m);
}

void get_current_month_year(int *mois, int *annee) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    *mois = t->tm_mon + 1;
    *annee = t->tm_year + 1900;
}

int get_week_number(const char *date_str) {
    int y, m, d;
    sscanf(date_str, "%d-%d-%d", &y, &m, &d);
    struct tm t = {0};
    t.tm_year = y - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = d;
    mktime(&t);
    int wday = (t.tm_wday == 0 ? 7 : t.tm_wday);
    int yday = t.tm_yday + 1;
    return (yday - wday + 10) / 7;
}

void choix_invalide(){ 
    printf("\033[1;31m"); // rouge vif
    printf("\n  -----------------------------------------\n");
    printf("||    Choix invalide, veuillez reessayer ! ||\n");
    printf("  -----------------------------------------\n");
    printf("|| Choix: "); 
    printf("\033[0m"); // réinitialise la couleur 
}