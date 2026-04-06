# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# benchmarking our 5.02 TeV pythia+madgraph sample against reference
The central values for deltaphi and deltaeta between our pp MC and reference pp MC was performed in Plots/202600404_ppClosure. Add into this directory another script for producing a comparison of Z pT distributions made from the analysis production files such as is done at Plots/20260115_ZResidualClosure. The two curves on the Z pt, Z eta, Z phi histograms should be the "Official pp gen MC" and the "Private Pythia+MadGraph". Use KylesPlotting.h and a ratio plot in the bottom between the two curves.

## resources
For all plotting tasks use the structure of .cpp files employing the root library and made with a makefile and ran with some runner .sh script. Implement this format as well for the old Z eta and track eta histograms from the sanity check.

# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
