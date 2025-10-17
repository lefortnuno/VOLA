#include "revenus.h"
#include "utils.h"
 
void ensure_revenus_fixes_current_month() {
    int mois, annee;
    get_current_month_year(&mois, &annee);

    FILE *f = fopen(REVENUS_FIXES_FILE, "r+");
    if (!f) {
        f = fopen(REVENUS_FIXES_FILE, "w");
        if (!f) { perror("Erreur creation fichier revenus fixes"); return; }
        
        // Écrire l'en-tête avec une boucle
        fprintf(f, "mois,annee,%s", noms_revenus[0]);
        for (int i = 1; i < nb_noms_revenus; i++) {
            fprintf(f, ",%s", noms_revenus[i]);
        }
        fprintf(f, "\n");
        
        fclose(f);
        f = fopen(REVENUS_FIXES_FILE, "a");
    }

    char line[256];
    int found = 0;
    fgets(line, sizeof(line), f); // Lire l'en-tête
    while (fgets(line, sizeof(line), f)) {
        int m, a;
        sscanf(line, "%d,%d", &m, &a);
        if (m == mois && a == annee) { found = 1; break; }
    }

    if (!found) {
        FILE *fa = fopen(REVENUS_FIXES_FILE, "a");
        if (fa) {
            // Écrire mois et année d'abord
            fprintf(fa, "%d,%d", mois, annee);
            
            // Boucle pour écrire les revenus
            for (int i = 0; i < nb_revenus; i++) {
                fprintf(fa, ",%.0f", revenus[i]);
            }
            fprintf(fa, "\n");
            
            fclose(fa);
            printf("✅ Revenus fixes creees automatiquement pour %02d/%d.\n", mois, annee);
        }
    }
    fclose(f);
} 

void set_revenus_fixes() {
    int mois, annee;
    get_current_month_year(&mois, &annee);

    // Charger les valeurs existantes du mois courant si disponibles
    FILE *fin = fopen(REVENUS_FIXES_FILE, "r");
    if (fin) {
        char line[256];
        fgets(line, sizeof(line), fin); // skip header
        while (fgets(line, sizeof(line), fin)) {
            int m, a;
            sscanf(line, "%d,%d", &m, &a);
            if (m == mois && a == annee) {
                char *token = strtok(line, ",");
                int index = 0;
                while (token != NULL) {
                    if (index >= 2 && (index - 2) < nb_revenus) {
                        revenus[index - 2] = atof(token);
                    }
                    token = strtok(NULL, ",");
                    index++;
                }
                break;
            }
        }
        fclose(fin);
    }

    printf("Mois courant : %02d/%d\n", mois, annee);
    char input[50];

    // Boucle sur toutes les revenus connues (dynamiquement)
    for (int i = 0; i < nb_revenus; i++) {
        printf("%s (%.2f) : ", noms_revenus[i + 2], revenus[i]);
        fgets(input, sizeof(input), stdin);
        if (input[0] != '\n') {
            revenus[i] = atof(input);
        }
    }

    // Réécrire le fichier avec mise à jour ou ajout
    fin = fopen(REVENUS_FIXES_FILE, "r");
    FILE *tmp = fopen("tmp.csv", "w");
    if (!tmp) {
        perror("Erreur creation fichier temporaire");
        return;
    }

    // Écrire l'en-tête
    fprintf(tmp, "%s", noms_revenus[0]);
    for (int i = 1; i < nb_noms_revenus; i++) {
        fprintf(tmp, ",%s", noms_revenus[i]);
    }
    fprintf(tmp, "\n");

    int updated = 0;
    char line[256];

    if (fin) {
        fgets(line, sizeof(line), fin); // skip header
        while (fgets(line, sizeof(line), fin)) {
            int m, a;
            sscanf(line, "%d,%d", &m, &a);
            if (m == mois && a == annee) {
                fprintf(tmp, "%d,%d", mois, annee);
                for (int i = 0; i < nb_revenus; i++) {
                    fprintf(tmp, ",%.2f", revenus[i]);
                }
                fprintf(tmp, "\n");
                updated = 1;
            } else {
                fputs(line, tmp);
            }
        }
        fclose(fin);
    }

    if (!updated) {
        fprintf(tmp, "%d,%d", mois, annee);
        for (int i = 0; i < nb_revenus; i++) {
            fprintf(tmp, ",%.2f", revenus[i]);
        }
        fprintf(tmp, "\n");
    }

    fclose(tmp);
    remove(REVENUS_FIXES_FILE);
    rename("tmp.csv", REVENUS_FIXES_FILE);

    printf("✅ Charges fixes mises à jour avec succès.\n");
    // afficher_les_charges();
}

