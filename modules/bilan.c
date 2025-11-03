#include "charges.h"
#include "data.h"
#include "depenses.h"
#include "revenus.h"
#include "utils.h"
#include <ctype.h>


// Affichage des Revenus et des Charges 
void afficher_bilan() {
    int mois, annee;
    printf("\033[3J\033[H\033[2J"); 
    sync_data_file(REVENUS_FIXES_FILE);
    sync_data_file(CHARGES_FIXES_FILE); 
    ensure_charges_fixes_current_month();
    ensure_revenus_fixes_current_month(); 
    get_current_month_year(&mois, &annee);

    FILE *f = fopen(CHARGES_FIXES_FILE, "r");
    if (!f) { 
        printf("❌ Erreur lecture charges fixes.\n"); 
        return; 
    }
    FILE *r = fopen(REVENUS_FIXES_FILE, "r");
    if (!r) { 
        printf("❌ Erreur lecture revenus fixes.\n"); 
        return; 
    }

    // Titre principal avec le mois
    char mois_annee_str[50];
    snprintf(mois_annee_str, sizeof(mois_annee_str), "%d-%02d-01", annee, mois);
    char mois_annee_formate[50];
    format_date_affichage(mois_annee_str, mois_annee_formate, sizeof(mois_annee_formate));
    
    printf("==============================================\n");
    printf("%-4sSYNTHESE DU MOIS : %s\n", spc, mois_annee_formate);
    printf("==============================================\n");

    char line[256];
    char revenuLine[256];
    fgets(line, sizeof(line), f); // Lire l'en-tête
    fgets(revenuLine, sizeof(revenuLine), r); // Lire l'en-tête

    while (fgets(line, sizeof(line), f)) { 
        int m, a;
        double charges_lues[nb_charges];
        char *token;
        int index = 0;
        
        int rm, ra;
        double revenus_lues[nb_revenus];
        char *rtoken;
        int rindex = 0;
 
        fgets(revenuLine, sizeof(revenuLine), r);
        rtoken= strtok(revenuLine, ",");
        while (rtoken != NULL && rindex < 2 + nb_revenus) {
            if (rindex == 0) rm = atoi(rtoken);
            else if (rindex == 1) ra = atoi(rtoken);
            else if (rindex - 2 < nb_revenus) revenus_lues[rindex - 2] = atof(rtoken);
            
            rtoken = strtok(NULL, ","); 
            rindex++;
        }  
        

        token = strtok(line, ",");
        while (token != NULL && index < 2 + nb_charges) {
            if (index == 0) m = atoi(token);
            else if (index == 1) a = atoi(token);
            else if (index - 2 < nb_charges) charges_lues[index - 2] = atof(token);
            
            token = strtok(NULL, ",");
            index++;
        }
        if (m == mois && a == annee && rm == mois && ra == annee) {
            
            printf("\n%2sREVENUS%13s|%2sCHARGES FIXES\n", spc , spc, spc);
            printf("----------------------------------------------\n"); 
            int max_items = (nb_revenus > nb_charges) ? nb_revenus : nb_charges;
            for (int i = 0; i < max_items; i++) { 
                if (i < nb_revenus)
                    printf("%-7s: %8.2f dhs | ", noms_revenus[i], revenus_lues[i]);
                else
                    printf("%-22s| ", spc);

                if (i < nb_charges)
                    printf("%-7s: %8.2f dhs\n", noms_charges[i], charges_lues[i]);
                else
                    printf("%-22s| ", spc); 
            }
            printf("----------------------------------------------\n");
            double total_charges = 0.0;
            for (int i = 0; i < nb_charges; i++) {
                total_charges += charges_lues[i];
            }
            double total_revenus = 0.0;
            for (int i = 0; i < nb_revenus; i++) {
                total_revenus += revenus_lues[i];
            }
            printf("Total  : \033[1;36m%8.2f\033[0m dhs | Total  : \033[1;36m%8.2f\033[0m dhs\n", total_revenus, total_charges);
            
            // Calculer les dépenses du mois
            double total_depenses_mois = 0.0;
            int nb_depenses = 0;
            FILE *f_depenses = fopen(DEPENSES_FILE, "r");
            if (f_depenses) {
                while (fgets(line, sizeof(line), f_depenses)) {
                    char date[20], heure[20];
                    double montant;
                    if (sscanf(line, "%10[^,],%8[^,],%lf", date, heure, &montant) == 3) {
                        int annee_dep, mois_dep, jour_dep;
                        sscanf(date, "%d-%d-%d", &annee_dep, &mois_dep, &jour_dep);
                        if (annee_dep == annee && mois_dep == mois) {
                            total_depenses_mois += montant;
                            nb_depenses++;
                        }
                    }
                }
                fclose(f_depenses);
                
                printf("----------------------------------------------\n");
                printf("\n%2sDIVERS REVENUS%-6s|%2sDIVERS DEPENSES\n", spc, spc, spc);
                printf("----------------------------------------------\n");
                printf("%-22s| Nombre : %4d \n", spc, nb_depenses); 
                printf("%-22s| Depense: \033[1;36m%8.2f\033[0m dhs\n", spc, total_depenses_mois);
                printf("----------------------------------------------\n");
                
                    
                // Calculer les dépenses du mois
                double total_lucky_mois = 0.0;

                // Total général (charges + dépenses)
                double total_general = total_charges + total_depenses_mois;
                double total_rgeneral = total_revenus + total_lucky_mois;
                
                printf("T.SOLDE: \033[1;33m%8.2f\033[0m dhs | ", total_rgeneral);
                printf("T.DPNS : \033[1;33m%8.2f\033[0m dhs\n", total_general);
                printf("----------------------------------------------\n");

                
                double reste_general = total_rgeneral - total_general;
                printf("\n%-22s|%2sSOLDE RESTANT\n", spc, spc); 
                printf("----------------------------------------------\n");
                printf("T.SOLDE - T.DEPENSE:  |  \033[1;33m%8.2f\033[0m dhs\n", reste_general);
                printf("----------------------------------------------\n");
            } else {
                printf("\n   Aucune depense ce mois.\n");
            }
            break;
        }
    }
    fclose(f);
}

