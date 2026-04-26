# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans. It is up to you how to schedule and send off the tasks to analyzer subagent(s), but keep the work of each subagent isolated. Be careful of recompiling executables which might be in use by another program. If there is a chance a file is being edited or executable ran or will soon-to-be ran by anther subagent, then wait until that subagent's task has concluded.


# new standard for muon-track selection
For the following tasks, use the current settings in OfficialWeightDictionary, i.e. V0.2 and most-current weights.

## task
Check and modify if needed systematics.sh such that the following standard for muon-track systematics is done. The nominal case should be given by `--isMuTagged=true --trackMuDR=-1 --trackMuClosest=false` and there should be one systematic variations given by `trackMuDR=0.004`. Run the new systematic variation for Zpt 5-500 and trk pT 0.5-500, and produce new plots for pPb, Pbp, pPb+Pbp standalone muon-track systematics with "Nominal" curve and "#Delta R > 0.004" curve. Make this the new standard for the standalone plot. Additionally, add back in the muon-track systematic to the total systematic calculation as calculated with the def for nominal and variation for pPb, Pbp, pPb+Pbp. Recalculate the total systematics as well as the total systematics overlay plots for these systems. We will wait on pp for now as the skims are still runnning.

## plots to overleaf
Copy the following new plots to the overleaf. 
 - standalone combined pPb+Pbp muon-track systematics plots should be put in the placeholder figure in the muon-track systematics section. Keep the pp plots blank here to be filled in later.
 - update the total systematics plots for combined pPb+Pbp.
 - update the track reconstruction : track selections section muon-track figure. Include three plots now for pp, pPb and Pbp data made from skims with event and track selection but using `--isMuTagged=false` so we can see all the tracks inside the deltaR regions. With plots made from MainAnalysis/20260216_temp/. Ensure there are two circles draw: one with radius 0.0025 and with radius 0.004.

## verification
Additionally, run the analysis on pPb in the same kinematic region but with the `trackMuDR=0.0025` setting. Theoretically this selection should be identical to the nominal case `--isMuTagged=true --trackMuDR=-1 --trackMuClosest=false` but we want to confirm this closure rigorously. Produce a set of closure plots at 20260404_pPbSystematics/plots/muonTrack/closure.

# pythia+madgraph misc
 - audit the pythia+madgraph homemade setup, what pdf selection is implemented and used right now?
 - audit the pythia+madgraph homemade setup, is the current simulation performed at LO or NLO? Report what settings in the setup control this.
 - remake the plots at Plots/20260404_ppClosure/production with the "inclusive" kinematics selection Z pT 5-500 and trk pT 0.5-500.

# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.

