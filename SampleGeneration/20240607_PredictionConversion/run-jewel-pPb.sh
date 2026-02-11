#!/bin/bash

INPUT=/home/kdeverea/Jewel/jewel-2.4.0/eventfiles/pPb
OUTPUTBASE=output/pPb
NFILES=$1

if [ -z "$NFILES" ]; then
   NFILES=0
fi

i=0
for FILE in ${INPUT}/*.hepmc;
do
   if [ $NFILES -ne 0 ] && [ $i -ge $NFILES ]; then
      break
   fi

   BASENAME=$(basename "$FILE" .hepmc)

   OUTPUTNAME=$OUTPUTBASE/$BASENAME.root
   echo "converting $FILE to $OUTPUTNAME"

   ./ExecuteHepMC --Input $FILE --Output $OUTPUTNAME 

   i=$((i+1))
done
echo "conversion done"
