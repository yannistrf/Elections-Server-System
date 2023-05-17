#!/bin/bash

# Check for args num
if [[ $# != 1 ]]; then
    echo "Usage: ./tallyVotes.sh [tallyResultsFile]"
    exit 1
fi

resultsFile=$1

inputFile="inputFile"

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
round=0
for token in $results;
do
    # when round % 2 = 1, token is a number 
    round=$((round+1))
    rem=$(($round % 2))
    if [ $rem -eq 1 ]; then
        votes=$token
        continue
    fi

    # else it's the name of the party
    # so we write the result to our file
    echo "$token $votes" >> $resultsFile
done
