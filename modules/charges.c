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

void afficher_charges_fixes() {
    ensure_charges_fixes_current_month();
    int mois, annee;
    get_current_month_year(&mois, &annee);

    FILE *f = fopen(CHARGES_FIXES_FILE, "r");
    if (!f) { printf("Erreur lecture charges fixes.\n"); return; }

    char line[256];
    fgets(line, sizeof(line), f);
    while (fgets(line, sizeof(line), f)) {
        int m, a;
        double loyer, wifi, tram, redal, iruno;
        sscanf(line, "%d,%d,%lf,%lf,%lf,%lf,%lf",
               &m, &a, &loyer, &wifi, &tram, &redal, &iruno);
        if (m == mois && a == annee) {
            char date_str[50];
            snprintf(line, sizeof(line), "%d-%02d-01", a, m);
            format_date_affichage(line, date_str, sizeof(date_str));
            printf("\n=== CHARGES FIXES (%s) ===\n", date_str);
            printf("Loyer : %.2f\nWifi : %.2f\nTram : %.2f\nRedal : %.2f\nIruno : %.2f\n",
                   loyer, wifi, tram, redal, iruno);
            printf("Total charges fixes : %.2f\n\n",
                   loyer + wifi + tram + redal + iruno);
            break;
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
    afficher_charges_fixes();
}