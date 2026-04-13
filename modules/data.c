#include "bilan.h"
#include "charges.h"
#include "data.h"
#include "depenses.h"
#include "revenus.h"
#include "utils.h"

int i_epargne = 69;


static char *my_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy) memcpy(copy, s, len); 
    return copy;
}

void reset_data() {
    nb_charges = nb_noms_charges = 0;
    nb_revenus = nb_noms_revenus = 0;
    i_epargne = 69;
    for (int i = 0; i < MAX_ITEMS; i++) {
        charges[i] = revenus[i] = 0.0f;
        if (noms_charges[i]) { free(noms_charges[i]); noms_charges[i] = NULL; }
        if (noms_revenus[i]) { free(noms_revenus[i]); noms_revenus[i] = NULL; }
    }
}

void read_schema() {
    reset_data();

    FILE *f = fopen(DATA_FILE, "r");
    FILE *t = fopen(TMP_FILE, "w");
    if (!f) {
        perror("Erreur ouverture fichier data.csv");
        return;
    }
    
    double epargne_precedente = get_reste_general_precedent(); 

    char line[256];
    int mode = 0; // 1 = charges, 2 = revenus 

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0; // supprimer le \n

        if (strstr(line, "Charges Fixes")) {
            mode = 1;
            fprintf(t, "%s\n", line);
            continue;
        } else if (strstr(line, "Revenus Fixes")) {
            mode = 2;
            fprintf(t, "%s\n", line);
            continue;
        }

        if (mode == 1 && nb_noms_charges == 0 && strchr(line, ',')) {
            // noms des charges
            char *token = strtok(line, ","); 
            while (token && nb_noms_charges < MAX_ITEMS) { 
                fprintf(t, nb_noms_charges == 0 ? "%s" : ",%s", token);
                noms_charges[nb_noms_charges] = my_strdup(token);
                nb_noms_charges++;
                token = strtok(NULL, ",");
            }
            fprintf(t, "\n");
        } else if (mode == 1 && nb_charges == 0 && strchr(line, ',')) {
            // valeurs des charges
            char *token = strtok(line, ","); 
            while (token && nb_charges < MAX_ITEMS) {
                fprintf(t, nb_charges == 0 ? "%s" : ",%s", token);
                charges[nb_charges] = atof(token);
                nb_charges++;
                token = strtok(NULL, ","); 
            }
            fprintf(t, "\n");
        } else if (mode == 2 && nb_noms_revenus == 0 && strchr(line, ',')) {
            // noms des revenus
            char *token = strtok(line, ","); 
            while (token && nb_noms_revenus < MAX_ITEMS) {
                fprintf(t, nb_noms_revenus == 0 ? "%s" : ",%s", token);
                noms_revenus[nb_noms_revenus] = my_strdup(token); 
                if(i_epargne == 69 && strcmp(token, "epargne") == 0) i_epargne = nb_noms_revenus; 
                nb_noms_revenus++;
                token = strtok(NULL, ",");
            }
            fprintf(t, "\n");
        } else if (mode == 2 && nb_revenus == 0 && strchr(line, ',')) {
            // valeurs des revenus
            char *token = strtok(line, ","); 
            while (token && nb_revenus < MAX_ITEMS) { 
                if(i_epargne != 69 && i_epargne == nb_revenus) fprintf(t, nb_revenus == 0 ? "%.2f" : ",%.2f", epargne_precedente);
                else fprintf(t, nb_revenus == 0 ? "%s" : ",%s", token);

                if(i_epargne != 69 && i_epargne == nb_revenus) revenus[nb_revenus] = epargne_precedente;
                else revenus[nb_revenus] = atof(token);  

                nb_revenus++;
                token = strtok(NULL, ","); 
            }
            fprintf(t, "\n");
        }
    }
    fclose(f); 
    fclose(t); 
    
    // REMPLACE DATA_FILE par TMP_FILE
    remove(DATA_FILE); 
    rename(TMP_FILE, DATA_FILE); 
}
 
void init_data() {
    read_schema();
    
    if (nb_charges == 0 || nb_revenus == 0) {
        printf("⚠️ Données manquantes dans %s\n", DATA_FILE);
    }
}

void update_charge() {
    mise_a_jour();
    init_data();
}

void update_revenu() {
    mise_a_jour();
    init_data();
}

// ============================================================
// HELPERS INTERNES
// ============================================================

