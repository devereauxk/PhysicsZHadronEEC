default: ExecuteCorrelationAnalysis ExecuteNmixComparisonPlot

ExecuteCorrelationAnalysis: CorrelationAnalysis.cpp
	g++ CorrelationAnalysis.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecuteCorrelationAnalysis `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecuteNmixComparisonPlot: PlotNmixComparison.cpp
	g++ PlotNmixComparison.cpp -O3 -I. -I./include -I$(ProjectBase)/CommonCode/include \
		-I$(ProjectBase)/Systematics/20260329_pPbSystematics \
		-o ExecuteNmixComparisonPlot `root-config --cflags --glibs`
