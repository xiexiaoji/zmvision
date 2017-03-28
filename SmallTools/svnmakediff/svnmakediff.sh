#!/bin/sh

svn st > modifyfiles.txt
vim modifyfiles.txt
mkdir -p zzdiffpatch
cp -f modifyfiles.txt zzdiffpatch/
mkdir -p zzdiffpatch/old
mkdir -p zzdiffpatch/new
sed -i 's/^.\ \{2\}.\ \{4\}//g' modifyfiles.txt

for line in `cat modifyfiles.txt`
do
    filepath=${line%/*}

    svn info $filepath 1>/dev/null 2>/dev/null
    if [ $? = 0 ]
    then
		if [ $filepath != $line ]
		then
			mkdir -p zzdiffpatch/old/$filepath
		fi
    fi

    svn info $line 1>/dev/null 2>/dev/null
    if [ $? = 0 ]
    then
		if [ $filepath != $line ]
		then
			mkdir -p zzdiffpatch/old/$filepath
		fi
        svn cat $line > zzdiffpatch/old/$line
    fi

	if [ $filepath != $line ]
	then
		mkdir -p zzdiffpatch/new/$filepath
	fi
    cp -af $line zzdiffpatch/new/$filepath
done

rm -f modifyfiles.txt