static void rewrite_data_csv() {
    FILE *f = fopen(DATA_FILE, "w");
    if (!f) { perror("Erreur ecriture data.csv"); return; }

    fprintf(f, "Charges Fixes\n");
    for (int i = 0; i < nb_noms_charges; i++)
        fprintf(f, i == 0 ? "%s" : ",%s", noms_charges[i]);
    fprintf(f, "\n");
    for (int i = 0; i < nb_charges; i++)
        fprintf(f, i == 0 ? "%.2f" : ",%.2f", charges[i]);
    fprintf(f, "\n");

    fprintf(f, "Revenus Fixes\n");
    for (int i = 0; i < nb_noms_revenus; i++)
        fprintf(f, i == 0 ? "%s" : ",%s", noms_revenus[i]);
    fprintf(f, "\n");
    for (int i = 0; i < nb_revenus; i++)
        fprintf(f, i == 0 ? "%.2f" : ",%.2f", revenus[i]);
    fprintf(f, "\n");

    fclose(f);
}

/* Ajoute une colonne 0.00 a la fin de chaque ligne de donnees */
static void add_column_to_history(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    FILE *t = fopen(TMP_FILE, "w");
    if (!t) { fclose(f); return; }

    char line[512];
    if (fgets(line, sizeof(line), f))
        fprintf(t, "%s", line); /* entete inchangee */

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        fprintf(t, "%s,0.00\n", line);
    }

    fclose(f); fclose(t);
    remove(filename); rename(TMP_FILE, filename);
}

/* Supprime la colonne col_idx (0-base, dans les colonnes de donnees) */
static void delete_column_from_history(const char *filename, int col_idx) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    FILE *t = fopen(TMP_FILE, "w");
    if (!t) { fclose(f); return; }

    char line[512], copy[512];
    if (fgets(line, sizeof(line), f))
        fprintf(t, "%s", line); /* entete inchangee */

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        strncpy(copy, line, sizeof(copy) - 1);
        copy[sizeof(copy) - 1] = 0;

        char *token = strtok(copy, ",");
        int idx = 0, written = 0;
        while (token) {
            if (idx != col_idx + 2) { /* +2 pour sauter mois,annee */
                if (written > 0) fprintf(t, ",");
                fprintf(t, "%s", token);
                written++;
            }
            token = strtok(NULL, ",");
            idx++;
        }
        fprintf(t, "\n");
    }

    fclose(f); fclose(t);
    remove(filename); rename(TMP_FILE, filename);
}

/* Vide le buffer stdin (apres un scanf) */
static void clear_buf() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static void show_charges() {
    ui_ux_design("=");
    printf("||  N.  %-20s   VALEUR\n", "LIBELLE");
    ui_ux_design("-");
    for (int i = 0; i < nb_noms_charges; i++)
        printf("||%3d. %-20.20s : %8.2f dhs\n", i+1, noms_charges[i], charges[i]);
    ui_ux_design("-");
}

static void show_revenus() {
    ui_ux_design("=");
    printf("||  N.  %-20s   VALEUR\n", "LIBELLE");
    ui_ux_design("-");
    for (int i = 0; i < nb_noms_revenus; i++)
        printf("||%3d. %-20.20s : %8.2f dhs\n", i+1, noms_revenus[i], revenus[i]);
    ui_ux_design("-");
}

// ============================================================
// GESTION CHARGES FIXES
// ============================================================

void add_charge_fixe() {
    printf("\033[3J\033[H\033[2J");
    init_data();
    printf("\n%-5sAJOUT - CHARGES FIXES\n", spc);
    show_charges();

    if (nb_noms_charges >= MAX_ITEMS) {
        printf("Limite atteinte (%d items).\n", MAX_ITEMS);
        return;
    }

    char nom[MAX_NAME_LEN], val_str[50];
    clear_buf();
    printf("|| AJOUT Charge Fixe  \n");
    printf("|| Nom : "); fflush(stdout);
    if (!fgets(nom, sizeof(nom), stdin)) return;
    nom[strcspn(nom, "\n")] = 0;
    if (!strlen(nom)) { libelle_vide(); return; }

    printf("|| Valeur [0.00 dhs] : "); fflush(stdout);
    if (!fgets(val_str, sizeof(val_str), stdin)) return;
    val_str[strcspn(val_str, "\n")] = 0;
    float val = strlen(val_str) ? (float)atof(val_str) : 0.0f;

    noms_charges[nb_noms_charges] = my_strdup(nom);
    charges[nb_charges] = val;
    nb_noms_charges++; nb_charges++;

    rewrite_data_csv();
    add_column_to_history(CHARGES_FIXES_FILE);

    printf("\033[1;32m");
    ui_ux_design("=");
    printf("||  C.F '%s':[%.2f dhs] enregistree.\n", nom, val);
    ui_ux_design("=");
    printf("\033[0m");
}

