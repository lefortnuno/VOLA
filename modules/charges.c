#include "charges.h"
#include "data.h"
#include "revenus.h"
#include "utils.h"

float charges[MAX_ITEMS];
char *noms_charges[MAX_ITEMS];
int nb_charges = 0;
int nb_noms_charges = 0;


void sync_charge_file() {
    int mois, annee;
    get_current_month_year(&mois, &annee);
    init_data();
    
    FILE *f = fopen(CHARGES_FIXES_FILE, "r");

    if (!f) {
        f = fopen(CHARGES_FIXES_FILE, "w");
        if (!f) { perror("Erreur creation fichier revenus fixes"); return; }
        
        // Écrire l'en-tête avec une boucle
        fprintf(f, "mois,annee,%s", noms_charges[0]);
        for (int i = 1; i < nb_noms_charges; i++) {
            fprintf(f, ",%s", noms_charges[i]);
        }
        fprintf(f, "\n%d,%d", mois, annee);
        for (int i = 0; i < nb_charges; i++) {
            fprintf(f, ",%.2f", charges[i]); 
        }
        fprintf(f, "\n");
        
        fclose(f);  
        return;
    }

    FILE *t = fopen(TMP_FILE, "w");

    char line[256]; 
    bool found = false;
    fgets(line, sizeof(line), f); // Lire l'en-tête
    fprintf(t, "%s", line);

    while (fgets(line, sizeof(line), f)) {
        int m, a;
        sscanf(line, "%d,%d", &m, &a);
        if (m == mois && a == annee) {
            found = true;  
            fprintf(t, "%d,%d", mois, annee);  
            for (int i = 0; i < nb_charges; i++) {
                fprintf(t, ",%.2f", charges[i]); 
            }
            fprintf(t, "\n");  
        } else {
            fprintf(t, "%s", line); 
        }
    } 
 
    if (!found) {
        fprintf(t, "%d,%d", mois, annee);  
        for (int i = 0; i < nb_revenus; i++) {
            fprintf(t, ",%.2f", charges[i]); 
        }
        fprintf(t, "\n");  
    }

    fclose(t);
    fclose(f);
     
    remove(CHARGES_FIXES_FILE);  
    rename(TMP_FILE, CHARGES_FIXES_FILE);
}
