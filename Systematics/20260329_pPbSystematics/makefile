default: ExecuteCalculateSystematics ExecutePlotSystematics ExecutePlotPUComparison ExecutePlotTrackSelectionDiff

ExecuteCalculateSystematics: CalculateSystematics.cpp
	g++ CalculateSystematics.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecuteCalculateSystematics `root-config --cflags --glibs`

ExecutePlotSystematics: PlotSystematics.cpp
	g++ PlotSystematics.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotSystematics `root-config --cflags --glibs`

ExecutePlotPUComparison: PlotPUComparison.cpp PlotComparisonUtilities.h
	g++ PlotPUComparison.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotPUComparison `root-config --cflags --glibs`

ExecutePlotTrackSelectionDiff: PlotTrackSelectionDiff.cpp PlotComparisonUtilities.h
	g++ PlotTrackSelectionDiff.cpp -O3 -I$(ProjectBase)/CommonCode/include -o ExecutePlotTrackSelectionDiff `root-config --cflags --glibs`