void modifier_charge_fixe() {
    printf("\033[3J\033[H\033[2J");
    init_data();
    if (!nb_noms_charges) { donnee_vide(); return; }

    printf("%-5sMODIFIER - CHARGES FIXES\n", spc);
    show_charges();

    char line[50];
    clear_buf();
    printf("|| MODIFICATION N.(1-%d) : ", nb_noms_charges); fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) return;
    int idx = atoi(line) - 1;
    if (idx < 0 || idx >= nb_noms_charges) { choix_invalide(); return; }

    char nom[MAX_NAME_LEN], val_str[50];
    printf("|| Nom '%s' (Entree=Garder) : ", noms_charges[idx]); fflush(stdout);
    if (!fgets(nom, sizeof(nom), stdin)) return;
    nom[strcspn(nom, "\n")] = 0;

    printf("|| Valeur [%.2f] (Entree=Garder) : ", charges[idx]); fflush(stdout);
    if (!fgets(val_str, sizeof(val_str), stdin)) return;
    val_str[strcspn(val_str, "\n")] = 0;

    if (strlen(nom)) { free(noms_charges[idx]); noms_charges[idx] = my_strdup(nom); }
    if (strlen(val_str)) charges[idx] = (float)atof(val_str);

    rewrite_data_csv();

    printf("\033[1;32m");
    ui_ux_design("=");
    printf("|| C.F N.%d | '%s':[%.2f dhs] enregistree.\n", idx+1, noms_charges[idx], charges[idx]);
    ui_ux_design("=");
    printf("\033[0m");
}

void supprimer_charge_fixe() {
    printf("\033[3J\033[H\033[2J");
    init_data();
    if (!nb_noms_charges) { donnee_vide(); return; }

    printf("%-5sSUPPRIMER - CHARGES FIXES\n", spc);
    show_charges();

    char line[50];
    clear_buf();
    printf("|| SUPPRESSION N.(1-%d) : ", nb_noms_charges); fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) return;
    int idx = atoi(line) - 1;
    if (idx < 0 || idx >= nb_noms_charges) { choix_invalide(); return; }

    char saved_nom[MAX_NAME_LEN];
    strncpy(saved_nom, noms_charges[idx], sizeof(saved_nom) - 1);
    saved_nom[sizeof(saved_nom) - 1] = 0;

    printf("|| Supprimer '%s':[%.2f dhs] ? (O/N) : ", saved_nom, charges[idx]); fflush(stdout);
    char confirm[10];
    if (!fgets(confirm, sizeof(confirm), stdin)) return;
    if (confirm[0] != 'o' && confirm[0] != 'O') { annuler(); return; }

    free(noms_charges[idx]);
    for (int i = idx; i < nb_noms_charges - 1; i++) {
        noms_charges[i] = noms_charges[i+1];
        charges[i] = charges[i+1];
    }
    noms_charges[nb_noms_charges-1] = NULL;
    charges[nb_charges-1] = 0.0f;
    nb_noms_charges--; nb_charges--;

    rewrite_data_csv();
    delete_column_from_history(CHARGES_FIXES_FILE, idx);

    printf("\033[1;31m");
    ui_ux_design("=");
    printf("|| Charge Fixe '%s' supprimee.\n", saved_nom);
    ui_ux_design("=");
    printf("\033[0m");
}

// ============================================================
// GESTION REVENUS FIXES
// ============================================================

