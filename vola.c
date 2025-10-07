#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define DEPENSES_FILE "depenses.csv"
#define CHARGES_FIXES_FILE "charges_fixes.csv"

/// ====== UTILS ======
void get_current_date(char *date_str, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(date_str, size, "%Y-%m-%d", t);  // format stockage
}

void get_current_time(char *time_str, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(time_str, size, "%H:%M:%S", t);  // format stockage
}

void format_date_affichage(const char *date_in, char *date_out, size_t size) {
    int y, m, d;
    sscanf(date_in, "%d-%d-%d", &y, &m, &d);

    const char *mois_fr[] = {
        "Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin",
        "Juillet", "Aout", "Septembre", "Octobre", "Novembre", "Decembre"
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
// === DEPENSES HEBDOMADAIRES ===
int get_week_number(const char *date_str) {
    int y, m, d;
    sscanf(date_str, "%d-%d-%d", &y, &m, &d);

    struct tm t = {0};
    t.tm_year = y - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = d;
    mktime(&t);

    // semaine ISO approximative (lundi = début)
    int wday = (t.tm_wday == 0 ? 7 : t.tm_wday);
    int yday = t.tm_yday + 1;
    return (yday - wday + 10) / 7; 
}

void afficher_depenses_hebdo() {
    FILE *f = fopen(DEPENSES_FILE, "r");
    if (!f) {
        printf("Aucune depense enregistree.\n");
        return;
    }

    double weekly_totals[3000][60] = {0}; // [annee-2000][semaine]
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        char date[20], heure[20];
        double montant;
        int y, m, d;
        sscanf(line, "%[^,],%[^,],%lf", date, heure, &montant);
        sscanf(date, "%d-%d-%d", &y, &m, &d);

        int week = get_week_number(date);
        if (y >= 2000 && y < 5000 && week >= 1 && week <= 53) {
            weekly_totals[y - 2000][week] += montant;
        }
    }
    fclose(f);

    printf("\n=== DEPENSES PAR SEMAINE ===\n");
    int an, w;
    for (an = 0; an < 3000; an++) {
        for (w = 1; w <= 53; w++) {
            if (weekly_totals[an][w] > 0) {
                double total = weekly_totals[an][w];
                printf("Semaine %02d - %d : %.2f dhs ", w, an+2000, total);
                if (total <= 100.0) {
                    printf("✅ (dans la limite)\n");
                } else {
                    printf("⚠️ DEPASSE la limite de 100 dhs !\n");
                }
            }
        }
    }
    printf("============================\n\n");
}


/// ====== DEPENSES VARIABLES ======
void add_depense();
void afficher_depenses();

void add_depense() {
    char input[50];
    printf("Saisissez vos depenses (entrez 'q' pour quitter):\n");

    while (1) {
        printf("Montant: ");
        scanf("%s", input);

        if (tolower(input[0]) == 'q') {
            printf("✅ Fin de saisie.\n");
            afficher_depenses();
            break;
        }

        double montant = atof(input);
        if (montant <= 0) {
            printf("⚠️ Montant invalide.\n");
            continue;
        }

        char date[20], heure[20];
        get_current_date(date, sizeof(date));
        get_current_time(heure, sizeof(heure));

        FILE *f = fopen(DEPENSES_FILE, "a");
        if (!f) {
            perror("Erreur ouverture fichier depenses");
            return;
        }
        fprintf(f, "%s,%s,%.2f\n", date, heure, montant);
        fclose(f);

        printf("→ %.2f enregistree.\n", montant);
    }
}

void afficher_depenses() {
    FILE *f = fopen(DEPENSES_FILE, "r");
    if (!f) {
        printf("Aucune depense enregistree.\n");
        return;
    }

    char line[256];
    double total = 0.0;
    printf("\n=== HISTORIQUE DES DEPENSES ===\n");

    while (fgets(line, sizeof(line), f)) {
        char date[20], heure[20];
        double montant;
        sscanf(line, "%[^,],%[^,],%lf", date, heure, &montant);

        char date_fmt[50], heure_fmt[20];
        format_date_affichage(date, date_fmt, sizeof(date_fmt));
        format_heure_affichage(heure, heure_fmt, sizeof(heure_fmt));

        printf("%s %s -> %.2f\n", date_fmt, heure_fmt, montant);
        total += montant;
    }
    fclose(f);

    printf("=== Total global : %.2f ===\n\n", total);
}

/// ====== NOUVELLE FONCTION : TOTAL PAR MOIS ======
void afficher_depenses_mensuelles() {
    FILE *f = fopen(DEPENSES_FILE, "r");
    if (!f) {
        printf("Aucune depense enregistree.\n");
        return;
    }

    double totals[13][3000] = {0}; 
    // [mois][annee-2000] pour stocker (2000 -> 4999)

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char date[20], heure[20];
        double montant;
        int y, m, d;
        sscanf(line, "%[^,],%[^,],%lf", date, heure, &montant);
        sscanf(date, "%d-%d-%d", &y, &m, &d);

        if (y >= 2000 && y < 5000 && m >= 1 && m <= 12) {
            totals[m][y - 2000] += montant;
        }
    }
    fclose(f);

    const char *mois_fr[] = {
        "Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin",
        "Juillet", "Aout", "Septembre", "Octobre", "Novembre", "Decembre"
    };

    printf("\n=== DEPENSES PAR MOIS ===\n");

    int an, m2;   // 🔥 déclarées ici
    for (an = 0; an < 3000; an++) {
        for (m2 = 1; m2 <= 12; m2++) {
            if (totals[m2][an] > 0) {
                printf("%s %d : %.2f\n", mois_fr[m2-1], an+2000, totals[m2][an]);
            }
        }
    }

    printf("=========================\n\n");
}


