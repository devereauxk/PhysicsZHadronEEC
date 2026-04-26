default: ExecuteCorrelationAnalysis ExecutePlotComparison ExecutePlotZcorrComparison ExecutePlotZptcutComparison ExecutePlotZcorrClosure ExecuteRunAverageDEta ExecutePlotExtremalRuns

ExecuteCorrelationAnalysis: CorrelationAnalysis.cpp
	g++ CorrelationAnalysis.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecuteCorrelationAnalysis `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecutePlotComparison: plot_comparison.cpp
	g++ plot_comparison.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlotComparison `root-config --cflags --glibs`

ExecutePlotZcorrComparison: plot_zcorr_comparison.cpp
	g++ plot_zcorr_comparison.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlotZcorrComparison `root-config --cflags --glibs`

ExecutePlotZptcutComparison: plot_zptcut_comparison.cpp
	g++ plot_zptcut_comparison.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlotZptcutComparison `root-config --cflags --glibs`

ExecutePlotZcorrClosure: plot_zcorr_closure.cpp
	g++ plot_zcorr_closure.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlotZcorrClosure `root-config --cflags --glibs`

ExecuteRunAverageDEta: tabulate_run_average_deta.cpp
	g++ tabulate_run_average_deta.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecuteRunAverageDEta `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecutePlotExtremalRuns: plot_extremal_runs.cpp
	g++ plot_extremal_runs.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlotExtremalRuns `root-config --cflags --glibs`
