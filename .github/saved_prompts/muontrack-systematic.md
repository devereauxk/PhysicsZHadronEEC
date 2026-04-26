# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# muon-track rejection systematics
## motivation
We will implement a new scheme for calculating the muon-track following the conclusions of the last task. To avoid massive contribution from the square feature in the track-muon deltaR graphs we will instead quote a variation of multiple non-zero deltaR requirements between tracks and muons rather than an all or nothing cut.

## task
Implement a new arguement in the main analysis CorrelationAnalysis called `--trackMuDR` which defaults to `-1`. When the default value -1 is used then keep the `if (par.isMuTagged && (*b->trackMuTagged)[j]) return false;` logic in the analysis at trackSelection. However if trackMuD is not -1 then INSTEAD of cutting on trackMuTagged boolean, then apply the logic `if ((*b->trackMuDR)[j] < trackMuDR) return false;`. Also implement another argument `--trackMuClosest` which defaults to `false`. When false fall back to the logic `if (par.isMuTagged && (*b->trackMuTagged)[j]) return false;`. When `true`, trackSelection should reject tracks if they are one of the two closest tracks to a muon, i.e. sort tracks by trackMuDR and reject tracks with the lowest trackMuDR values, per event. You can assume that the analysis will never be ran with more than one flag out of --isMuTagged --trackMuDR or --trackMuClosest used.

## running diagnostics
Implement the following systematic variations in a SEPERATE `systematics-trackMuDR.sh` file that is structured similar to systematics.sh. we are still determining what the best strategy here is so we will test without messing with the working  systematics structure. Implement it for pp and pPb and Pbp data for variations
```
--isMuTagged=false
--trackMuDR=0.001
--trackMuDR=0.0025
--trackMuDR=0.0035
--trackMuClosest=true
```
the nominal should be taken as `--isMuTagged=true --trackMuDR=-1 --trackMuClosest=false`. Analyze all these cases and overlay all the central values using the framework at `/home/kdeverea/PhysicsZHadronEEC/Systematics/20260329_pPbSystematics/PlotMuonTrackComparison.cpp`.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
