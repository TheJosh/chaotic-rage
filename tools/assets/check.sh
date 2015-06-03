#!/bin/bash
#
# Finds all .blend files in the orig/ directory, and checks them
# for issues
#
set -e

# Drop into repository root
cd `dirname $0`
cd ../..

# Directoy where building will take place
mkdir -p orig-check

# For the coloured status fields
RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
NORMAL=$(tput sgr0)

# Take first argument as a filter on model name, if specified
if [ "$1" != "" ]; then
	MATCH="$1*.blend"
else
	MATCH="*.blend"
fi

# Find files in the source directory, and process into the destination directory
function process {
	for FILE in $(find $1 -name "$MATCH"); do
		echo "$FILE"
		
		rm -rf orig-check/*
		
		# Build
		ERROR=$((
			blender -b "$FILE" -P tools/assets/rebuild_blender.py -- "orig-check" >/dev/null
		) 2>&1)
		
		# Report build failures
		if [ "$ERROR" != "" ]; then
			echo "Build failure"
			echo
			echo $ERROR
			echo
			continue;
		fi
		
		# Check for spaces in any generated filenames
		ISSUE_SPACE_FILENAME=$(
			find "orig-check/" -name "* *"
		)
		if [ "$ISSUE_SPACE_FILENAME" != "" ]; then
			echo "$RED  Spaces in generated filenames  $NORMAL"
			echo "$ISSUE_SPACE_FILENAME" | sed "s|orig-check/|    |"
		fi
		
		# Check for images with the wrong base name
		BASENAME=$(basename -s .blend "$FILE")
		ISSUE_WRONG_BASENAME=$(
			find "orig-check" -type f -not -name "$BASENAME.*"
		)
		if [ "$ISSUE_WRONG_BASENAME" != "" ]; then
			echo "$RED  Incorrect basename in generated files  $NORMAL"
			echo "$ISSUE_WRONG_BASENAME" | sed "s|orig-check/|    |"
		fi
	done
}

# Do the processing
process "orig/doodads"
process "orig/misc"
process "orig/units"
process "orig/vehicles"
process "orig/weapons"

# Nuke our temp directory
rm -rf orig-check

