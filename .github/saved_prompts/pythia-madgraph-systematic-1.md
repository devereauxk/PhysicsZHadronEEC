# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# energy extrapolation systematic plots
 - relabel the pp total systematic uncertainty plots to change "Energy extrapolation" to "EnergyExtrapolation"
 - read the recently updated systematics section, there are placeholder figures for the standalone pp energy extrapolation variation plots. Place the energy extrapolation plots there in the overleaf.
 - in the same section there are place-holders for the mc-driven weight graph and closure graph. Wire the correct graphs from the pythia+madgrpah comparison here.
 - update the pp total systematics plots in the overleaf.


# resources
For all plotting tasks use the structure of .cpp files employing the root library and made with a makefile and ran with some runner .sh script. Implement this format as well for the old Z eta and track eta histograms from the sanity check.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
