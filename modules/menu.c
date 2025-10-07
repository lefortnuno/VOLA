 
#include "menu.h"
#include "depenses.h" 
#include "charges.h" 

void menu() {
    int choix;
    do {
        printf("\n===== GESTION DEPENSES =====\n");
        printf("1. Ajouter des depenses\n");
        printf("2. Afficher l'historique des depenses\n");
        printf("3. Afficher les charges fixes\n");
        printf("4. Modifier les charges fixes du mois\n");
        printf("5. Afficher total des depenses par mois\n");
        printf("6. Afficher total des depenses par semaine (limite 100 dhs)\n");
        printf("0. Quitter\n");
        printf("Choix: ");
        scanf("%d", &choix);

        switch (choix) {
            case 1: add_depense(); break;
            case 2: afficher_depenses(); break; 
            case 5: afficher_depenses_mensuelles(); break;
            case 3: afficher_charges_fixes(); break;
            case 4: set_charges_fixes(); break;
            case 6: afficher_depenses_hebdo(); break;
            case 0: printf("Au revoir 👋\n"); break;
            default: printf("Choix invalide.\n"); break;
        }
    } while (choix != 0);
}
