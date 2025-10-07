 
#include "modules/depenses.h" 
#include "modules/charges.h" 

int main() { 
    int choix;
    do {
        printf("\n  ================= GESTION DEPENSES ================\n");
        printf("|| 1.Ajouter ");
        printf("|| 2.Historique ");
        printf("|| 3.Fixes "); 
        printf("|| 0.Quitter ||");
        printf("\n  ===================================================\n");
        printf("|| Choix: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: add_depense(); break;
            case 2: afficher_depenses(); break; 
            case 5: afficher_depenses_mensuelles(); break; 

            case 0:
                printf("\033[1;32m"); // vert vif
                printf("\n  ===========================================\n");
                printf("||   Merci d avoir utilise le programme !  ||\n"); 
                printf("||             A bientot :)               ||\n");
                printf("  ===========================================\n\n");
                printf("\033[0m"); // réinitialise la couleur
                break;


            default:
                printf("\033[1;31m"); // rouge vif
                printf("\n  ------------------------------------------\n");
                printf("||    Choix invalide, veuillez reessayer ! ||\n");
                printf("  ------------------------------------------\n");
                printf("\033[0m"); // réinitialise la couleur
                break;

        }
    } while (choix != 0);
 
    return 0;
}
