#!/bin/bash
#
# Find 'copyright' files in orig/ and outputh them all into one big file.
# This can then be copy-pasted into the debian/copyright file.
#
set -e

# Drop into repository root
cd `dirname $0`
cd ../..

# Where to look for copyright files
PATHS="orig"

# Iterate copyright files and output the results
for FILE in $(find $PATHS -name "copyright"); do
	DIR=`echo "$FILE" | sed -e 's~/copyright~~'`
	
	# Main orig/ directory
	echo "Files: $DIR/*";
	
	# Generated .dae files
	find "$DIR" -name "*.blend" | sed 's/.blend/*/' | sed "s|$DIR|data/cr/models|" | xargs -n1 echo "      "
	
	# Copy these across from the copyright file
	grep "Copyright: " $FILE
	grep "License: " $FILE
	
	# Newline between sections
	echo
done

