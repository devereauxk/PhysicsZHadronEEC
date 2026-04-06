# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# muon-track rejection
## plots for old skims
Make new muon-track delta eta and delta phi plots for PbPb data in the `MainAnalysis/2026016_temp` directory for the following PbPb forests
`/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PbPbData`
`/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PbPbMC`
for ~1M events. These plots should be made using the same framework as the other forest followup studies. Pair reco muons to reco tracks in the forests. Output the files to `MainAnalysis/2026016_temp/plots/PbPbforests/`. Produce seperate sets of muon-track deltaR plots following the same structure as those made at raw_forest_followup/ppData_rawForest_single_1M*.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
