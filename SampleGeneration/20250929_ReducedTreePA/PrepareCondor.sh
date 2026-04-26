mkdir -p Log

SubmitFile=SubmitPA.condor

echo "Universe              = vanilla" > $SubmitFile
echo "Executable            = $PWD/Condor.sh" >> $SubmitFile
echo "should_transfer_files = NO" >> $SubmitFile
echo "+JobFlavour           = \"microcentury\"" >> $SubmitFile
echo "MY.WantOS             = \"el9\"" >> $SubmitFile
echo >> $SubmitFile

for i in `ls $PWD/Samples/PAData/*/*root | Reformat 25 | sed "s/ /,/g" | sed "s/,[,]*$//"`
do
   Base=`basename $i`

   echo "Arguments = PAData8TeV ${i} $PWD/Output/PAData/Reco${Base} Dummy" >> $SubmitFile
   echo "Output    = Log/PAData${Base/.root}.out" >> $SubmitFile
   echo "Error     = Log/PAData${Base/.root}.out" >> $SubmitFile
   echo "Log       = Log/PAData${Base/.root}.log" >> $SubmitFile
   echo "Queue" >> $SubmitFile
   echo >> $SubmitFile
done

for i in `ls $PWD/Samples/PAMC/*root | sed "s/ /,/g" | sed "s/,[,]*$//"`
do
   Base=`basename $i`

   echo "Arguments = PAMC8TeV ${i} $PWD/Output/PAMC/Reco${Base} $PWD/Output/PAMC/Gen${Base}" >> $SubmitFile
   echo "Output    = Log/PAMC${Base/.root}.out" >> $SubmitFile
   echo "Error     = Log/PAMC${Base/.root}.out" >> $SubmitFile
   echo "Log       = Log/PAMC${Base/.root}.log" >> $SubmitFile
   echo "Queue" >> $SubmitFile
   echo >> $SubmitFile
done

for i in `ls $PWD/Samples/APMC/*root | sed "s/ /,/g" | sed "s/,[,]*$//"`
do
   Base=`basename $i`

   echo "Arguments = PAMC8TeV ${i} $PWD/Output/APMC/Reco${Base} $PWD/Output/APMC/Gen${Base}" >> $SubmitFile
   echo "Output    = Log/APMC${Base/.root}.out" >> $SubmitFile
   echo "Error     = Log/APMC${Base/.root}.out" >> $SubmitFile
   echo "Log       = Log/APMC${Base/.root}.log" >> $SubmitFile
   echo "Queue" >> $SubmitFile
   echo >> $SubmitFile
done

