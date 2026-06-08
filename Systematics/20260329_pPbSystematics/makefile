default: ExecuteCalculateSystematics ExecutePlotSystematics ExecutePlotPUComparison ExecutePlotTrackSelectionDiff ExecutePlotTrackCorrectionDiff ExecutePlotMuonTrackComparison ExecutePlotEnergyExtrapolationComparison ExecutePlotEventMixingComparison ExecutePlotMetColorsExample ExecuteComputeChiSquared

ExecuteCalculateSystematics: CalculateSystematics.cpp
	g++ CalculateSystematics.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecuteCalculateSystematics `root-config --cflags --glibs`

ExecutePlotSystematics: PlotSystematics.cpp
	g++ PlotSystematics.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotSystematics `root-config --cflags --glibs`

ExecutePlotPUComparison: PlotPUComparison.cpp PlotComparisonUtilities.h
	g++ PlotPUComparison.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotPUComparison `root-config --cflags --glibs`

ExecutePlotTrackSelectionDiff: PlotTrackSelectionDiff.cpp PlotComparisonUtilities.h
	g++ PlotTrackSelectionDiff.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotTrackSelectionDiff `root-config --cflags --glibs`

ExecutePlotTrackCorrectionDiff: PlotTrackCorrectionDiff.cpp PlotComparisonUtilities.h
	g++ PlotTrackCorrectionDiff.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotTrackCorrectionDiff `root-config --cflags --glibs`

ExecutePlotMuonTrackComparison: PlotMuonTrackComparison.cpp PlotComparisonUtilities.h
	g++ PlotMuonTrackComparison.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotMuonTrackComparison `root-config --cflags --glibs`

ExecutePlotEnergyExtrapolationComparison: PlotEnergyExtrapolationComparison.cpp PlotComparisonUtilities.h
	g++ PlotEnergyExtrapolationComparison.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotEnergyExtrapolationComparison `root-config --cflags --glibs`

ExecutePlotEventMixingComparison: PlotEventMixingComparison.cpp PlotComparisonUtilities.h ResultCombinationUtilities.h
	g++ PlotEventMixingComparison.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotEventMixingComparison `root-config --cflags --glibs`

ExecutePlotMetColorsExample: PlotMetColorsExample.cpp
	g++ PlotMetColorsExample.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotMetColorsExample `root-config --cflags --glibs`

ExecuteComputeChiSquared: ComputeChiSquared.cpp
	g++ ComputeChiSquared.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecuteComputeChiSquared `root-config --cflags --glibs`
