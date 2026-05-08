# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# full stack reanalysis with V0.2 skims
The V0.2 skims were produced and added manually at the following directory.
/home/kdeverea/PhysicsZHadronEEC/SampleGeneration/20250929_ReducedTreePA/V0.2
Note the correction here to use the skims copied over to /home instead of /eos.
We will now reproduce entire analysis stack with these skims as the inputs. This will involve recalculating the VZ, Z, and track correction weights in the OfficialWeightDictionary.sh for the central values. For analysis producion use NTHREAD=20 as a default.

## official weight handeling
we do not want to override correction files for the old analysis since we many want to return to it as a backup. Move the current official weight.sh file to a new file OfficialWeightDictionary_ANV1.sh and start a clean OfficialWeightDictionary.sh with paths to the new skims and empty paths for the correction factors to-calculate.

Use the same TAG convention we have been using. Now increment the official TAG to: EEV4, ZV7, trkV25. Use todays date 20260331 where is appears in flags.

For the official files, assume from now on that input skims have the CORRECT naming orientation PPb* indeed point to the PPb collision orientation, and this is true for all MC and data skims.

## correction recalculation
Recalculate the correction factors for pPb and Pbp skims with the V0.2 in the following order.
 - VZ correction
 - Z correction
 - Track correction

For each subsequent correction calculated, store the relevant path to correction files in OfficialWeightDictionary.sh and ensure the next correction factor reads in the new correction path from there.

Add support along the way to run over pp skims, but do not run them as they are not V0.2 ready.

During that default event selection is such that PU rejection is off for both pp and pPb as a default for all correction and analysis scripts.

Ensure there is no |vz|<15 requirement for the VZ calculation fit as it helps the fit technique to include bins outside the analysis region. But in ALL subsequent correction analysis scripts add logic to enforce |vz|<15 in the event selection function.

For each correction recalculated, remake the appropriate closure plots. Use the same kinematic selections as the analysis and push them to the analysis note.

## MC-level closure
Reproduce the closure of reco to gen MC plots with and without background subtraction applied. Use the same kinematic selection as the current plots in the analysis note and push them to the note. If at this point significant nonclosure is seen then stop and debug the source of nonclosure.

## pPb - Pbp seperate combination comparison
Reproduce also this plot with the new corrections applied to data pPb and Pbp and push to the analysis note.

## HLT trigger efficiency plots
This section of the analysis note is unfinished since we were waiting for the new skims to include the HLT turn on curve information. Produce the turn on curves for the HLT curves using prexisitng code and write them into the analysis note as figures and update the text of that section to point to the plots and describe them correctly. 

## Miscelaneus plots for analysis note
 - ensure the VZ distribution and ratio plots are updated
 - new Z invariant mass distributions with for pPb and Pbp

# systematics framework for V0.2 skims
The new information in the V0.2 allows us to implement systematics rigorously now. There are some cases where systematics require us to recalculate seperate variations of the corrections in our pipeline. Implement a framework of new runner scripts, or modifications to old ones, in the main analysis directory and correction directories that produces specialized corrections for the following cases. DO NOT plot closure plots for any of these correction factors except those automatically produced during calculation. For these implement support assuming the pp skims will have the same structure, but do not run as these skims are not ready still.

## Track selection variations
Seperate track residual corrections will be need to be calculated for the loose and tight variationsc choices. Update the track residual correction directory to support generating loose and tight corrections, by substituting out the input Tree to the TreeLoose/TreeTight as the main analysis script does. vary the trees to loose and tight for the reco files; for the gen track information we make no variation so keep gen to reference the nominal tree. keep the nominal correction as the default, and functionality should stay the same for nominal with no extra `_Nominal_` in the file names. Process the correction files for loose and tight variation and add a `_Loose_` or `_Tight_`  name to the files. Add the files to the OfficialWeightDictionary.sh. In total there should be 16 additional track residual correction references in the dictionary for 2x new selection variations, 4x Zpt bins each, 2x pPb and Pbp.

## Muon-track rejection
This is similar to the track selection variations in that it will only affect the track residual correction. Make a runner script that can run correlation analysis with the IsMuTagged=false flag and name the variation files with a `_IsMuTaggedFalse_` in the name. Generate the corrections and add to the official dictionary. This corresponds with 8 additional corrections: 4x ZpT bins, 2x pPb and Pbp.

## PU in pPb
During the VZ calculation the default event selection is such that PU rejection is off for both pp and pPb as a default, meaning there is no PU enformenet as nominal, and we require PU=1 as a systematic variation. Since we calculate this step with data input although, we need a full stack of new corrections just for the PU=1 systematic. Carry implement this such that we can calculate a new VZ (PU=1)correction, followed by a new Z correction (PU=1), followed by a new track residual correction (PU=1), at each step making sure (PU=1). For this it would be really annoyiong if it made an inpact on our main runner scripts, so make new scripts to avoid this clashing with code that is already tested to be good. Run the full train of corrections. And put the corrections in the dictionary, for this we will have an additional 12 corrections: (1 VZ + 1 Z + 4 track) x2 pPb or Pbp. 


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
