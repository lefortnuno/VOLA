#include "charges.h"
#include "utils.h"

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
    fgets(line, sizeof(line), f);
    while (fgets(line, sizeof(line), f)) {
        int m, a;
        sscanf(line, "%d,%d", &m, &a);
        if (m == mois && a == annee) { found = 1; break; }
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
    if (!f) { printf("Erreur lecture charges fixes.\n"); return; }

    printf("\n=== DEPENSES DU MOIS ===\n");

    char line[256];
    fgets(line, sizeof(line), f); // Lire l'en-tête
    while (fgets(line, sizeof(line), f)) {
        int m, a;
        double loyer, wifi, tram, redal, iruno;
        sscanf(line, "%d,%d,%lf,%lf,%lf,%lf,%lf",
               &m, &a, &loyer, &wifi, &tram, &redal, &iruno);
        if (m == mois && a == annee) {
            char date_str[50];
            snprintf(line, sizeof(line), "%d-%02d-01", a, m);
            format_date_affichage(line, date_str, sizeof(date_str));
            printf("\n--- Charges Fixes ---\n");
            printf("Loyer : %.2f  dhs\nWifi : %.2f  dhs\nTram : %.2f  dhs\nRedal : %.2f  dhs\nIruno : %.2f  dhs\n",
                   loyer, wifi, tram, redal, iruno);
            double total_charges = loyer + wifi + tram + redal + iruno;
            printf("--- Totaux: %.2f  dhs\n", total_charges);
            
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
                
                printf("\n--- Charges Variables ---\n");
                printf("Totaux: %.2f  dhs (%d depenses)\n", total_depenses_mois, nb_depenses);
                
                // Total général (charges + dépenses)
                double total_general = total_charges + total_depenses_mois;
                printf("\n--- Total General ---\n");
                printf("Charges fixes + Variables: %.2f  dhs\n", total_general);
            } else {
                printf("\nAucune depense ce mois.\n");
            }
            break;
        }
    }
    fclose(f);
}