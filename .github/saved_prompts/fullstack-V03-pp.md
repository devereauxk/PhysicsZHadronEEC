# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans. It is up to you how to schedule and send off the tasks to analyzer subagent(s), but keep the work of each subagent isolated. Be careful of recompiling executables which might be in use by another program (especially those in CommonCode). If there is a chance a file is being edited or executable ran or will soon-to-be ran by anther subagent, then wait until that subagent's task has concluded.

# V0.3 full stack analysis
## official skims sets
The new pp skims have been completed, hadd-ed, copied, and are available at
`SampleGeneration/20250929_ReducedTreePA/V0.3/PPData_Reco.root` for data
`SampleGeneration/20250929_ReducedTreePA/V0.3/PPMC_Reco.root` for MC reco
`SampleGeneration/20250929_ReducedTreePA/V0.3/PPMC_Gen.root` for MC gen
These were regenerated and now should be fixed with non-zero trackWeights.
We will refer to these at the V0.3 skims. We will continue to use the EPOS files from the same paths as before as well as the private pp inputs. Assume all files have been correctly labelled for their respective collision orientation. The previous task running the analysis at working point
```
OFFICIAL_TAG_PP="EEV5_ZV8_trkV26_nmix10"
OFFICIAL_TAG_PPB="ZV8_trkV26_nmix10"
```
completed for pPb/Pbp only because of the missing track weights for pp.

## task
Your task is to complete the fulls stack of corrections and plots detailed in `.github/saved_prompts/fullstack_v03_template.md` for the new V0.3 pp skims ONLY. Save your new corrections to OfficialWeightDictionary.sh, touch ONLY pp weight files and edit ONLY pp-specific paths in the dictionary. In addition to following all directions in the template file use the dictionary as a laundry list and make sure all pp correction factors and systematics are renewed. Remove all current pp corrections/systematics from the dictionary and start the pp calculation chain from scratch (the skim definitions) - i.e. even VZ weights should be refereshed. Every pp plot appearing in the note should be made new, including all those detailed in the "plots" section below.

## reference info
The following information and tasks were used to create the template plan. They are reposted here for guidance. Take the appropriate directions for the pp case only when you make your plan.

### correction factor recalculation
Once a correction is calculated, fill it in to the dictionary. Always source the correction locations from the dictionary when called in runner .sh scripts. Opt to use preexisting runner scripts in the main analysis directory. Additionaly, corrections should be made for the full train of systematics variations as already implemented in systematics.sh and as variables in the dictionary. The main difference between the skims is the inclusion/correction of the loose and tight trees. In general the central values should be unchanged in these calculations but we will recalculate to be thorough. Use the same kinematic selections for these corrections as was done for V0.2.

### new systematics
#### pp track selection loose/tight
We now have the proper information to complete the track selection systematic for pp. Follow the same procedure as pPb and Pbp to calculate this systematic with loose and tight variations, and specially-calculated track residual corrections for each of those as well. Store them as new items in thhe dictionary `OFFICIAL_R_WEIGHT_FILE_PP_LOOSE` and `OFFICIAL_R_WEIGHT_FILE_PP_TIGHT`.

#### muon-track tagging
We now replace the old IsMuTagged=false condition as the variation for out track-muon systematic. The `*_ISMUTAGGEDFALSE` items in the dictionary should be removed and new definitions `OFFICIAL_R_WEIGHT_{}_TRACKMUDR0p004` for {PP, PPB, PBP} put in their place. Use `--isMuTagged=true` as the nominal case. For this systematic (as should already be the default in `central.sh`), and use `--trackMuDR=0.004` as the variation in `systematics.sh`. Implement this standard also in `MainAnalysis/20251211_TrackCorrection/workflow/` and calculate the track residual correction for the variations. Call these corrections in `systematics.sh` for the 0.004 delta R variation. 

### plots
For each level of corrections recalculated, the associated closure plots should be reproduced and copied to the overleaf. Reference the overleaf for the VZ, Z, track residual, energy extrapolation and other closure plots to remake and copy. A full list is also given here for a full list of plots to refresh with the new results and copy over. Use the same kinematic selection (zpt and track pt) as is done in the overleaf for each set of plots.
 - HLT trigger : make a new one for pp as the HLT hist information is now available. Add the pp plot to the same figure as the pPb and Pbp trigger curves in the trigger efficiency section. Make these in `Plots/20260326_HLTCurves` and copy over to the note.
 - VZ correction weight ratios for pp as closure for VZ reweighting. Make these in `Plots/20251001_pPbVZReweighting` and copy over to the note.
 - invariant mass peaks in Z boson reconstruction section. Make these in `Plots/20260213_Central/plot_zmass.cpp` and copy over to the note.
 - Z correction factors. Make these in `MainAnalysis/20260115_ZCorrection/workflow/` and copy over to the note.
 - Z correction closure. Make these in `Plots/20260115_ZResidualClosure/` and copy over to the note.
 - track-muon distribution in the track selection section make from `MainAnalysis/20260216_temp/`.
 - track residual correction factors. Make these in `MainAnalysis/20251211_TrackCorrection/workflow/` and copy over to the note.
 - track residual correction closure. Make these in `Plots/20251202_trackResidualClosure/` and copy over to the note.
 - energy extrapolation. Make these with the data-driven approach (i.e. using the ratio of Z pt spectra in 8.16 TeV pPb data and 5.02 TeV pp data) in `MainAnalysis/20260222_EnergyExtrapolation` and `Plots/20260213_Central/plot_energyExtrapolation.cpp`.
 - all closure plots for the central values in pp MC before and after background substraction. Make these with `Plots/20260120_CentralClosure/`. At this step an audit should be made for the closure at MC-level after background subtraction. These plots should close reasonable well. If they do not then bug hunt and report the issues. If they are silly then fix and continue the plotting campaign.
 - all pp systematics plots should be made new. We also have the information for pp as detailed in the "new systematics" section of this note. And should now make standalone plots for pp for track selection systematic and muon-track tagging. Make these plots and add them to their respective sections in the note. Also calculate the new total systematic uncertainties for pp. pp should display curves now for (TrackSelection, TrackCorrection, MuonRejection, PU, ScaleFactor, EnergyExtrapolation, Total). Use the same kinematics selection Z pt 5-500 and trk pT 0.5-500 for all the systematic plots. Make these at `Systematics/`.
 - result plots. Remake these plots for energy extrapolated pp and combined pPb sets for the present kinematic selections.

### data quality guidelines
Initialize the workflow per-subtask by making sure the analysis and plotting executables are fresh. For all plots here use ONLY the corrections calculated newly for the V0.3 and ignore those from dates before 20260407. Push plots to the overleaf, making as little edits to the text as possible. If a correction doesnt exist, then find out why and make it yourself. At each step audit the plots themselves and compare to the current note versions. Report anywhere that the plots differ significantly. Especially in cases of closure plots, we expect the same level of closure, so report when this happens and it should be a red flag to check for bugs. Once each plotting subtask is done, they should be copied to the overleaf so they can be reviewed during this long campaign. Use Nthread=30, be carefull of segfaulted threads that might return quitely, so check the terminal outputs diligently. For safer but slower running use lower nthread, but try not to go below 10, I will leave it up to you.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer(s) handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer(s) throught these tasks.

