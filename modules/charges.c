#include "charges.h"
#include "revenus.h"
#include "utils.h"

float charges[] = {1150, 100, 160, 50, 0, 0};
char *noms_charges[] = {"mois", "annee", "loyer", "wifi", "tram", "redal", "iruno", "dette"};
int nb_charges = sizeof(charges) / sizeof(charges[0]);
int nb_noms_charges = sizeof(noms_charges) / sizeof(noms_charges[0]);

float revenus[] = {5000, 0, 0};
char *noms_revenus[] = {"mois", "annee", "AC2I", "dev", "autre"};
int nb_revenus = sizeof(revenus) / sizeof(revenus[0]);
int nb_noms_revenus = sizeof(noms_revenus) / sizeof(noms_revenus[0]);

void ensure_charges_fixes_current_month() {
    int mois, annee;
    get_current_month_year(&mois, &annee);

    FILE *f = fopen(CHARGES_FIXES_FILE, "r+");
    if (!f) {
        f = fopen(CHARGES_FIXES_FILE, "w");
        if (!f) { perror("Erreur creation fichier charges fixes"); return; }
        
        // Écrire l'en-tête avec une boucle
        fprintf(f, "%s", noms_charges[0]);
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
            printf("✅ Charges fixes creees automatiquement pour %02d/%d.\n", mois, annee);
        }
    }
    fclose(f);
}

// Affichage des Revenus et des Charges 
void afficher_les_charges() {
    printf("\033[3J\033[H\033[2J"); 
    ensure_charges_fixes_current_month();
    ensure_revenus_fixes_current_month();
    int mois, annee;
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
    
    printf("==========================================\n");
    printf("  SYNTHESE DU MOIS : %s\n", mois_annee_formate);
    printf("==========================================\n");

    char line[256];
    char revenuLine[256];
    fgets(line, sizeof(line), f); // Lire l'en-tête
    fgets(revenuLine, sizeof(revenuLine), r); // Lire l'en-tête

    while (fgets(line, sizeof(line), f)) { 
        char *spc = " ";
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
            
            printf("\n%2sREVENUS%11s|%3sCHARGES FIXES\n", spc , spc, spc);
            printf("------------------------------------------\n"); 
            int bcl = nb_charges - nb_revenus > 0 ? nb_charges : nb_revenus;
            for (int i = 0; i < bcl; i++) {
                if (i < nb_revenus) {
                    printf("%-5s: %8.2f dhs | ", noms_revenus[i+2], revenus_lues[i]);
                }
                else {
                    printf("%-20s| ", spc);
                }
                if (i < nb_charges) {
                    printf("%-5s: %8.2f dhs\n", noms_charges[i+2], charges_lues[i]);
                }
                else {
                    printf("%-20s| ", spc);
                }
            }
            printf("------------------------------------------\n");
            double total_charges = 0.0;
            for (int i = 0; i < nb_charges; i++) {
                total_charges += charges_lues[i];
            }
            double total_revenus = 0.0;
            for (int i = 0; i < nb_revenus; i++) {
                total_revenus += revenus_lues[i];
            }
            printf("total: \033[1;36m%8.2f\033[0m dhs | Total: \033[1;36m%8.2f\033[0m dhs\n", total_revenus, total_charges);
            
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
                
                printf("------------------------------------------\n");
                printf("\n%2sDIVERS REVENUS%-4s|%2sDIVERS DEPENSES\n", spc, spc, spc);
                printf("------------------------------------------\n");
                printf("Nombr: 0%-12s| Nombr: %4d \n", spc, nb_depenses);
                printf("------------------------------------------\n");
                printf("Total: \033[1;36m0.0\033[0m dhs%-6s| Total: \033[1;36m%8.2f\033[0m dhs\n", spc, total_depenses_mois);
                printf("------------------------------------------\n");
                
                    
                // Calculer les dépenses du mois
                double total_lucky_mois = 0.0;

                // Total général (charges + dépenses)
                double total_general = total_charges + total_depenses_mois;
                double total_rgeneral = total_revenus + total_lucky_mois;

                printf("\n%2sTOTAL SOLDE%-7s|%2sTOTAL DEPENSES\n", spc, spc, spc);
                printf("------------------------------------------\n");
                printf("R + D: \033[1;33m%8.2f\033[0m dhs | ", total_rgeneral);
                printf("C + D: \033[1;33m%8.2f\033[0m dhs\n", total_general);
                printf("------------------------------------------\n");

                
                double reste_general = total_rgeneral - total_rgeneral;
                printf("\n%-20s|%2sSOLDE RESTANT\n", spc, spc); 
                printf("------------------------------------------\n");
                printf("T.SOLDE - T.DEPENSE:| \033[1;33m%8.2f\033[0m dhs\n", reste_general);
                printf("------------------------------------------\n");
                printf("==========================================\n");
            } else {
                printf("\n   Aucune depense ce mois.\n");
            }
            break;
        }
    }
    fclose(f);
}

// modifier a partir d'ici
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
    afficher_les_charges();
}

