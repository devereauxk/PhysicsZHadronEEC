#source ./clean.sh

export ProjectBase=/home/kdeverea/PhysicsZHadronEEC
export AnalysisBasePA=$ProjectBase/SampleGeneration
export NTHREAD=${NTHREAD:-5}
#make Execute

OUTPUTDIR=/eos/cms/store/group/phys_heavyions/kdeverea/Run2_2016_pPb_Skim/V0.0
mkdir -p $OUTPUTDIR/PAData
mkdir -p $OUTPUTDIR/PAMC
mkdir -p $OUTPUTDIR/APMC

DODATA=$1
DOPAMC=$2
DOAPMC=$3

if [[ "$DODATA" == "1" ]]; then
   for i in `ls $PWD/Samples/PAData/000*/HiForestAOD_*.root | sed "s/ /,/g" | sed "s/,[,]*$//"`
   do
      ./local_skim.sh PAData8TeV ${i} $OUTPUTDIR/PAData/Reco$(basename ${i}) Dummy
   done
fi

if [[ "$DOPAMC" == "1" ]]; then
   for i in `ls $PWD/Samples/PAMC/*root | sed "s/ /,/g" | sed "s/,[,]*$//"`
   do
      ./local_skim.sh PAMC8TeV ${i} $OUTPUTDIR/PAMC/Reco$(basename ${i}) $OUTPUTDIR/PAMC/Gen$(basename ${i})
   done
fi

if [[ "$DOAPMC" == "1" ]]; then
   for i in `ls $PWD/Samples/APMC/*root | sed "s/ /,/g" | sed "s/,[,]*$//"`
   do
      ./local_skim.sh PAMC8TeV ${i} $OUTPUTDIR/APMC/Reco$(basename ${i}) $OUTPUTDIR/APMC/Gen$(basename ${i})
   done
fi
