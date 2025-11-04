#!/bin/bash

set -e  # Arrêter le script en cas d'erreur
message="fix: Nombre a virgule pour Zelina"  

cd VOLA
git checkout dev
git pull origin dev 
make 
rm -f ../vola
mv vola ../vola 
git checkout main
rm -f vola
cd .. 
cp vola VOLA/
cd VOLA
git add vola
git commit -m "$message"
git push origin main 
cd ..
./vola
