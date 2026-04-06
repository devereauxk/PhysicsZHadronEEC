# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# energy extrapolation systematic calculation
## motivation
Accoring to the overleaf we are still missing a key systematic uncertainty for the pp set. This stems from the energy extrapolation, currently the 8.16 TeV pp central values are calculated through a data-driven approach where we correct by a scale derived from the ratio of Z pT spectra (8.16 pPb data) / (5.02 pp data). We will keep this as the nominal case to be used in our central values. We will use one variation to calculate systematics which is the MC-driven correction factor we have derived from our homemade pp pythia+madgraph.

## task
### analysis with EE variation
The energy extrapolation values for the MC-driven method were produced in the last task and are at `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260222_EnergyExtrapolation/workflow/output/20260405_EnergyExtrapolation_private8160over5020.root`. Quote this weight along with the nominal weight file in OfficialWeightDisctionary.sh at `OFFICIAL_EE_WEIGHT_FILE_PP_PRIVATE`. Implement the variation in systematics.sh in the main analysis directory by quoting the path from the dictionary. Remember this variation is only applied to pp. Run an analysis of pp data with the variation of energy extrapolation weight for the Z pt 5-500 and track pT 0.5-500 kinematic selection and use the tag suffix `_EEPrivate` for the analysis output.

### systematics calculation
Then implement the energy extrapolation in the systematics calculation for pp data in the systematics working directory. Do this in the same was as PU, track selection, and track correction is done where bin-by-bin we compare the nominal histogram central values with the variation(s) and quote the maximum absolute difference as the systematic uncertainty. Remake the total pp systematic calculation with EE systematic, remake the total systematics plots to include a curve for EE in pp. Additionally make a stand-alone script like is done for PU which produces histograms for the deltaeta deltaphi histograms with the `Nominal` case overlayed with the variation to be labeled `MC-driven`.

# misc
As a second minor task, I have noticed the EEV4 .pdf plots are missing from the energy extrapolation correction directory `/home/kdeverea/PhysicsZHadronEEC/MainAnalysis/20260222_EnergyExtrapolation/workflow/output/`. Without effecting the EEV4 weight at `OFFICIAL_EE_WEIGHT_FILE_PP`, which is the current official nominal weight, regenerate these histograms with the same sets of data as `OFFICIAL_EE_WEIGHT_FILE_PP`.


# resources
For all plotting tasks use the structure of .cpp files employing the root library and made with a makefile and ran with some runner .sh script. Implement this format as well for the old Z eta and track eta histograms from the sanity check.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
