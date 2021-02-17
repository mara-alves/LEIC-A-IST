#!/bin/bash

inputdir=$1
outputdir=$2
maxthreads=$3

# check if input and output directories exit
if [ ! -d "$inputdir" ]
then
    echo "Error: Invalid input directory"
    exit
fi

if [ ! -d "$outputdir" ]
then
    echo "Error: Invalid output directory"
    exit
fi

# make sure the number of threads is 1 or more
if [ "$maxthreads" -lt 1 ]
then
    echo "Error: Invalid number of threads"
    exit
fi

for file in "$inputdir"/*.txt
do
    filename=${file##*/}    #remove directory from the file name
    filename=${filename%.*} #remove .txt from the name
    for i in $(seq 1 "$maxthreads")
    do
        echo InputFile="$file" NumThreads="$i"
        ./tecnicofs "$file" "$outputdir"/"$filename"-"$i".txt "$i" | grep "TecnicoFS"
    done
done