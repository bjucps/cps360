#!/bin/bash

. ~/macros/banner.sh "Project 1"
. ~/macros/assert_file.sh README.md Makefile 
. ~/macros/at_least_one_file.sh my-cat.c my-grep.c my-zip.c my-unzip.c

for FILE in my-cat my-grep my-zip my-unzip; do
  if [ -e $FILE ]; then
    rm $FILE
  fi
done

echo Executing makefile...
echo -n "Result: "
if stderr=$(make 2>&1); then
   echo "Compile succeeded"
   echo
else
   echo "FAIL"
   echo "----------------------------------------------------------------"
   echo "$stderr"
   echo "----------------------------------------------------------------"
   echo
   exit 1
fi

. ~/macros/at_least_one_file.sh my-cat my-grep my-zip my-unzip

cp -r ~/cps360/project1_files .

python3 ~/cps360/project1.py | tee results.log

