#!/bin/sh

svn st > modifyfiles.txt
vim modifyfiles.txt
mkdir -p zzdiffpatch
cp -f modifyfiles.txt zzdiffpatch/
sed -i 's/^.\ \{7\}//g' modifyfiles.txt

for line in `cat modifyfiles.txt`
do
    filepath=${line%/*}

    svn info $line 1>/dev/null 2>/dev/null
    if [ $? = 0 ]
    then
        mkdir -p zzdiffpatch/old/$filepath
        svn cat $line > zzdiffpatch/old/$line
    fi

    mkdir -p zzdiffpatch/new/$filepath
    cp -af $line zzdiffpatch/new/$filepath
done

rm -f modifyfiles.txt
