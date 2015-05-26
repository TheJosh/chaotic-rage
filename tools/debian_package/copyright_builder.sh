#!/bin/bash
#
# Find 'copyright' files in orig/ and outputh them all into one big file
# This can then be copy-pasted into debian/copyright
#
set -e

# Drop into repository root
cd `dirname $0`
cd ../..

# Where to look for copyright files
PATHS="orig"

# Iterate copyright files and output the results
for FILE in $(find $PATHS -name "copyright"); do
	DIR=`echo "$FILE" | sed -e 's~/copyright~/*~'`
	
	echo "Files: $DIR";
	grep "License: " $FILE
	grep "Copyright: " $FILE
	echo
done

