default: ExecuteCorrelationAnalysis ExecutePionComparisonPlot

ExecuteCorrelationAnalysis: CorrelationAnalysis.cpp
	g++ CorrelationAnalysis.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecuteCorrelationAnalysis `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecutePionComparisonPlot: PlotPionComparison.cpp
	g++ PlotPionComparison.cpp -O3 -I. -I./include -I$(ProjectBase)/CommonCode/include \
		-I$(ProjectBase)/Systematics/20260329_pPbSystematics \
		-o ExecutePionComparisonPlot `root-config --cflags --glibs`

TestRun: ExecuteCorrelationAnalysis
	./ExecuteCorrelationAnalysis --Input XXX1.root,XXX2.root --Output output.root
