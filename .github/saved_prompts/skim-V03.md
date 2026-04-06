# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.

# permission to write to /eos/cms/store/group/phys_heavyions/kdeverea
Check AGENT_REQUIREMENTS.md and copilot-instructions.md for updated permissions. You are now allowed to write files to the users directory on eos with the caveat the hadd is forbidden. The skimmer may ask you if you ran kinit -5 before, reply `y` to this.

# parallelization
Generally use NTHREAD=5 for MC sets and NTHREAD=15 for data. There is a known buffer size issue that might cause all processes to error if buffer is exceeded. If this happens decrease NTHREAD number and rerun the affected sets.

# V0.3 pp skim production
The skimmer is now ready for full V0.3 production for pp MC and data. Use as inputs
/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PPMC for MC and
/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/Samples/PPData as data
Produce the skims on the full input sets and write the outputs to
/eos/cms/store/group/phys_heavyions/kdeverea/Run2_2016_pPb_Skim/V0.3/PPMC and
/eos/cms/store/group/phys_heavyions/kdeverea/Run2_2016_pPb_Skim/V0.3/PPData respectively. DO NOT hadd or merge the files in anyway - simply write them to these directories. Rerun files that error or are zombies.

# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
