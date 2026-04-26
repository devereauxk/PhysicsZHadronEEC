# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans. It is up to you how to schedule and send off the tasks to analyzer subagent(s), but keep the work of each subagent isolated. Be careful of recompiling executables which might be in use by another program. If there is a chance a file is being edited or executable ran or will soon-to-be ran by anther subagent, then wait until that subagent's task has concluded.

# permission to write to /eos/cms/store/group/phys_heavyions/kdeverea
Check AGENT_REQUIREMENTS.md and copilot-instructions.md for updated permissions. You are now allowed to write files to the users directory on eos with the caveat the hadd is forbidden.

# parallelization
Generally use NTHREAD=5 for MC sets and NTHREAD=15 for data. There is a known buffer size issue that might cause all processes to error. If this happens decrease NTHREAD number and rerun the affected sets.

# V0.3 pp skim production
The skimmer is now ready for full V0.3 production for pp MC and data. Use as inputs
/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PPMC for MC and
/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PPData as data
Produce the skims on the full input sets and write the outputs to
/eos/cms/store/group/phys_heavyions/kdeverea/Run2_2016_pPb_Skim/V0.3/PPMC and
/eos/cms/store/group/phys_heavyions/kdeverea/Run2_2016_pPb_Skim/V0.3/PPData respectively. DO NOT hadd or merge the files in anyway - simply write them to these directories. Rerun files that error or are zombies.

# muon-track standalone plots remake
The standalone muon-track systematic plots, the ones at Systematics/22060329_pPbSystematics/plots/muonTrack/*40_350* were generated with the following task `systematics-trackMuDR.sh` file that is structured similar to systematics.sh. we are still determining what the best strategy here is so we will test without messing with the working  systematics structure. Implement it for pp and pPb and Pbp data for variations
```
--isMuTagged=false
--trackMuDR=0.001
--trackMuDR=0.0025
--trackMuDR=0.0035
--trackMuClosest=true
```
the nominal should be taken as `--isMuTagged=true --trackMuDR=-1 --trackMuClosest=false`. Analyze all these cases and overlay all the central values using the framework at `/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/PlotMuonTrackComparison.cpp`.
Repeat this extended list of variations for Zpt 5-500 and trk pT 0.5-500. Remake the plots at muonTrack with newly produced variations. For the deltaeta plots, double the whitespace between the max histogram bin and the max yaxis value.

# V0.3 pPb/Pbp skim production
Perform this section only after the last pltting item and pp skim task. Do not attempt to skim this until the pp skims are completed.
We will also produce refreshed V0.3 skims for the PA MC and data. This is because we patched a small bug with the event selection criteria. Use as inputs
/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PAMC for MC and
/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PAData as data
Produce the skims on the full input sets and write the outputs to
/eos/cms/store/group/phys_heavyions/kdeverea/Run2_2016_pPb_Skim/V0.3/{PA,AP}MC and
/eos/cms/store/group/phys_heavyions/kdeverea/Run2_2016_pPb_Skim/V0.3/PAData respectively. DO NOT hadd or merge the files in anyway - simply write them to these directories. Rerun files that error or are zombies.

# misc tasks
For the following avoid recompiling CommonCode as an agent is running skiming code which references those objects and that may affect them.
 - The homemade 5.02TeV pythia+madgraph Z pt spectra as produced in Plots/20260404_ppClosure/plots/production do not make sense since the homemade graphs are significantly lower (near zero) compared with the official pp pythia+madgraph MC curve. Debug why this is, look at the normalization applied and confirm event weights are applied properly during histogram filling/N_Z counting. Remake the plots when the issue is isolation. Report back what the issue was.
 - Taking a glimpse at the skim for homemade pythia+madgraph 5.02TeV we see no negative event weights, but the official pp MC sample does have negative weights. In general we expect NLO generators like pythia+madgraph to have negative weights. Investigate if there truely are no negtaive weights in our homemade sample. If so confirm there is no requirement in the analysis pipeline including generation, ConvertHepMC3, and main analysis that forces positive event weights. Report back on your conclusions.
 - Add the homemade 5.02 TeV and 8.16 TeV pp pythia+madgraph skim path locations to the OfficialWeightDictionary and reference these variables wherever homemade pythia+madgraph is referenced in the main analysis directory as well as the energy extrapolation calculation directory.
 - Looking at the muon-track standalone plots produced at Systematics/20260329_pPbSystematics/plots/muonTracks/ for pPb, Pbp and combined pPb+Pbp we see the deltaR < 0.0025 curve (`trackMuDR=0.0025`) and Nominal (`--isMuTagged=true --trackMuDR=-1 --trackMuClosest=false`) curve do not agree perfectly. This is unexpected since in the skim logic the trackMuTagged branch should fire if deltaR with a muon is < 0.0025. Investiagte why these curves disagree and report back. If it is a plotting or analysis issue then regenerate the plots. Do not edit, rerun or recompile skimmer since it is in use right now.
There are many items to report here so create a .md file and put feedback in there for review by the user.

# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.

