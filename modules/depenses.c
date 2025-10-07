#include "depenses.h"
#include "utils.h"
#include <ctype.h>

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
        if (!f) { perror("Erreur ouverture fichier depenses"); return; }
        fprintf(f, "%s,%s,%.2f\n", date, heure, montant);
        fclose(f);

        printf("→ %.2f enregistree.\n", montant);
    }
}

void afficher_depenses() {
    FILE *f = fopen(DEPENSES_FILE, "r");
    if (!f) { printf("Aucune depense enregistree.\n"); return; }

    char line[256]; double total = 0.0;
    printf("\n=== HISTORIQUE DES DEPENSES ===\n");
    while (fgets(line, sizeof(line), f)) {
        char date[20], heure[20]; double montant;
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

void afficher_depenses_mensuelles() {
    FILE *f = fopen(DEPENSES_FILE, "r");
    if (!f) { printf("Aucune depense enregistree.\n"); return; }

    double totals[13][3000] = {{0}};
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char date[20], heure[20]; double montant;
        int y, m, d;
        sscanf(line, "%[^,],%[^,],%lf", date, heure, &montant);
        sscanf(date, "%d-%d-%d", &y, &m, &d);
        if (y >= 2000 && y < 5000 && m >= 1 && m <= 12)
            totals[m][y - 2000] += montant;
    }
    fclose(f);

    const char *mois_fr[] = {
        "Janvier","Fevrier","Mars","Avril","Mai","Juin",
        "Juillet","Aout","Septembre","Octobre","Novembre","Decembre"
    };
    printf("\n=== DEPENSES PAR MOIS ===\n");
    for (int an = 0; an < 3000; an++) {
        for (int m2 = 1; m2 <= 12; m2++) {
            if (totals[m2][an] > 0)
                printf("%s %d : %.2f\n", mois_fr[m2-1], an+2000, totals[m2][an]);
        }
    }
    printf("=========================\n\n");
}

void afficher_depenses_hebdo() {
    FILE *f = fopen(DEPENSES_FILE, "r");
    if (!f) { printf("Aucune depense enregistree.\n"); return; }

    double weekly_totals[3000][60] = {{0}};
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        char date[20], heure[20]; double montant;
        int y, m, d;
        sscanf(line, "%[^,],%[^,],%lf", date, heure, &montant);
        sscanf(date, "%d-%d-%d", &y, &m, &d);
        int week = get_week_number(date);
        if (y >= 2000 && y < 5000 && week >= 1 && week <= 53)
            weekly_totals[y - 2000][week] += montant;
    }
    fclose(f);

    printf("\n=== DEPENSES PAR SEMAINE ===\n");
    for (int an = 0; an < 3000; an++) {
        for (int w = 1; w <= 53; w++) {
            if (weekly_totals[an][w] > 0) {
                double total = weekly_totals[an][w];
                printf("Semaine %02d - %d : %.2f dhs ", w, an+2000, total);
                printf(total <= 100.0 ? "✅ (dans la limite)\n" :
                                        "⚠️ DEPASSE la limite de 100 dhs !\n");
            }
        }
    }
    printf("============================\n\n");
}
