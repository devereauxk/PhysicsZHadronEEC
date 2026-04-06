# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# validation of pythia+madgraph setup for Z-hadron study
## motivation
The setup as concluded and produced some reasonable plots for Z eta and track eta. This is a good sanity check but by no means confirmation that our setup is capable of producing a similar set of events as the official cmssw-ran pythia+madgraph at 5.02TeV. That is the purpose of this study.

## task
Use the official cmssw-ran file pp file at `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20241102_ZhadronVsZPt/mergedSample/pp-v11-Zpt0.root` as comparison.

### event selection
The route chosen in the last task was to use a ConvertHepMC3-style script for parsing the MC information into the output TTree ZHadronMessenger format. Take a look at the skimmer at ReduceTreePA/ReduceTree.cpp and take note of the event and track selections that are applied to generator level-MC, as well as the conditions and method for which Z boson information and is filled into the skims. Audit our curent ConvertHepMC3 and implement any of these functionalitites that might be missing.

### Zpt0 check
The first check is to confirm the Z pT spectrum is similar between the official MC and our MC. Check the configuration for the drell-yan process to make sure that production of Z bosons down to 0 GeV is possible. Then produce a sufficient number of events 100k-1M for pp at 5.02 TeV. It is up to you to maximize this number of events while keeping the total validation task time under around 6 hours. Compare the Z pT spectra between our sample and the reference sample. Produce a plot for these spectra, with log binning on the x (pT) axis going from 0.01 Zpt to 500 geV, and a linearly binned plot going from 0 to 50 in Z pt. For both of these have a ratio plot in the bottom panel. Try to use PlotCMSRatio function from include/KylesPlotting.h.

### eta check
Redo the comparison from the sanity check for the Z eta and track eta distributions. Overlay our 5.02 TeV pp distribution with the reference with ratio plot in the bottom.

### central value check
Finally setup a runner script in the main analysis folder `pp-madgraphclosure.sh` that runs the main analysis in the MC generator mode (with no corrections applied, not even VZ) for the reference pp MC and our pp MC for nmix=10. Make sure the analysis completes for the new MC set without error. Then make a new directory at `Plots/20260404_ppClosure` where you plot the overlay of the two pp sets with difference plot. Make the same -all -bkg and -result plots as is done at `/home/kdeverea/PhysicsZHadronEEC/Plots/20260120_CentralClosure`.

## resources
For all plotting tasks use the structure of .cpp files employing the root library and made with a makefile and ran with some runner .sh script. Implement this format as well for the old Z eta and track eta histograms from the sanity check.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