/// ====== CHARGES FIXES ======
void ensure_charges_fixes_current_month();
void afficher_charges_fixes();
void set_charges_fixes();

void ensure_charges_fixes_current_month() {
    int mois, annee;
    get_current_month_year(&mois, &annee);

    FILE *f = fopen(CHARGES_FIXES_FILE, "r+");
    if (!f) {
        f = fopen(CHARGES_FIXES_FILE, "w");
        if (!f) { perror("Erreur creation fichier charges fixes"); return; }
        fprintf(f, "mois,annee,loyer,wifi,tram,redal,iruno\n");
        fclose(f);
        f = fopen(CHARGES_FIXES_FILE, "a");
    }

    char line[256];
    int found = 0;
    fgets(line, sizeof(line), f); // skip header

    while (fgets(line, sizeof(line), f)) {
        int m, a;
        sscanf(line, "%d,%d", &m, &a);
        if (m == mois && a == annee) {
            found = 1;
            break;
        }
    }

    if (!found) {
        FILE *fa = fopen(CHARGES_FIXES_FILE, "a");
        if (fa) {
            fprintf(fa, "%d,%d,1150,100,160,50,1000\n", mois, annee);
            fclose(fa);
            printf("✅ Charges fixes creees automatiquement pour %02d/%d.\n", mois, annee);
        }
    }

    fclose(f);
}

void afficher_charges_fixes() {
    ensure_charges_fixes_current_month();

    int mois, annee;
    get_current_month_year(&mois, &annee);

    FILE *f = fopen(CHARGES_FIXES_FILE, "r");
    if (!f) {
        printf("Erreur lecture charges fixes.\n");
        return;
    }

    char line[256];
    fgets(line, sizeof(line), f); // skip header
    while (fgets(line, sizeof(line), f)) {
        int m, a;
        double loyer, wifi, tram, redal, iruno;
        sscanf(line, "%d,%d,%lf,%lf,%lf,%lf,%lf",
               &m, &a, &loyer, &wifi, &tram, &redal, &iruno);
        if (m == mois && a == annee) {
            char date_str[50];
            snprintf(line, sizeof(line), "%d-%02d-01", a, m);
            format_date_affichage(line, date_str, sizeof(date_str));

            printf("\n=== CHARGES FIXES (%s) ===\n", date_str);
            printf("Loyer : %.2f\nWifi : %.2f\nTram : %.2f\nRedal : %.2f\nIruno : %.2f\n",
                   loyer, wifi, tram, redal, iruno);
            printf("Total charges fixes : %.2f\n\n",
                   loyer + wifi + tram + redal + iruno);
            break;
        }
    }
    fclose(f);
}

