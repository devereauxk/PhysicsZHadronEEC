The correction factors were recalculated by hand, reference the following as the new working point correction factors

VZWeightFile_PP="${VZ_WEIGHT_FILE_PP:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_pp.root}"
VZWeightFile_PPb="${VZ_WEIGHT_FILE_PPB:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_pPb.root}"
VZWeightFile_PbP="${VZ_WEIGHT_FILE_PBP:-/home/kdeverea/PhysicsZHadronEEC/Plots/20251001_pPbVZReweighting/summary/20260321_ZPT0_500_VzReweightFits_PbP.root}"

ZWeightFile_PP="${Z_WEIGHT_FILE_PP:-my_ZWeights/20260321_ZCorrection_V6_pp_zPt0-500.root}"
ZWeightFile_PPb="${Z_WEIGHT_FILE_PPB:-my_ZWeights/20260321_ZCorrection_V6_PPb_zPt0-500.root}"
ZWeightFile_PbP="${Z_WEIGHT_FILE_PBP:-my_ZWeights/20260321_ZCorrection_V6_PbP_zPt0-500.root}"

RWeightFile_PP="${R_WEIGHT_FILE_PP:-my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_pp_zPt}"
RWeightFile_PPb="${R_WEIGHT_FILE_PPB:-my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_PPb_zPt}"
RWeightFile_PbP="${R_WEIGHT_FILE_PBP:-my_residualWeights/20260321_ZV6_trkV24_TrackResidualCorrection_PbP_zPt}"

write instructions for the analyzer to continue the propagation of these weights to the paper plots. First, the analyzer should copy over the VZ distribution plots, and Z correction / correction closure, and track correction / correction closure. Use the plots that I just made by hand for this, they are in their respective directories with the ZV6 and trkV24 tags, ask me about any descrepancies. The analyzer should then use the weight referenced here to rerun for all pp pPb Pbp the MC central closure, and results plots. For pp the energy extrapolation should be reran. And for the pPb/Pbp the seperate comparison/combination plots should be remade. Have the analyzer copy the new plots over to the analysis note. As a standard for the future, the overleaf figures should NOT contain any plot which isnt referenced in text. Furthermore every file in figures which is copied over should have an identical filename with its source. Have the analyzer audit the figures for these condition when done.