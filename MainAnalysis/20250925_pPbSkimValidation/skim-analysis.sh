#!/bin/bash
# ============================================================
# Process skim by hand
# ============================================================

name="20250925_skimvalidation"

# pPb MC gen
skim=./pPbSample/PPbMC_Gen.root
name_1=20250925_skimvalidation_pPbMC_Gen
ApplyEventSelection=0
ApplyTrackSelection=0

root -l -b -q "skimValidation.C(\"$skim\", \"output/$name_1.root\", false, $ApplyEventSelection, $ApplyTrackSelection)"

# pPb MC reco
skim=./pPbSample/PPbMC_Reco.root
name_2=20250925_skimvalidation_pPbMC_Reco
ApplyEventSelection=0
ApplyTrackSelection=0

root -l -b -q "skimValidation.C(\"$skim\", \"output/$name_2.root\", false, $ApplyEventSelection, $ApplyTrackSelection)"

#============================================================

root -l -q -b "plotComparison.C(\"output/$name_1.root\", \"output/$name_2.root\", \"plots/$name\")"