double get_reste_general_precedent() {
    double reste_general = 0.0;
    int mois, annee;
    get_current_month_year(&mois, &annee);

    // Mois précédent
    if (--mois == 0) {
        mois = 12;
        annee--;
    }

    FILE *f = fopen(CHARGES_FIXES_FILE, "r");
    FILE *r = fopen(REVENUS_FIXES_FILE, "r");
    FILE *d = fopen(DEPENSES_FILE, "r");
    double total_charges = 0.0, total_revenus = 0.0, total_depenses = 0.0;

    if (!f) {
        printf("❌ Impossible d'ouvrir %s\n", CHARGES_FIXES_FILE);
        return 0.0;
    }
    if (!r) {
        printf("❌ Impossible d'ouvrir %s\n", REVENUS_FIXES_FILE);
        fclose(f);
        return 0.0;
    }
    if (!d) {
        printf("❌ Impossible d'ouvrir %s\n", DEPENSES_FILE);
        fclose(f);
        fclose(r);
        return 0.0;
    }

    char line[256];
    
    // Lecture des CHARGES
    fgets(line, sizeof(line), f); // skip header "trofel"
    while (fgets(line, sizeof(line), f)) {
        int m, a, idx = 0; 
        char *token = strtok(line, ",");
        while (token) {
            if (idx == 0) m = atoi(token);
            else if (idx == 1) a = atoi(token);
            else {
                if (m == mois && a == annee) {
                    total_charges += atof(token);
                }
            }
            token = strtok(NULL, ",");
            idx++;
        }
    }
    fclose(f);

    // Lecture des REVENUS
    fgets(line, sizeof(line), r); // skip header "trofel"
    while (fgets(line, sizeof(line), r)) {
        int m, a, idx = 0; 
        char *token = strtok(line, ",");
        while (token) {
            if (idx == 0) m = atoi(token);
            else if (idx == 1) a = atoi(token);
            else {
                if (m == mois && a == annee) {
                    total_revenus += atof(token);
                }
            }
            token = strtok(NULL, ",");
            idx++;
        }
    }
    fclose(r);
 
    // Lecture des DEPENSES
    while (fgets(line, sizeof(line), d)) { 
        int m, a;
        char *token = strtok(line, ",");
        
        if (token) {
            sscanf(token, "%d-%d", &a, &m);
            token = strtok(NULL, ",");  
            if (token) {
                token = strtok(NULL, ",");  
                if (token && m == mois && a == annee) total_depenses += atof(token); 
            }  
        }
    }
    fclose(d);

    reste_general = total_revenus - total_charges - total_depenses;
    return reste_general;
}
 