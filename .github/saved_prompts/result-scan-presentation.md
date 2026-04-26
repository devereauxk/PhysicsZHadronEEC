# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# results scan
## running
using the current official weights and official input skims produce central values for delta phi and delta eta as ran by central.sh. For kinematic selection use trk pT 0.5-2 2-4 4-500 and Zpt use ranges 0-5, 0-10, 10-20, 20-30, 0-20, 0-30, 30-500, 20-40, 40-60, 60-500. For all cases use nmix=10 and nthread=20 and analyze pp, pPb, and Pbp. pp data should be energy extrapolated and pPb data should be combined with Pbp data.

## central values plots
Use the plotting macros at Plots/20260213_Central/plot_central_combined.cpp and associated runner scripts to make the plots and output them at Plots/20260213_Central/plots/EEV4_ZV7_trkV25_nmix10/.

## presentation
Make a minimal latex presentation with the results of the scan. Format the slides so that each slide corresponds with a Z pt selection, with three plots each side-by-side corresponding with the three track pT choices. minimize whitespace around the figures and include no text except for titles for each slide shich should specify the Z pt. Make a title slide. Make slide 2 with information on the skims, weights, tags, etc which can be used to differentiate these plots from past and future versions. Save the presentation to its own directory at Plots/20260213_Central/presentations.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
