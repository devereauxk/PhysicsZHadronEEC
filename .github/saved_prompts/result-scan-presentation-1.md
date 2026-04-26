# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.


# misc beautification tasks
 - For the result scan presentation, there is significant white space around the plots. Especially horizontally. We want to compare figures side-by-side. Try to reduce this white space and inspect the result. If there is still too much white space, seperate deltaeta and deltaphi histograms onto seperate slides. Regenerate the presentation .pdf.
 - For the muon-track rejection systematics plots, increase the yaxis max value on the deltaeta plot to extend 30% higher. Remake all the standalone muon-track rejection plots with this change. 

# presentation changes
 - analyze and add the following Zpts to the slides 5-10, 30-40, 40-500.
 - arrange the slides so that there go from low Zpt to high Zpt based on the lower Zpt bin edge.
 - Scale each plot's pPb-pp difference subplot yaxis such that it is +/- 1.2 * abs(max difference).

# misc
 - The yaxis for the standalone muontrack rejection systematics plots at /Systematics/20260329_pPbSystematics/plots/muonTrack are still unchanged. Scale the yaxis for the deltaeta histograms for the ZPT40_350 bin - the one with the 5 systematic variations for pPb - such that they have double the white space above the max histogram value and the max yaxis value.


# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.
