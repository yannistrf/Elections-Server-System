#!/bin/bash

# Check for args num
if [[ $# != 1 ]]; then
    echo "Usage: ./processLogFile.sh [poll-log]"
    exit 1
fi

inputFile=$1
resultsFile="pollerResultsFile"

# Check if file exists and if it does if we can read it
if [ ! -f $inputFile ] || [ ! -r $inputFile ]; then
    echo "Can't read $inputFile."
    exit 1
fi

# If there is already a resultsFile, delete it
if [ -f $resultsFile ]; then
    rm -i $resultsFile
fi


# change column order (party firstName lastName), sort as of the name (-s to not change order in case of same name),
# keep only the first vote from every voter, get the column with the parties, sort it, count every party appearance
results=`awk '{print $3, $1, $2}' $inputFile | sort -k 2,3 -s | uniq -f 1 | cut -f1 -d " " | sort | uniq -c`


# results looks like this: 10 SYRIZA 12 KKE 5 MERA25
# we want to add new lines and reverse the order
while read num party; do
    echo "$party $num" >> $resultsFile
done <<< $results