#!/bin/sh

# Load parameters
filesdir=$1
searchstr=$2

# Check that both parameters were passed
if [ -z "$filesdir" ] || [ -z "$searchstr" ]; then
    echo "Missing parameters. Exitting..."
    exit 1

# Check whether filesdir contains valid path
elif ! [ -d "$filesdir" ]; then
    echo "filesdir path not found. Exitting..."
    exit 1

fi

# Get the number of files in filesdir
file_count=$(find "$filesdir" -type f | wc -l)

# Get the number of matching lines
match_count=$(grep -r -c "$searchstr" "$filesdir" | awk -F: '{sum +=$2} END {print sum}') 

echo "The number of files are $file_count and the number of matching lines are $match_count"
