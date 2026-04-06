# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# pythia+madgraph Z-hadron mc-driven scale factor
## motivation
The correct reference is a gen-level MC to compare against the gen-level MC we are producing. The difference is the `pythia-gen-` in the file name - the corrected file name is below, as well as a summary of the studies to redo. Our 5.02 TeV pp sample should be already made and should be used again here.

## task
Use the official cmssw-ran file pp gen-level file at `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pythia-gen-v11-Zpt0.root` as comparison. Rewrite the appropriate scripts such that particles are saved down to 0.5 GeV. This is especially important for the central value comparisons which are for track pTs 0.5-500 GeV. Regenerate the MC pp at 5.02 TeV for 100k events if the track info isn't stored to that low of pT. Also for our purposes the EventWeight and TrackWeight should be applied for both our pp MC and the reference. This is beacause madgraph is an NLO generator and stores some negative weights which actually should be applied. Use these standards for remaking the following plots.

### Zpt0 check
The first check is to confirm the Z pT spectrum is similar between the official MC and our MC. Compare the Z pT spectra between our sample and the reference sample. Produce a plot for these spectra, with log binning on the x (pT) axis going from 0.01 Zpt to 500 geV, and a linearly binned plot going from 0 to 50 in Z pt. For both of these have a ratio plot in the bottom panel. Try to use PlotCMSRatio function from include/KylesPlotting.h.

### eta check
Redo the comparison from the sanity check for the Z eta and track eta distributions. Overlay our 5.02 TeV pp distribution with the reference with ratio plot in the bottom.

### central value check
Finally setup a runner script in the main analysis folder `pp-madgraphclosure.sh` that runs the main analysis in the MC generator mode (with no corrections applied, not even VZ) for the reference pp MC and our pp MC for nmix=10. Make sure the analysis completes for the new MC set without error. Then make a new directory at `Plots/20260404_ppClosure` where you plot the overlay of the two pp sets with difference plot. Make the same -all -bkg and -result plots as is done at `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure`.


## resources
For all plotting tasks use the structure of .cpp files employing the root library and made with a makefile and ran with some runner .sh script. Implement this format as well for the old Z eta and track eta histograms from the sanity check.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
