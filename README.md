# 💰 VOLA - Gestionnaire de Dépenses Personnelles

Application en langage C pour suivre et analyser vos dépenses financières.

## 📋 Fonctionnalités

- ✅ Ajout de dépenses avec date et heure automatiques
- 📊 Affichage de l'historique complet
- 📅 Analyse des dépenses par mois
- 📆 Analyse des dépenses par semaine
- ⚠️ Alertes de dépassement de budget (100 dhs/semaine)
- 💾 Sauvegarde automatique dans des fichiers

## 🛠️ Compilation

### Sous Windows (MinGW)

```bash
mingw32-make clean
mingw32-make
.\vola
```

### Sous iOS (ISH)

```bash
make
./vola
```

# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

The binary must be run from the project root so relative paths to `data/` resolve correctly.

## Deploy script

`bin/deploy` pulls the latest code, rebuilds, and replaces the binary one level up:

```bash
cd VOLA && git pull && make && rm ../vola && mv vola ../vola && cd .. && ./vola
```

## Architecture

VOLA is a terminal-based personal finance manager written in C99. All source lives in two places:

- **`vola.c`** — entry point; menu loop dispatches to module functions
- **`modules/`** — one `.c`/`.h` pair per domain

### Module responsibilities

| Module     | Role                                                                                                                                                                                                                                             |
| ---------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| `utils`    | Shared constants (`MAX_ITEMS=50`, file paths), date/time helpers, UI formatting (`ui_ux_design`), UI feedback helpers (`choix_invalide`, `libelle_vide`, `donnee_vide`, `annuler`), global `spc` spacer                                          |
| `data`     | Reads/writes `data/data.csv` (the schema); loads fixed charges and revenues into global arrays; carries over the previous month's remaining balance into the `epargne` revenue slot; hidden admin functions for add/modify/delete of fixed items |
| `depenses` | Add and display variable expenses; syncs to `data/depenses.csv`                                                                                                                                                                                  |
| `casuel`   | Add and display occasional income; syncs to `data/casuel.csv`                                                                                                                                                                                    |
| `charges`  | Manages fixed charges; syncs to `data/charges_fixes.csv`                                                                                                                                                                                         |
| `revenus`  | Manages fixed revenues; syncs to `data/revenu_fixes.csv`                                                                                                                                                                                         |
| `bilan`    | Monthly summary: aggregates all four CSV files, computes per-week totals (weeks are day-ranges 1-7, 8-14, 15-21, 22-28, 29-31), and rolls previous-month balance into current savings                                                            |

### Hidden admin shortcuts (non visibles dans le menu)

Ces raccourcis sont saisis au prompt principal comme un choix ordinaire :

| Code | Action                    |
| ---- | ------------------------- |
| `11` | Ajouter une charge fixe   |
| `12` | Modifier une charge fixe  |
| `13` | Supprimer une charge fixe |
| `41` | Ajouter un revenu fixe    |
| `42` | Modifier un revenu fixe   |
| `43` | Supprimer un revenu fixe  |

Ces fonctions sont dans `modules/data.c`. Elles modifient `data/data.csv` (schema) **et** propagent le changement de colonnes vers `data/charges_fixes.csv` / `data/revenu_fixes.csv` (historique mensuel) via les helpers statiques `add_column_to_history` / `delete_column_from_history`. Le fichier `data/tmp.csv` est utilisé comme fichier temporaire pour toutes les réécritures atomiques.

### Data files (`data/`)

All persistence is plain CSV, read with `fgets`/`strtok`:

| File                | Format                                                                                               |
| ------------------- | ---------------------------------------------------------------------------------------------------- |
| `data.csv`          | Schema: fixed charge and revenue names + amounts (rewritten each run to bake in rolled-over savings) |
| `depenses.csv`      | `YYYY-MM-DD,HH:MM:SS,amount,label` per line                                                          |
| `casuel.csv`        | Same format as `depenses.csv`                                                                        |
| `charges_fixes.csv` | `mois,annee,val1,val2,...` one row per month                                                         |
| `revenu_fixes.csv`  | Same format as `charges_fixes.csv`                                                                   |

### Key global state (declared in `utils.h`, defined in modules)

- `charges[]`, `revenus[]` — float arrays of fixed amounts (max `MAX_ITEMS=50`)
- `noms_charges[]`, `noms_revenus[]` — dynamically allocated name strings
- `nb_charges`, `nb_revenus`, `nb_noms_charges`, `nb_noms_revenus` — counts
- `i_epargne` — index of the `epargne` slot in the revenues array (sentinel value `69` = not found)

### Savings rollover logic

On each startup `read_schema()` calls `get_reste_general_precedent()`, which reads all four CSV files for the **previous** month and returns the net balance. This value overwrites the `epargne` field in `data.csv` so the current month starts with last month's carry-forward.

## Compiler flags

`-Wall -std=c99 -Imodules` — headers are included with bare names (e.g. `#include "utils.h"`) thanks to `-Imodules`.
