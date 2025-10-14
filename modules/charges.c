#include "charges.h"
#include "utils.h"
// Stocker les données dans des arrays (accessible par d'autres fonctions)
float charges[] = {1150, 100, 160, 50, 0};
char *noms_charges[] = {"mois", "annee", "loyer", "wifi", "tram", "redal", "iruno"};
int nb_charges = sizeof(charges) / sizeof(charges[0]);
int nb_noms_charges = sizeof(noms_charges) / sizeof(noms_charges[0]);

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
    afficher_les_charges();
}

void afficher_les_charges() {
    printf("\033[2J\033[H"); 
    ensure_charges_fixes_current_month();
    int mois, annee;
    get_current_month_year(&mois, &annee);

    FILE *f = fopen(CHARGES_FIXES_FILE, "r");
    if (!f) { 
        printf("❌ Erreur lecture charges fixes.\n"); 
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
    fgets(line, sizeof(line), f); // Lire l'en-tête
    while (fgets(line, sizeof(line), f)) {
        int m, a;
        double charges_lues[nb_charges];
        char *token;
        int index = 0;

        token = strtok(line, ",");
        while (token != NULL && index < 2 + nb_charges) {
            if (index == 0) m = atoi(token);
            else if (index == 1) a = atoi(token);
            else if (index - 2 < nb_charges) charges_lues[index - 2] = atof(token);
            
            token = strtok(NULL, ",");
            index++;
        }
        if (m == mois && a == annee) {
            
            printf("\n  CHARGES FIXES\n");
            printf("------------------------------------------\n"); 
            for (int i = 0; i < nb_charges; i++) {
                printf("%-8s: %8.2f dhs\n", noms_charges[i+2], charges_lues[i]);
            }
            printf("------------------------------------------\n");
            double total_charges = 0.0;
            for (int i = 0; i < nb_charges; i++) {
                total_charges += charges_lues[i];
            }
            printf("Total    : \033[1;36m%8.2f\033[0m dhs\n", total_charges);
            
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
                
                printf("\n  CHARGES VARIABLES\n");
                printf("------------------------------------------\n");
                printf("Nombre   : %4d depenses\n", nb_depenses);
                printf("Total    : \033[1;36m%8.2f\033[0m dhs\n", total_depenses_mois);
                
                // Total général (charges + dépenses)
                double total_general = total_charges + total_depenses_mois;
                printf("\n  TOTAL GENERAL\n");
                printf("==========================================\n");
                printf("Fixes + Variables : \033[1;33m%8.2f\033[0m dhs\n", total_general);
                printf("==========================================\n");
            } else {
                printf("\n   Aucune depense ce mois.\n");
            }
            break;
        }
    }
    fclose(f);
}