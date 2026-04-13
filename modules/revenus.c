#include "charges.h" 
#include "data.h" 
#include "revenus.h"
#include "utils.h"
#include <time.h>
 
float revenus[MAX_ITEMS];
char *noms_revenus[MAX_ITEMS];
int nb_revenus = 0;
int nb_noms_revenus = 0;

void sync_revenu_file() {
    int mois, annee;
    get_current_month_year(&mois, &annee);  
    init_data();

    FILE *f = fopen(REVENUS_FIXES_FILE, "r");

    if (!f) {
        f = fopen(REVENUS_FIXES_FILE, "w");
        if (!f) { perror("Erreur creation fichier revenus fixes"); return; }
        
        // Écrire l'en-tête avec une boucle
        fprintf(f, "mois,annee,%s", noms_revenus[0]);
        for (int i = 1; i < nb_noms_revenus; i++) {
            fprintf(f, ",%s", noms_revenus[i]); 
        }
        fprintf(f, "\n%d,%d", mois, annee);
        for (int i = 0; i < nb_revenus; i++) {
            fprintf(f, ",%.2f", revenus[i]); 
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
            for (int i = 0; i < nb_revenus; i++) {
                fprintf(t, ",%.2f", revenus[i]); 
            }
            fprintf(t, "\n");  
        } else {
            fprintf(t, "%s", line); 
        }
    } 
 
    if (!found) {
        fprintf(t, "%d,%d", mois, annee);  
        for (int i = 0; i < nb_revenus; i++) {
            fprintf(t, ",%.2f", revenus[i]); 
        }
        fprintf(t, "\n");  
    }

    fclose(t);
    fclose(f);
     
    remove(REVENUS_FIXES_FILE);  
    rename(TMP_FILE, REVENUS_FIXES_FILE);
}