void add_revenu_fixe() {
    printf("\033[3J\033[H\033[2J");
    init_data();
    printf("\n%-5sAJOUT - REVENUS FIXES\n", spc);
    show_revenus();

    if (nb_noms_revenus >= MAX_ITEMS) {
        printf("Limite atteinte (%d items).\n", MAX_ITEMS);
        return;
    }

    char nom[MAX_NAME_LEN], val_str[50];
    clear_buf();
    printf("|| AJOUT Revenu Fixe  \n");
    printf("|| Nom : "); fflush(stdout);
    if (!fgets(nom, sizeof(nom), stdin)) return;
    nom[strcspn(nom, "\n")] = 0;
    if (!strlen(nom)) { donnee_vide(); return; }

    printf("|| Valeur [0.00 dhs] : "); fflush(stdout);
    if (!fgets(val_str, sizeof(val_str), stdin)) return;
    val_str[strcspn(val_str, "\n")] = 0;
    float val = strlen(val_str) ? (float)atof(val_str) : 0.0f;

    noms_revenus[nb_noms_revenus] = my_strdup(nom);
    revenus[nb_revenus] = val;
    nb_noms_revenus++; nb_revenus++;

    rewrite_data_csv();
    add_column_to_history(REVENUS_FIXES_FILE);

    printf("\033[1;32m");
    ui_ux_design("=");
    printf("|| R.F '%s':[%.2f dhs] enregistree.\n", nom, val);
    ui_ux_design("=");
    printf("\033[0m");
}

void modifier_revenu_fixe() {
    printf("\033[3J\033[H\033[2J");
    init_data();
    if (!nb_noms_revenus) { donnee_vide(); return; }

    printf("%-5sMODIFIER - REVENUS FIXES\n", spc);
    show_revenus();

    char line[50];
    clear_buf();
    printf("|| MODIFICATION N.(1-%d) : ", nb_noms_revenus); fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) return;
    int idx = atoi(line) - 1;
    if (idx < 0 || idx >= nb_noms_revenus) { choix_invalide(); return; }

    char nom[MAX_NAME_LEN], val_str[50];
    printf("|| Nom '%s' (Entree=Garder) : ", noms_revenus[idx]); fflush(stdout);
    if (!fgets(nom, sizeof(nom), stdin)) return;
    nom[strcspn(nom, "\n")] = 0;

    printf("|| Valeur [%.2f] (Entree=Garder) : ", revenus[idx]); fflush(stdout);
    if (!fgets(val_str, sizeof(val_str), stdin)) return;
    val_str[strcspn(val_str, "\n")] = 0;

    if (strlen(nom)) { free(noms_revenus[idx]); noms_revenus[idx] = my_strdup(nom); }
    if (strlen(val_str)) revenus[idx] = (float)atof(val_str);

    rewrite_data_csv();

    printf("\033[1;32m");
    ui_ux_design("=");
    printf("|| R.F N.%d | '%s':[%.2f dhs] enregistree.\n", idx+1, noms_revenus[idx], revenus[idx]);
    ui_ux_design("=");
    printf("\033[0m");
}

void supprimer_revenu_fixe() {
    printf("\033[3J\033[H\033[2J");
    init_data();
    if (!nb_noms_revenus) { donnee_vide(); return; }

    printf("%-5sSUPPRIMER - REVENUS FIXES\n", spc);
    show_revenus();

    char line[50];
    clear_buf();
    printf("|| SUPPRESSION N.(1-%d) : ", nb_noms_revenus); fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) return;
    int idx = atoi(line) - 1;
    if (idx < 0 || idx >= nb_noms_revenus) { choix_invalide(); return; }

    char saved_nom[MAX_NAME_LEN];
    strncpy(saved_nom, noms_revenus[idx], sizeof(saved_nom) - 1);
    saved_nom[sizeof(saved_nom) - 1] = 0;

    printf("|| Supprimer '%s':[%.2f dhs] ? (O/N) : ", saved_nom, revenus[idx]); fflush(stdout);
    char confirm[10];
    if (!fgets(confirm, sizeof(confirm), stdin)) return;
    if (confirm[0] != 'o' && confirm[0] != 'O') { annuler(); return; }

    free(noms_revenus[idx]);
    for (int i = idx; i < nb_noms_revenus - 1; i++) {
        noms_revenus[i] = noms_revenus[i+1];
        revenus[i] = revenus[i+1];
    }
    noms_revenus[nb_noms_revenus-1] = NULL;
    revenus[nb_revenus-1] = 0.0f;
    nb_noms_revenus--; nb_revenus--;

    rewrite_data_csv();
    delete_column_from_history(REVENUS_FIXES_FILE, idx);

    printf("\033[1;31m");
    ui_ux_design("=");
    printf("|| Revenu Fixe '%s' supprime.\n", saved_nom);
    ui_ux_design("=");
    printf("\033[0m");
}
 