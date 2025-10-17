#!/bin/sh
git fetch --all
git reset --hard origin/main
make
mv vola ../vola
cd ..
./vola
