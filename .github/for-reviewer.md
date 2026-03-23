We want to codify the official weights so they are used whenever data is reprocessed. There are instances when the analyzer uses outdated weights and we want to avoid this. The following are the paths of the official weights

VZWeightFile_PP="${VZ_WEIGHT_FILE_PP:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_pp.root}"
VZWeightFile_PPb="${VZ_WEIGHT_FILE_PPB:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_pPb.root}"
VZWeightFile_PbP="${VZ_WEIGHT_FILE_PBP:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_PbP.root}"

ZWeightFile_PP="${Z_WEIGHT_FILE_PP:-my_ZWeights/20260321_ZCorrection_V6_pp_zPt0-500.root}"
ZWeightFile_PPb="${Z_WEIGHT_FILE_PPB:-my_ZWeights/20260321_ZCorrection_V6_PPb_zPt0-500.root}"
ZWeightFile_PbP="${Z_WEIGHT_FILE_PBP:-my_ZWeights/20260321_ZCorrection_V6_PbP_zPt0-500.root}"

RWeightFile_PP="${R_WEIGHT_FILE_PP:-my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_pp_zPt}"
RWeightFile_PPb="${R_WEIGHT_FILE_PPB:-my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_PPb_zPt}"
RWeightFile_PbP="${R_WEIGHT_FILE_PBP:-my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_PbP_zPt}"

First make sure all runner scripts that reference weights use these weights for VZ, Z, and track corrections for all pp, pPb, and Pbp scripts. Report any inconsistencies.

Make a config file at PhysicsZHadronEEC/OfficialWeightDictionary.sh to act as a reference for official weights, giving the FULL path to each, and populate the file with the directories here. Modify all runner scripts that reference weights to point to this file for the official paths. Update copilot-instructions with the new standard for weight referencing. Complete this task yourself.

Secondly, form a plan for the review to speed up the skimming procedure testing on PAData input files. Right now it seems to be limited by file I/O. Have the analyzer try out parallelization across a few files with a few choices for NTREADS on a subset of the files to see if parallelization helps (it might not even). If it does report the best value NTREAD tried. When implementing parallelization make sure the processes write to unique file names. Have the analyzer implement the script in a new file run_parallel_local_skim.sh and that it has the style of run_local_skim.sh and calls local_skim.sh. Make edits to local_skim.sh as necessary to support compatibility.