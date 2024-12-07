[B#!/bin/bash

# Toggle options (1 to enable, 0 to disable)
use_PbPb=1
use_Hybrid=1
use_HybridNoWake=0
use_PYQUEN=0
use_Jewel=0
use_JewelNoRecoil=0
use_CoLBT=0
use_PYTHIA8=0
rebinPhi=1
rebinY=2

# Initialize empty variables for files, labels, colors, lines, etc.
myfile=""
sys=""
label=""
colors=""
lines=""
markers="20,20,20,20,20,20,20,20"
extraInfoPhi="$1<p_{T}^{Z}<$2@GeV","|y_{Z}|<2.4"
extraInfoEta="$1<p_{T}^{Z}<$2@GeV","|y_{Z}|<2.4!@|#Delta#phi_{ch!Z}|<#frac{#pi}{2}"

# Add data based on toggles
if [ $use_PbPb -eq 1 ]; then
    myfile+="plots/PbPbMerged0_30_ZPT$1_$2-result.root,"
    sys+="plots/sys_pp-result.root,"
    label+="PbPb@0-30%,"
    colors+="1181,"
    lines+="0,"
fi

if [ $use_Hybrid -eq 1 ]; then
    myfile+="plots/hybridPbPb030_ZPT$1_$2-result.root,"
    sys+="plots/sys_pp-result.root,"
    label+="Hybrid,"
    colors+="1179,"
    lines+="1,"
fi

if [ $use_HybridNoWake -eq 1 ]; then
    myfile+="plots/hybridPbPbNoWake030-result.root,"
    sys+="plots/sys_pp-result.root,"
    label+="Hybrid@No@wake,"
    colors+="1179,"
    lines+="2,"
fi

if [ $use_PYQUEN -eq 1 ]; then
    myfile+="plots/pyquenPbPb030-result.root,"
    sys+="plots/sys_pp-result.root,"
    label+="PYQUEN,"
    colors+="1183,"
    lines+="2,"
fi

if [ $use_Jewel -eq 1 ]; then
    myfile+="plots/jewelPbPb030-result.root,"
    sys+="plots/sys_pp-result.root,"
    label+="Jewel@v2.2.0,"
    colors+="1180,"
    lines+="1,"
fi

if [ $use_JewelNoRecoil -eq 1 ]; then
    myfile+="plots/jewelPbPbNoRecoil030-result.root,"
    sys+="plots/sys_pp-result.root,"
    label+="Jewel@No@recoil,"
    colors+="1180,"
    lines+="2,"
fi

if [ $use_CoLBT -eq 1 ]; then
    myfile+="plots/PbPbMerged_CCNU_v4.root,"
    sys+="plots/sys_pp-result.root,"
    label+="CoLBT,"
    colors+="7,"
    lines+="1,"
fi

if [ $use_PYTHIA8 -eq 1 ]; then
    myfile+="plots/20pythiaGen-result.root,"
    sys+="plots/sys_pp-result.root,"
    label+="PYTHIA8@p_{T}^{Z}>20@GeV,"
    colors+="1,"
    lines+="2,"
fi


# Add data based on toggles
if [ $use_PbPb -eq 1 ]; then
    myfile+="plots/pp_ZPT$1_$2-result.root,"
    sys+="plots/sys_pp-result.root,"
fi

if [ $use_Hybrid -eq 1 ]; then
    myfile+="plots/hybridPP_ZPT$1_$2-result.root,"
    sys+="plots/sys_pp-result.root,"
fi

if [ $use_HybridNoWake -eq 1 ]; then
    myfile+="plots/hybridPP-result.root,"
    sys+="plots/sys_pp-result.root,"
fi

if [ $use_PYQUEN -eq 1 ]; then
    myfile+="plots/pyquenPP-result.root,"
    sys+="plots/sys_pp-result.root,"
fi

if [ $use_Jewel -eq 1 ]; then
    myfile+="plots/jewelPP-result.root,"
    sys+="plots/sys_pp-result.root,"
fi

if [ $use_JewelNoRecoil -eq 1 ]; then
    myfile+="plots/jewelPP-result.root,"
    sys+="plots/sys_pp-result.root,"
fi

if [ $use_CoLBT -eq 1 ]; then
    myfile+="plots/pp_CCNU_v4.root,"
    sys+="plots/sys_pp-result.root,"
fi

if [ $use_PYTHIA8 -eq 1 ]; then
    myfile+="plots/pythiaGen-result.root,"
    sys+="plots/sys_pp-result.root,"
fi

echo $myfile


# Remove trailing commas
myfile=${myfile%,}
sys=${sys%,}
label=${label%,}
colors=${colors%,}
lines=${lines%,}

# Execute plotting commands
./ExecuteDiff --ExtraInfo $extraInfoPhi --Markers $markers --Colors $colors --XAxisLabel "#Delta#phi_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta#phi_{ch,Z}" --SolidXMin 0 --SolidXMax 3.1415926 --XMin -1.5758 --XMax 4.7275 --Rebin $rebinPhi --ToPlot DeltaPhi --DataFiles $myfile --SystematicFiles $sys --YMin -2.5 --YMax 4.5 --RMin -2.5 --RMax 2.5 --OutputBase summary/result-DeltaPhi-PbPb0_30_ZPt$1_$2_MC --CurveLabels $label --lines $lines

./ExecuteDiff --ExtraInfo $extraInfoEta --Markers $markers --Colors $colors --XAxisLabel "#Deltay_{ch,Z}" --YAxisLabel "d#LT#DeltaN_{ch}#GT/d#Delta y_{ch,Z}" --SolidXMin 0 --SolidXMax 3.9999 --XMin -3.999 --XMax 3.9999 --Rebin $rebinY --ToPlot DeltaEta --DataFiles $myfile --SystematicFiles $sys --YMin -1.45 --YMax 1.45 --RMin -1.55 --RMax 1.55 --OutputBase summary/result-DeltaEta-PbPb0_30_ZPt$1_$2_MC --CurveLabels $label --lines $lines
