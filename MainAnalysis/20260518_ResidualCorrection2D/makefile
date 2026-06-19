default: ExecuteCorrelationAnalysis Execute2DResidualComparisonPlot

ExecuteCorrelationAnalysis: CorrelationAnalysis.cpp
	g++ CorrelationAnalysis.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecuteCorrelationAnalysis `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

Execute2DResidualComparisonPlot: Plot2DResidualComparison.cpp
	g++ Plot2DResidualComparison.cpp -O3 -I. -I./include -I$(ProjectBase)/CommonCode/include \
		-I$(ProjectBase)/Systematics/20260329_pPbSystematics \
		-o Execute2DResidualComparisonPlot `root-config --cflags --glibs`

TestRun: ExecuteCorrelationAnalysis
	./ExecuteCorrelationAnalysis --Input XXX1.root,XXX2.root --Output output.root
