# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# pythia+madgraph Z-hadron mc-driven scale factor
## 8.16 / 5.02 sample Z pt ratio
Produce now 100k pp events with our pythia+madgraph setup for 8.16 TeV. Use the tools at MainAnalysis/20260222_EnergyExtrapolation that takes the ratio of Z pT spectra between our mandmade 8.16 and 5.02 TeV pp 100k event pythia+madgraph samples. Produce the plots and weight files that are chacteristic of that directory but tag them with no EEV- tag and use a _closure tag instead. DO NOT override the preexisting root or pdf files there.

## investigation of variation between homemade 5.02 and reference 5.02
The agreement between our set and the reference 5.02 set is much better now but there is still some disagreement in the Z pt spectra where our set is pushed to slighly lower Z pTs wrt the reference set. This is characteristic of a lower pt hhat min setting or some sort of lower hard scattering energy scale setting. Do not change, but audit the code and pythia config cards that might contain settings that would make this effect. Furthermore study the effect of the pythhia tune used. Do we actally use the CP5 tune? Would using a different tune give a similar behavior? Document your findings in a .md file.  

## resources
For all plotting tasks use the structure of .cpp files employing the root library and made with a makefile and ran with some runner .sh script. Implement this format as well for the old Z eta and track eta histograms from the sanity check.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
