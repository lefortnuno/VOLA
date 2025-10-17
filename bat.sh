#!/bin/sh
git fetch --all
git reset --hard origin/main
make
rm ../vola
mv vola ../vola
cd ..
./vola
