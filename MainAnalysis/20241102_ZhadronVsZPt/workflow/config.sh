PT_RANGES=("1_2" "2_4" "4_10")
HIBIN_RANGES=("0_30")
ZPT_RANGES=("0_10" "10_20")
UseLeadingTrk=0

root -l -b -q "ZPtEtaCorrection.C(\"output/pPbMC_ZPT0_100-0_40.root\", \"output/1pPb_ZPT0_100-0_40.root\")"