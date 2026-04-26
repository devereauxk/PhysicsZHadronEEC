# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# pythia+madgraph setup for Z-hadron study
## motivation
Review PhysicsZHadronEEC and the Overleaf file for an overview for the general goals of the Z-hadron correlation study in pPb. You we will come across the statement that energy extrapolation is applied to pp 5.02 TeV data so that it can be compared to pPb at 8.16 TeV. Currently we perform this extrapolation with a data-driven method where we weight the dN/dpT for Z bosons from pp 5.02 TeV to pPb 8.16 TeV. In order to confirm this extrapolation is robust, and to calculate systematics for the energy extrapolation, we want a pp MC sample generated at 8.16 TeV. We already have an official pythia+madgraph sample at 5.02 TeV which is stored as the current pp MC file. We will now produce set up a private local installation of pythia+madgraph generator so we can produce 1) 8.16 TeV pp for our tasks, and 2) 5.02 TeV pp to benchmark the official sample and verify the setup is correct.

## your task
Install PYTHIA8+MADGRAPH5_aMC@NLO in a new directory at ~/pythia. Set up the installation for generating pp events at either 5.02 TeV or 8.16 TeV in the Drell-Yan Z to mumu channel. For pythia8 use the CP5 tune. Format the output such that is it parsable into a ZHadronMessenger structure. Significant development may be needed to format the output in this way. The output should be root files or hepmc files which can be converted to root files via the ConvertHempMC scripts in SampleGeneration/20240607_PredictionConversion. Use the scripts in this directory as inspiration to either produce files parsable by the skimmer available at 20260229_ReduceForestPA/ReduceForest.cpp, or produce a new skimmer which is suited to specifically the MC output. Note for reference the forests which serve as inputs to the skimmer are available at Samples. Decide which method works best for the pyhtia+madgraph setup. Either way the main deliverable is a "skim" file with a single TTree called Tree which is made from the ZHadronMessenger with as many branches filled as possible. For the MC processing scripts, make a new directory at SampleGeneration/20260403_PythiaMadgraph.

## test the framework
Once implemented, test the pipeline from generation to skim, on 5.02 TeV pp pythia+madgraph MC which you generate. Use a modest number of events for the test and produce a quick dn/deta for tracks and dn/deta for Z bosons with the output skim employing the ZHadronMessenger structure.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
