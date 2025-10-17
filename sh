#!/bin/sh
git config --global user.name "lefortnuno"
git config --global user.email "trofelnuno@gmail.com" 
git fetch --all
git reset --hard origin/main
make
rm data/charges_fixes.csv
rm ../vola
mv vola ../vola
cd ..
./vola
