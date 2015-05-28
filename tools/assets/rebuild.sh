#!/bin/bash
#
# Find 'copyright' files in orig/ and outputh them all into one big file
# This can then be copy-pasted into debian/copyright
#
set -e

# Drop into repository root
cd `dirname $0`
cd ../..

# For the coloured status fields
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
NORMAL=$(tput sgr0)

# Find files in the source directory, and process into the destination directory
function process {
	for FILE in $(find $1 -name "*.blend"); do
		echo -n "$FILE "
	
		ERROR=$((
			blender -b "$FILE" -P tools/assets/rebuild_blender.py -- "$2" >/dev/null
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
}

# Do the processing
process "orig/doodads" "data-test/cr/models"
process "orig/misc" "data-test/cr/models"
process "orig/units" "data-test/cr/models"
process "orig/vehicles" "data-test/cr/models"
process "orig/weapons" "data-test/cr/models"
