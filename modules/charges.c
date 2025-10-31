#include "charges.h"
#include "data.h"
#include "revenus.h"
#include "utils.h"

float charges[MAX_ITEMS];
char *noms_charges[MAX_ITEMS];
int nb_charges = 0;
int nb_noms_charges = 0;


void ensure_charges_fixes_current_month() {
    int mois, annee;
    get_current_month_year(&mois, &annee);
    init_data();

    FILE *f = fopen(CHARGES_FIXES_FILE, "r+");
    if (!f) {
        f = fopen(CHARGES_FIXES_FILE, "w");
        if (!f) { perror("Erreur creation fichier charges fixes"); return; }
        
        // Écrire l'en-tête avec une boucle
        fprintf(f, "mois,annee,%s", noms_charges[0]);
        for (int i = 1; i < nb_noms_charges; i++) {
            fprintf(f, ",%s", noms_charges[i]);
        }
        fprintf(f, "\n");
        
        fclose(f);
        f = fopen(CHARGES_FIXES_FILE, "a");
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
        FILE *fa = fopen(CHARGES_FIXES_FILE, "a");
        if (fa) {
            // Écrire mois et année d'abord
            fprintf(fa, "%d,%d", mois, annee);
            
            // Boucle pour écrire les charges
            for (int i = 0; i < nb_charges; i++) {
                fprintf(fa, ",%.0f", charges[i]);
            }
            fprintf(fa, "\n");
            
            fclose(fa); 
        }
    }
    fclose(f);
}


// modifier a partir d'ici a MAJ
void set_charges_fixes() {
    int mois, annee;
    get_current_month_year(&mois, &annee);

    // Charger les valeurs existantes du mois courant si disponibles
    FILE *fin = fopen(CHARGES_FIXES_FILE, "r");
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
                    if (index >= 2 && (index - 2) < nb_charges) {
                        charges[index - 2] = atof(token);
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

    // Boucle sur toutes les charges connues (dynamiquement)
    for (int i = 0; i < nb_charges; i++) {
        printf("%s (%.2f) : ", noms_charges[i + 2], charges[i]);
        fgets(input, sizeof(input), stdin);
        if (input[0] != '\n') {
            charges[i] = atof(input);
        }
    }

    // Réécrire le fichier avec mise à jour ou ajout
    fin = fopen(CHARGES_FIXES_FILE, "r");
    FILE *tmp = fopen("tmp.csv", "w");
    if (!tmp) {
        perror("Erreur creation fichier temporaire");
        return;
    }

    // Écrire l'en-tête
    fprintf(tmp, "%s", noms_charges[0]);
    for (int i = 1; i < nb_noms_charges; i++) {
        fprintf(tmp, ",%s", noms_charges[i]);
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
                for (int i = 0; i < nb_charges; i++) {
                    fprintf(tmp, ",%.2f", charges[i]);
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
        for (int i = 0; i < nb_charges; i++) {
            fprintf(tmp, ",%.2f", charges[i]);
        }
        fprintf(tmp, "\n");
    }

    fclose(tmp);
    remove(CHARGES_FIXES_FILE);
    rename("tmp.csv", CHARGES_FIXES_FILE);

    printf("✅ Charges fixes mises à jour avec succès.\n");
    // afficher_les_charges();
}

