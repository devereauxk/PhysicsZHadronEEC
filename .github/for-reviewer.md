# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans. It is up to you how to schedule and send off the tasks to analyzer subagent(s), but keep the work of each subagent isolated. Be careful of recompiling executables which might be in use by another program. If there is a chance a file is being edited or executable ran or will soon-to-be ran by anther subagent, then wait until that subagent's task has concluded.


# V0.3 production followup
 - Make a new latex presentation following the same format as the one at Plots/20260213_Closure/presentation, with the same exact Z pt and trk binnings, but made with the new V0.3 data productions, new tag "ZV8_trkV26_nmix10" , and new weights. Reproduce the needed histograms in the main analysis using the new production.
 - plots were copied to the overleaf apparantly by changing the file names to match the filename called in tex, do not do this, transfer files to the overleaf with the EXACT file name as they were generated with. For this production we should see ZV8, trkV26 etc for all plots. Go back to the overleaf and recopy ALL modified plots. Change instead the file names in the tex text to match the incoming plots. From now on keep only USED pdfs in the figures/ file. Remove all figure pdfs which are actively called in the text.
 

# finalized kinematic selection
We have now decided on the following kinematic selections for the final results:
 - "Inclusive plot": Zpt 0-500 and trk pT 0.5-15 GeV (one selection).
 - "Scan plots": Scanning over Z pt 0-30 30-500, and trk pT 0.5-2 2-4 4-1 (2x3 =6 selections).
Propagate these selections to the plots should be made and copied to the following locations in the analysis note.
 - Results: keep the current structure of one figure for inclusive plot and then one figure with the scan plots. Arrange the scan plots so each row is a Zpt selection and each coloumn is a track pT selection. Reduce the horizontal white space between plots as much as possible.
 - MC closure (before and after background subtraction): remake ALL MC closure plots applying the new track selection of trk pT 0.5-15, but keep the 0-10, 10-20, 20-40, 40-500 Z pt bins. Replace those effected in-note as well as in the appendix.
 - pp extrapolation. Keep the extrapolation calculation the same, but for the plots showing the effect differentially in zpt and track pt, modify the inclusize figure and the subsequnt scan figures to include the new selection scheme.
 - pPb+Pbp combinign section: do the same for this section
 - systematics: replot all the systematics with the inclusive selection and put them in the main text. Make systematics plots for the scan selections to but make a new appendix section at the end of the note to put them in. 
 - track-muon plots that were saved to the "track reconstruction" section of the note with the track pT selection of 0.5-15 GeV and ZpT 0-500 GeV. Do this for pp, pPb and Pbp and replace those plots in the note. 


# difference systematics calculation
The result plots in the note include two histograms per plot technically: the central value correlation histogram for pPb and pp in the top panel, and then the difference pPb - pp histogram in the bottom panel. So far we have calculated the systematic uncertainty on the top histogram curves and this is all taken care of, but for our final plots we also need to calculate the bottom systematic serperately from the top. Given a systematic with nominalppb, and variations varppb1, varppb2, ... for pPb and nominalpp, varpp1, varpp2, ... for pp. Define the nominal difference as (nominalppb - nominalpp). This is just the central value for the difference plots. Define the variation differences as (varppb1 - varpp1), (varppb2 - varpp2), etc. The systematic uncertainty should be quoted bin-by-bin as the max absolute difference between the nominal difference and ALL the variation differences. For systematics which pertain to only on system, like energy extrapolation for pp, the variation differences should be (nominalppb - varpp1), (nominalppb - varpp2), etc.

Add support for the difference systematic calculation and perform it for the final inclsuive selection. Make a graph in deltaeta and deltaphi for the total difference systematic decomposed in terms of all the individual systematics like is done at Systematics/20260329_pPbSystematics/PlotSystematics and add it to the total systematics section of the overleaf under the total central value systematics plot. Make minimal change in text to cite the new figure.

# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.

