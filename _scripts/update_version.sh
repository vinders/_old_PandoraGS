#!/bin/bash

# Identify version file
FILENAME="./build_version.txt"
if ! [ -e $FILENAME ]; then
    cd ..
    if ! [ -e $FILENAME ]; then
        echo "${FILENAME} not found in current directory nor in parent directory!"
        exit 1
    fi
fi

# Update version file
read -r FIRST_LINE<$FILENAME
echo $FIRST_LINE > $FILENAME
REVISION=$(git rev-list --count HEAD 2>&1)
echo $REVISION >> $FILENAME

exit 0
