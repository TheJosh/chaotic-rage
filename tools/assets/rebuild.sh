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

# For the right-aligned status fields
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
NORMAL=$(tput sgr0)

# Iterate copyright files and output the results
for FILE in $(find $PATHS -name "*.blend"); do
	echo -n "$FILE "
	
	ERROR=$((
		blender -b "$FILE" -P tools/assets/rebuild_blender.py >/dev/null
	) 2>&1)
	
	if [ "$ERROR" == "" ]; then
		echo " $GREEN[ OK ]$NORMAL"
	else
		echo " $RED[FAIL]$NORMAL"
		echo
		echo "$ERROR"
		echo
	fi
done

