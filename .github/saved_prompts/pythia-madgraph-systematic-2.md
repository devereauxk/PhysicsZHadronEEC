# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# official to private pythia+madgraph Z boson yields
Looking at the Z rapidity distribution made at Plots/20260404_ppClosure we see the private MC sample seems to integrate to a lower number than the official MC sample. This doesn't make sense since dN_Z/deta_Z is filled by Zs and is normalized by total number of Zs, so the integral should be the same between samples. Debug the cause of the normaliztion difference and rerun/remake plots when it is figured out. One thing to check is if event weights are applied when the Z distribution is filled. They should be applied as histogram weights. Also N_Z should be incremented using the same event weights weights, not just adding 1 for each Z. Another possibility it is indeed normalized but over a more broad Z pt or Z eta kinematic region, and a different cut is applied to the respective sets. If that is the case we should apply the same cuts as the official set.

This fix will also probably effect the other diagnotic plots made in ppClosure, so remake the other plots is that is the case. Perform an audit of MC-driven energy extrapolation scripts at MainAnalysis/20260222_EnergyExtrapolation to see if this bug propagates there. If so remake the MC-driven correction factor, replace it in the official weight dictionary, rerun the energy extrapolation systematics and plot the new plots. If this effects overleaf plots for the analysis note, then copy the new ones to the note. Summarize the bug at the end of running and what exactly was reran.


# resources
For all plotting tasks use the structure of .cpp files employing the root library and made with a makefile and ran with some runner .sh script. Implement this format as well for the old Z eta and track eta histograms from the sanity check.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
