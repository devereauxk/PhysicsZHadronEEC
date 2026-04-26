# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.

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

# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.


