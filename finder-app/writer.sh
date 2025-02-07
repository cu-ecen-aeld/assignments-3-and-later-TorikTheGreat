#!/bin/sh

# Load parameters
writefile=$1
writestr=$2

# Check whether both parameters were input
if [ -z "$writefile" ] || [ -z "$writefile" ]; then
 echo "Missing parameters. Exitting..."
 exit 1

fi

# Check if directory exists, if not then create it
if ! [ -d "$(dirname $writefile)" ]; then
 mkdir -p "$(dirname $writefile)"
fi

# Write weitestr to file
echo "$writestr" > "$writefile"

# Print error message if file creation failed
if ! [ -e "$writefile" ]; then
 echo "File creation failed. Exitting..."
 exit 1
fi
