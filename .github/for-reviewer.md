# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# muon-track rejection systematics
## motivation
We will implement a new scheme for calculating the muon-track following the conclusions of the last task. To avoid massive contribution from the square feature in the track-muon deltaR graphs we will instead quote a variation of multiple non-zero deltaR requirements between tracks and muons rather than an all or nothing cut.

## task
Implement a new branch in each CorrelationAnalysis script

TODO



# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
