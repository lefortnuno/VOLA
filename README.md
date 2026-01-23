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

## GTK

### windows 11

https://www.msys2.org/
pacman -Syu
pacman -S mingw-w64-x86_64-gtk3
gcc ui/main_ui.c modules/*.c -o vola.exe `pkg-config --cflags --libs gtk+-3.0`
./vola.exe
