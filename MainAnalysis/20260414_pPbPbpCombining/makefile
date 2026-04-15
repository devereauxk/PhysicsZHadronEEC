default: ExecuteCorrelationAnalysis ExecutePlotComparison

ExecuteCorrelationAnalysis: CorrelationAnalysis.cpp
	g++ CorrelationAnalysis.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecuteCorrelationAnalysis `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecutePlotComparison: plot_comparison.cpp
	g++ plot_comparison.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlotComparison `root-config --cflags --glibs`
