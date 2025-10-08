 
#include "modules/depenses.h" 
#include "modules/charges.h" 
#include "modules/utils.h" 

int main() { 
    int choix = 0; 
    time_t maintenant = time(NULL);
    do {
        printf("\n  ================= GESTION DEPENSES ================\n");
        printf("|| 1.Ajouter ");
        printf("|| 2.Historique ");
        printf("|| 3.Totaux "); 
        printf("|| 0.Quitter ||");
        printf("\n  ===================================================\n");
        printf("|| Choix: "); 
        fflush(stdout); 
        scanf("%d", &choix);

        switch (choix) {
            case 1: add_depense(); break;
            case 2: afficher_depenses(maintenant); break;  
            case 3: afficher_les_charges(); break;

            case 0:
                printf("\033[1;32m"); // vert vif
                printf("\n  ===========================================\n");
                printf("||   Merci d avoir utilise le programme !  ||\n"); 
                printf("||             A bientot :)                ||\n");
                printf("  ===========================================\n\n");
                printf("\033[0m"); // réinitialise la couleur
                break;


            default:
                choix_invalide();
                break;

        }
    } while (choix != 0);
 
    return 0;
}
