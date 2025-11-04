#include "bilan.h"
#include "charges.h"
#include "data.h"
#include "depenses.h"
#include "revenus.h"
#include "utils.h"

int i_epargne = 69;


static char *my_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy) memcpy(copy, s, len); 
    return copy;
}

void reset_data() {
    nb_charges = nb_noms_charges = 0;
    nb_revenus = nb_noms_revenus = 0;
    i_epargne = 69;
    for (int i = 0; i < MAX_ITEMS; i++) {
        charges[i] = revenus[i] = 0.0f;
        if (noms_charges[i]) { free(noms_charges[i]); noms_charges[i] = NULL; }
        if (noms_revenus[i]) { free(noms_revenus[i]); noms_revenus[i] = NULL; }
    }
}

void read_schema() {
    reset_data();

    FILE *f = fopen(DATA_FILE, "r");
    FILE *t = fopen(TMP_FILE, "w");
    if (!f) {
        perror("Erreur ouverture fichier data.csv");
        return;
    }
    
    double epargne_precedente = get_reste_general_precedent(); 

    char line[256];
    int mode = 0; // 1 = charges, 2 = revenus 

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0; // supprimer le \n

        if (strstr(line, "Charges Fixes")) {
            mode = 1;
            fprintf(t, "%s\n", line);
            continue;
        } else if (strstr(line, "Revenus Fixes")) {
            mode = 2;
            fprintf(t, "%s\n", line);
            continue;
        }

        if (mode == 1 && nb_noms_charges == 0 && strchr(line, ',')) {
            // noms des charges
            char *token = strtok(line, ","); 
            while (token && nb_noms_charges < MAX_ITEMS) { 
                fprintf(t, nb_noms_charges == 0 ? "%s" : ",%s", token);
                noms_charges[nb_noms_charges] = my_strdup(token);
                nb_noms_charges++;
                token = strtok(NULL, ",");
            }
            fprintf(t, "\n");
        } else if (mode == 1 && nb_charges == 0 && strchr(line, ',')) {
            // valeurs des charges
            char *token = strtok(line, ","); 
            while (token && nb_charges < MAX_ITEMS) {
                fprintf(t, nb_charges == 0 ? "%s" : ",%s", token);
                charges[nb_charges] = atof(token);
                nb_charges++;
                token = strtok(NULL, ","); 
            }
            fprintf(t, "\n");
        } else if (mode == 2 && nb_noms_revenus == 0 && strchr(line, ',')) {
            // noms des revenus
            char *token = strtok(line, ","); 
            while (token && nb_noms_revenus < MAX_ITEMS) {
                fprintf(t, nb_noms_revenus == 0 ? "%s" : ",%s", token);
                noms_revenus[nb_noms_revenus] = my_strdup(token); 
                if(i_epargne == 69 && strcmp(token, "epargne") == 0) i_epargne = nb_noms_revenus; 
                nb_noms_revenus++;
                token = strtok(NULL, ",");
            }
            fprintf(t, "\n");
        } else if (mode == 2 && nb_revenus == 0 && strchr(line, ',')) {
            // valeurs des revenus
            char *token = strtok(line, ","); 
            while (token && nb_revenus < MAX_ITEMS) { 
                if(i_epargne != 69 && i_epargne == nb_revenus) fprintf(t, nb_revenus == 0 ? "%.0f" : ",%.0f", epargne_precedente);
                else fprintf(t, nb_revenus == 0 ? "%s" : ",%s", token);

                if(i_epargne != 69 && i_epargne == nb_revenus) revenus[nb_revenus] = epargne_precedente;
                else revenus[nb_revenus] = atof(token);  

                nb_revenus++;
                token = strtok(NULL, ","); 
            }
            fprintf(t, "\n");
        }
    }
    fclose(f); 
    fclose(t); 
    
    // REMPLACE DATA_FILE par TMP_FILE
    remove(DATA_FILE); 
    rename(TMP_FILE, DATA_FILE); 
}
 
void init_data() {
    read_schema();
    
    if (nb_charges == 0 || nb_revenus == 0) {
        printf("⚠️ Données manquantes dans %s\n", DATA_FILE);
    }
}

void update_schema() { 
    choix_invalide();
}
 
// void sync_data_file(const char *filename) { 
//     int mois, annee;
//     get_current_month_year(&mois, &annee);

//     FILE *f = fopen(filename, "r"); 
//     FILE *t = fopen(TMP_FILE, "w"); 
//     if (!f) {
//         perror("Erreur ouverture fichier csv source");
//         return;
//     }  

//     char line[256];
//     fgets(line, sizeof(line), f); // skip header "trofel"
//     fprintf(t, "%s", line);

//     while (fgets(line, sizeof(line), f)) {
//         int m, a, idx = 0; 
//         char line_copy[256];
//         strcpy(line_copy, line); // Copie pour strtok
//         char *token = strtok(line_copy, ",");
        
//         while (token && idx < 2) {
//             if (idx == 0) m = atoi(token);
//             else if (idx == 1) a = atoi(token); 
//             token = strtok(NULL, ",");
//             idx++;
//         }
//         if (m != mois || a != annee) fprintf(t, "%s", line); 
//         }

//     fclose(f); 
//     fclose(t);

//     remove(filename);  
//     rename(TMP_FILE, filename); 
// }
 