default: ExecuteCorrelationAnalysis ExecuteDeltaRMuTrkPlot ExecuteDeltaRZGenRecoPlot

ExecuteCorrelationAnalysis: CorrelationAnalysis.cpp
	g++ CorrelationAnalysis.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecuteCorrelationAnalysis `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecuteDeltaRMuTrkPlot: plot_deltaR_MuTrk.cpp
	g++ plot_deltaR_MuTrk.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecuteDeltaRMuTrkPlot `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecuteDeltaRZGenRecoPlot: plot_deltaR_ZGenReco.cpp
	g++ plot_deltaR_ZGenReco.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecuteDeltaRZGenRecoPlot `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o


TestRun: ExecuteCorrelationAnalysis
	./ExecuteCorrelationAnalysis --Input XXX1.root,XXX2.root --Output output.root
