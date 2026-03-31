# introduction
Read the guidelines at .github/copilot-instructions. You have the role of reviewer, you will write plans for analyzer and then start an analyzer subagent to carry out that those plans.

These are follow ups for the last tasks:

# loose tight track tree investigation
Looking at the loose, tight, nominal overlay plot the statistical error bars on the tight and loose points are significantly larger than the nominal case. Investigate why this is. Was a smaller fraction of the skim used for these checks? if so rerun the systematic runners for full statisiticsa and remake the systematic plots. Ensure the other systematics are also calcualted with full statistics. If rerunning is done use at least NTHREAD=15. 

# conclusion
Make the minimum number of edits necessary to implement functionality. Do not add superflous error statement catches or checks. 

Have the subanalyzer handoff a summary to you and you should review that summary for flaws. Update copilot instructions on the new norms detailed here and learned by you or the subanalyzer throught these tasks.

