#! /bin/bash

dirName=$1
numFiles=$2
numDirs=$3
levels=$4

dirCount=0
paths=()

for ((i=0; i<$numDirs; i++))
do
    currentPath="$dirName"
    for ((j=0; j<$levels; j++))
    do
        NUMBER=$(cat /dev/urandom | tr -dc '0-9' | fold -w 256 | head -n 1 | sed -e 's/^0*//' | head --bytes 10)
        let "length = $NUMBER % 8 + 1"
        randomText=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $length | head -n 1)
        currentPath="$currentPath"/"$randomText"
        $(mkdir $currentPath)
        paths+=($currentPath)
        ((dirCount+=1))
        if [ $dirCount -eq $numDirs ]
        then
            break;
        fi
    done
    if [ $dirCount -eq $numDirs ]
    then
        break;
    fi
done

for ((i=0; i<$numFiles; i++))
do
    let "index = $i % $numDirs"
    path="${paths[$index]}"
    echo index:$index
    echo $path
    NUMBER=$(cat /dev/urandom | tr -dc '0-9' | fold -w 256 | head -n 1 | sed -e 's/^0*//' | head --bytes 10)
    let "length = $NUMBER % 8 + 1"
    randomText=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $length | head -n 1)
    filePath="$path"/"$randomText"
    let "kbCount = ($NUMBER % 128 + 1) * 1024"
    randomBytes=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w $kbCount | head -n 1)
    echo $randomBytes >> $filePath
done




NEW_UUID=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 4 | head -n 1)
