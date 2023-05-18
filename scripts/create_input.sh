#!/bin/bash

# Check for args num
if [[ $# != 2 ]]; then
    echo "Usage: ./create_input.sh [politicalParties.txt] [numLines]"
    exit 1
fi

# Extract args
partiesFile=$1
numLines=$2
inputFile="inputFile"

# Check if the partiesFile exists
if [ ! -f $partiesFile ]; then
    echo "$partiesFile does not exist."
    exit 1
fi

# Check if the user gave a number
if ! [[ $numLines =~ ^[0-9]+$ ]]; then
    echo "Second argument must be a positive number."
    exit 1
fi

# Check if numLines > 0
if [[ $numLines -lt 1 ]]; then
    echo "Please enter a positive number."
    exit 1
fi


# Get number of parties (number of lines of partiesFile)
partiesNum=`wc -l < $partiesFile`

# If inputFile exists, delete it
if [ -f $inputFile ]; then
    rm -i $inputFile
fi


for (( lineNum=1; lineNum<=numLines; lineNum++ ));
do
    # Pick a random party
    partyNum=$(($RANDOM % partiesNum + 1))
    randParty=`sed -n "${partyNum}p" < $partiesFile`
    
    # Generate first and last name
    randLen=$(($RANDOM % 10 + 3))
    firstName=`cat /dev/urandom | tr -dc "a-z" | fold -w $randLen| head -n 1`
    randLen=$(($RANDOM % 10 + 3))
    lastName=`cat /dev/urandom | tr -dc "a-z" | fold -w $randLen| head -n 1`

    # Write the record in the inputFile
    echo "$firstName $lastName $randParty" >> $inputFile
done