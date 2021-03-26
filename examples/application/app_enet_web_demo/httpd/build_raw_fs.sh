#!/bin/bash

rm -rf fs-build
cp -a fs fs-build
cd fs-build
rm -rf *.php */*.php

echo "step 3"
filelist=`ls *.html`
for file in $filelist
do
	echo "handling ${file}"
	gzip -9nv ${file}
	mv ${file}.gz ${file}
done

cd ..

./makefsdata.exe fs-build -e

rm -f fsdata_custom.h
mv fsdata.c fsdata_custom.h
