# Introduction

Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.

We will now implement the systematic uncertainties for the analysis note stored in the overleaf repository. Read the text for a description of each of the checks to carry out.

Rewrite systematics.sh in the main analysis folder such that the various systematics are ran for the pp, pPb, Pbp options with the format
```
if doPP:
run_pp_chain <input files, corrections> TAG="SystematicTag1" --ExampleSystematicFlag1
run_pp_chain <input files, corrections> TAG="SystematicTag2" --ExampleSystematicFlag2
run_pp_chain <input files, corrections> TAG="SystematicTag3" --ExampleSystematicFlag3
and so on
```

Implement the following systematics in this manner for pp, pPb and Pbp. For the nominal cases, DONOT include in the systematics.sh file as these should only be ran by central.sh. To not overwrite these files do not use the "central tags" : "EEV3_ZV6_trkV24_nmix10" for pp or "ZV6_trkV24_nmix10" for pPb/Pbp alone without individual descriptions. In fact please store these tags in the OfficialWeightsDisctionary as OFFICIAL_TAG_PP and OFFICIAL_TAG_PPB, respectively, so they are paired directly to the correction filse used, and deploy them in central.sh and systematics.sh.

# First task
Updating the main analysis CorrelationAnalysis and the systematics.sh runner script. When editing CorrelationAnalysis make the minimum number of edits necessary to implement. Do not add superflous error statement catches or checks. 

## track selection systematic
Already implemented in main analysis code. Just hard-code the check in the runner.
These new relate to tags 

TAG="${OFFICIAL_TAG}_Loose$ --TrackSelectionMode=Loose
TAG="${OFFICIAL_TAG}_Tight$ --TrackSelectionMode=Tight

## track-muon rejection
Make sure this tag is in the analysis code as --IsMuTagged=true/false. True is the nominal choice when rejection is applied (i.e. track is rejecting if it is tagged as a muon), false is when the rejection is not applied. In systematics.sh this should show up as

TAG="${OFFICIAL_TAG}_IsMuTaggedFalse$ --IsMuTagged=False

## Pile-up (PU) rejection
### for pp
This is already implemented for pp. Use the form

TAG="${OFFICIAL_TAG}_IsPURejectFalse$ --IsPUReject=False

### for pPb
This is not applied for pPb but we want to extend the study to look at PU in pPb/Pbp. Modify the main analysis to apply PU rejection to PU when IsPUReject=true. Then add this line as well to the pPb and Pbp cases in the systematics runner

TAG="${OFFICIAL_TAG}_IsPURejectFalse$ --IsPUReject=False

## Muon scale factor uncertainties
Right now there are 4 muon scale factor uncertainty variations in the skimmer stored in the ExtraZWeight[0-3] branch. When applied, these should scale the Z Weight for both signal and mixed events, i.e. do ZWeight*=ExtraZWeight[i]. The nominal case is no extra Z weight applied so only apply if the flag is called. Each flag call should correpond to a different Z weight.

TAG="${OFFICIAL_TAG}_MuVar0$ --ExtraZWeight=0
TAG="${OFFICIAL_TAG}_MuVar1$ --ExtraZWeight=1
TAG="${OFFICIAL_TAG}_MuVar2$ --ExtraZWeight=2
TAG="${OFFICIAL_TAG}_MuVar3$ --ExtraZWeight=3

## test run
These are all implemented in the V0.1 skims (except PU in pPb) so run a quick test of systematics.sh to ensue all tags work. Use Z pt 40-350 and trk pT 2-500 so it doesnt take too long to run.


# Second task
Creating a systematics working directory.

## structure
Locate the folder at Systematics/20260329_pPbSystematics. This will be the working direcotry for calcualting the systematics and plotting them. First copy over the file at 20241102/systematics.C.

## total systematics calculation
Use the systematics.C file as a starting point. Rewrite the file in cpp and make a makefile to run it. Inspect the file a remove systematics we do not reference in the analysis note, i.e. centrality hibin, lowbin treatment. Audit the code for bugs and modify it where needed. Implement the file to include run on all the systematic variations just implemented in the First Task, i.e. track selections systematic, track-muon rejection, PU, and muon scale factor variations. For each of these the file should read in the nominal result produced by central.sh with the OFFICIAL_TAGs and compare to the variations one-by-one for the deltaphi and deltaeta obeservables in each Z pt and track pT bin seperately. For each uncertainty type calculate the max deviation for each bin in deltaeta and deltaphi between nominal and variations ands store the max variation for the bin as the uncertainty contirbution. Do this for all bins in deltaphi and deltaeta. Do this for all uncertainties as well. In addition to the variation uncertainties add an additional 2.4% uncertainty constant across bins to account for the tracking correction uncertainty. At the end, the total systematic uncertainty should be calculated bin-by-bin in deltaeta and deltaphi by summing the maxdeviations in quadrature. Generally follow the direction used in the file already and report any inconsistencies back to me. For each input Z pT root file, systematics.C should produce an output root file with histograms for

1) the individual systematics for each uncertainty binned in deltaeta, deltaphi. These should be named TrackSelection, TrackCorrection, MuonRejection, PUpp, PUpPb, ScaleFactor. So thats 12 TH1s: 6 uncertainties and for each 1 deltaphi TH1 and 1 deltaeta TH1.
2) total systematic uncertainty: total calculated by adding all systematics in quadrature. So thats 2 TH1s: 1 deltaphi TH1 and 1 deltaeta TH1.

## plotting script
Produce also a plotting script in this working directory in a cpp file that takes in the systematic uncetainty root files and produces (for that track pT and Z pT) a plot that displays all individual uncertainty histograms overlayed as well as the total uncertainty histogram. Two plots should be made: one for deltaphi uncertainty and one for deltaeta. Use different colors for each uncertainty and a legend. In the plot state the relevant kinematic selection for Z pt and track pT.

## systematics runner
Create a runner script that runs the systematic calculation as well as the plotting and dictates which uncertainties are included in the calculation/plotting, as well as which Z pTs and track pTs are ran over. We do this since we will have more uncertainties to add later, and we may have to change the kinematic selections. Have the runner reference the OFFICIAL_TAGs from the dictionary, and the tag suffixs used in the First task.

## test run
Test run the systematic calculation and plotting on the Z pt 40-350 and trk pT 2-500 file produced in First task. Exclude the pPb PU from consideration.


# Conclusion
Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks. Add a README to the systematics.md folder with a breif description of all this information.
