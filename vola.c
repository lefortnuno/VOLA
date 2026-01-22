#include "modules/bilan.h" 
#include "modules/data.h" 
#include "modules/depenses.h" 
#include "modules/casuel.h" 
#include "modules/charges.h" 
#include "modules/utils.h"  

int choix = 0;

int main() {  
    time_t maintenant = time(NULL);
    
    int saisie_valide;
    do {
        do {
            printf("\n  ================ GESTION DEPENSES ===============\n");
            printf("|| 1.Ajouter ");
            printf("|| 2.Historiq ");
            printf("|| 3.Total "); 
            printf("|| 0.Quitter ||");
            printf("\n  =================================================\n");
            // printf("||%-6s 4.UpRevenus %-6s", spc, spc);
            // printf("||%-4s 5.UpCharges %-5s||", spc, spc);
            printf("||%-6s 4.Casuel-A  %-6s", spc, spc);
            printf("||%-4s 5.Casuel-H  %-5s||", spc, spc);
            printf("\n  =================================================\n");
            printf("|| Choix: "); 
            fflush(stdout); 

            if (scanf("%d", &choix) != 1) { 
                int c;
                while ((c = getchar()) != '\n' && c != EOF);
                saisie_valide = 0;
            } else {
                saisie_valide = (choix >= 0 && choix <= 5);
            }
            
            if (!saisie_valide) {
                choix_invalide();
            }
        } while (!saisie_valide); 

        switch (choix) {
            case 1: add_depense(); break;
            case 2: afficher_depenses(maintenant); break;  
            case 3: 
                printf("\n\033[3J\033[H\033[2J");
                afficher_bilan(); 
                break;

            case 4: 
                add_casuel();  
                // printf("\n\033[3J\033[H\033[2J");
                // update_revenu(); 
                break;
                
            case 5: 
                afficher_casuels(maintenant);
                // printf("\n\033[3J\033[H\033[2J");
                // update_charge(); 
                break;

            case 0:
                choix_de_quitter();
                break;  
            default:
                choix_invalide();
                break;

        }
    } while (choix != 0);
 
    return 0;
}
