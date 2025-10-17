#include "depenses.h"
#include "utils.h"
#include "charges.h"
#include <ctype.h>

void add_depense() {
    char input[50];
    printf("|| Saisissez vos depenses (entrez 'q' pour quitter):\n");
    while (1) {
        printf("|| Montant: ");
        fflush(stdout); 
        scanf("%s", input);
        if (tolower(input[0]) == 'q') {
            printf("✅ Fin de saisie.\n");
            
            // Ajouter cette ligne pour passer la date actuelle
            time_t maintenant = time(NULL);
            afficher_depenses(maintenant);

            break;
        }
        double montant = atof(input);
        if (montant <= 0) {
            printf("⚠️ Montant invalide.\n");
            continue;
        }

        char date[20], heure[20];
        get_current_date(date, sizeof(date));
        get_current_time(heure, sizeof(heure));

        FILE *f = fopen(DEPENSES_FILE, "a");
        if (!f) { perror("Erreur ouverture fichier depenses"); return; }
        fprintf(f, "%s,%s,%.2f\n", date, heure, montant);
        fclose(f);

        printf("→ %.2f enregistree.\n", montant);
    }
}


void afficher_depenses(time_t date_ref) {
    time_t date_now = date_ref;
    int choix_semaine;
    do { 
        printf("\033[3J\033[H\033[2J"); 

        struct tm tm_ref = *localtime(&date_ref);
        int wday = tm_ref.tm_wday; // dimanche=0
        int decalage_lundi = (wday == 0) ? -6 : 1 - wday;
        struct tm tm_debut = tm_ref;
        tm_debut.tm_mday += decalage_lundi;
        tm_debut.tm_hour = 0;
        tm_debut.tm_min = 0;
        tm_debut.tm_sec = 0;
        mktime(&tm_debut); // normalise la date

        struct tm tm_fin = tm_debut;
        tm_fin.tm_mday += 6;
        tm_fin.tm_hour = 23;
        tm_fin.tm_min = 59;
        tm_fin.tm_sec = 59;
        mktime(&tm_fin);

        // Affichage de la semaine en titre  
        char debut_str[20], fin_str[20], mois_annee_str[20];
        char debut_formate[50], fin_formate[50], mois_annee_formate[50];

        // Vérifier si début et fin sont dans le même mois
        strftime(debut_str, sizeof(debut_str), "%Y-%m", &tm_debut);
        strftime(fin_str, sizeof(fin_str), "%Y-%m", &tm_fin);

        printf("===============================================\n");
        printf("  DEPENSE DE LA SEMAINE ");

        if (strcmp(debut_str, fin_str) == 0) { 
            char jour_debut[3], jour_fin[3];
            strftime(jour_debut, sizeof(jour_debut), "%d", &tm_debut);
            strftime(jour_fin, sizeof(jour_fin), "%d", &tm_fin);
             
            strftime(mois_annee_str, sizeof(mois_annee_str), "%Y-%m-%d", &tm_fin);
            format_date_affichage(mois_annee_str, mois_annee_formate, sizeof(mois_annee_formate));
             
            char *mois_annee_only = strchr(mois_annee_formate, ' ');
            if (mois_annee_only != NULL) {
                mois_annee_only++; // sauter le premier espace
            } else {
                mois_annee_only = mois_annee_formate;
            }
              
            printf("%s -> %s \033[1;36m%s\033[0m \n", jour_debut, jour_fin, mois_annee_only); 
            
        } else {
            // Mois différents : format complet pour les deux
            strftime(debut_str, sizeof(debut_str), "%Y-%m-%d", &tm_debut);
            strftime(fin_str, sizeof(fin_str), "%Y-%m-%d", &tm_fin);
            
            format_date_affichage(debut_str, debut_formate, sizeof(debut_formate));
            format_date_affichage(fin_str, fin_formate, sizeof(fin_formate));
             
            printf("%s -> %s\n",  debut_formate, fin_formate);
        }
        printf("==============================================\n\n");
        printf("    DATE     │ HEURE │   MONTANT  \n");
        printf("----------------------------------------------\n");



        // Lecture fichier
        FILE *f = fopen(DEPENSES_FILE, "r");
        if (!f) { 
            printf("  Aucune depense enregistree.\n"); 
            return; 
        }

        char line[256]; 
        double total = 0.0;
        int depenses_trouvees = 0;
        
        while (fgets(line, sizeof(line), f)) {
            char date[20], heure[20]; 
            double montant;
            
            // Lecture du format CSV : AAAA-MM-JJ,HH:MM:SS,MONTANT
            if (sscanf(line, "%10[^,],%8[^,],%lf", date, heure, &montant) == 3) {
                struct tm tm_dep = {0};
                // Conversion du format AAAA-MM-JJ
                sscanf(date, "%d-%d-%d", &tm_dep.tm_year, &tm_dep.tm_mon, &tm_dep.tm_mday);
                tm_dep.tm_mon -= 1; // Les mois vont de 0 à 11
                tm_dep.tm_year -= 1900; // Année depuis 1900
                
                // Conversion du format HH:MM:SS
                sscanf(heure, "%d:%d:%d", &tm_dep.tm_hour, &tm_dep.tm_min, &tm_dep.tm_sec);
                
                time_t t_dep = mktime(&tm_dep);
                time_t t_debut = mktime(&tm_debut);
                time_t t_fin = mktime(&tm_fin);

                if (t_dep >= t_debut && t_dep <= t_fin) {
                    // Formatage pour l'affichage
                    char date_fmt[50], heure_fmt[20];
                    snprintf(date_fmt, sizeof(date_fmt), "%02d/%02d/%04d", 
                             tm_dep.tm_mday, tm_dep.tm_mon + 1, tm_dep.tm_year + 1900);
                    snprintf(heure_fmt, sizeof(heure_fmt), "%02d:%02d", 
                             tm_dep.tm_hour, tm_dep.tm_min);
                    
                    printf("  %-10s │ %-5s │ %8.2f dhs \n", date_fmt, heure_fmt, montant);
                    total += montant;
                    depenses_trouvees++;
                }
            }
        }
        fclose(f);

        if (depenses_trouvees == 0) {
            printf("   Aucune depense cette semaine.\n");
        } else {  
            printf("----------------------------------------------\n");
            printf("%-20s|  SYNTHESE GENERALE\n", spc);
            printf("----------------------------------------------\n");
            printf("Nombre:%-16s|  %4d depenses \n", spc, depenses_trouvees);
            printf("Total  :%-15s|  \033[1;36m%8.2f\033[0m dhs\n", spc, total);
            printf("----------------------------------------------\n");
        }
 
        // Menu navigation semaine avec validation
        int saisie_valide;
        do {
            printf("\n  ================================================\n");
            printf("|| 1.Precedent ");
            printf("|| 2.Actuel ");
            printf("|| 3.Suivant "); 
            printf("|| 0.Menu ||");
            printf("\n  ================================================\n");
            printf("|| Choix: "); 
            fflush(stdout); 
            
            if (scanf("%d", &choix_semaine) != 1) {
                // Si la saisie n'est pas un nombre, vider le buffer
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                saisie_valide = 0;
            } else {
                saisie_valide = (choix_semaine >= 0 && choix_semaine <= 3);
            }
            
            if (!saisie_valide) {
                choix_invalide();
            }
        } while (!saisie_valide);

        if (choix_semaine == 1) date_ref -= 7 * 24 * 60 * 60;  
        else if (choix_semaine == 2) date_ref = date_now; 
        else if (choix_semaine == 3) date_ref += 7 * 24 * 60 * 60;
    } while (choix_semaine != 0);
}