void set_charges_fixes() {
    int mois, annee;
    get_current_month_year(&mois, &annee);

    double loyer = 1150, wifi = 100, tram = 160, redal = 50, iruno = 1000;
    FILE *fin = fopen(CHARGES_FIXES_FILE, "r");
    if (fin) {
        char line[256];
        fgets(line, sizeof(line), fin); // skip header
        while (fgets(line, sizeof(line), fin)) {
            int m, a;
            double tl, tw, tt, tr, ti;
            sscanf(line, "%d,%d,%lf,%lf,%lf,%lf,%lf", &m, &a, &tl, &tw, &tt, &tr, &ti);
            if (m == mois && a == annee) {
                loyer = tl; wifi = tw; tram = tt; redal = tr; iruno = ti;
                break;
            }
        }
        fclose(fin);
    }

    printf("Mois courant : %02d/%d\n", mois, annee);
    char input[50];

    printf("Loyer (%.2f) : ", loyer);
    fgets(input, sizeof(input), stdin); // vider tampon
    if (input[0] != '\n') loyer = atof(input);

    printf("Wifi (%.2f) : ", wifi);
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') wifi = atof(input);

    printf("Tram (%.2f) : ", tram);
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') tram = atof(input);

    printf("Redal (%.2f) : ", redal);
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') redal = atof(input);

    printf("Iruno (%.2f) : ", iruno);
    fgets(input, sizeof(input), stdin);
    if (input[0] != '\n') iruno = atof(input);

    fin = fopen(CHARGES_FIXES_FILE, "r");
    FILE *tmp = fopen("tmp.csv", "w");
    char line[256];

    fprintf(tmp, "mois,annee,loyer,wifi,tram,redal,iruno\n");

    int updated = 0;
    if (fin) {
        fgets(line, sizeof(line), fin); // skip header
        while (fgets(line, sizeof(line), fin)) {
            int m, a;
            sscanf(line, "%d,%d", &m, &a);
            if (m == mois && a == annee) {
                fprintf(tmp, "%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                        mois, annee, loyer, wifi, tram, redal, iruno);
                updated = 1;
            } else {
                fputs(line, tmp);
            }
        }
        fclose(fin);
    }

    if (!updated) {
        fprintf(tmp, "%d,%d,%.2f,%.2f,%.2f,%.2f,%.2f\n",
                mois, annee, loyer, wifi, tram, redal, iruno);
    }

    fclose(tmp);
    remove(CHARGES_FIXES_FILE);
    rename("tmp.csv", CHARGES_FIXES_FILE);

    printf("✅ Charges fixes mises a jour avec succes.\n");
    afficher_charges_fixes();
}

/// ====== MENU PRINCIPAL ======
void menu() {
    int choix;
    do {
        printf("\n===== GESTION DEPENSES =====\n");
        printf("1. Ajouter des depenses\n");
        printf("2. Afficher l'historique des depenses\n");
        printf("3. Afficher les charges fixes\n");
        printf("4. Modifier les charges fixes du mois\n");
        printf("5. Afficher total des depenses par mois\n");
        printf("6. Afficher total des depenses par semaine (limite 100 dhs)\n");
        printf("0. Quitter\n");
        printf("Choix: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: add_depense(); break;
            case 2: afficher_depenses(); break;
            case 3: afficher_charges_fixes(); break;
            case 4: set_charges_fixes(); break;
            case 5: afficher_depenses_mensuelles(); break;
            case 6: afficher_depenses_hebdo(); break;
            case 0: printf("Au revoir 👋\n"); break;
            default: printf("Choix invalide.\n"); break;
        }
    } while (choix != 0);
}


int main() {
    ensure_charges_fixes_current_month();
    menu();
    return 0;
}
