#!/bin/bash

THIS_PATH="$(realpath "$0")"
THIS_DIR="$(dirname "$THIS_PATH")"

# Find all files in THIS_DIR which end in .ino, .cpp, etc., as specified
# in the regular expression just below
FILE_LIST="$(find "$THIS_DIR" | grep -E ".*(\.c|\.h)$")"

echo -e "Files found to format = \n\"\"\"\n$FILE_LIST\n\"\"\""

# Run for all the files

cpplint --filter=-legal/copyright,-whitespace,-readability/checking,-readability/todo --recursive $FILE_LIST
