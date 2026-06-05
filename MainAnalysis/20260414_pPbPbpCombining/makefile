default: ExecuteCorrelationAnalysis ExecutePlotComparison ExecutePlotZcorrComparison ExecutePlotZptcutComparison ExecutePlotZcorrClosure ExecuteRunAverageDEta ExecutePlotExtremalRuns ExecutePlot12x12PPbPbP ExecutePlot12x12Sideband ExecutePlot12x12CombinedSigned ExecutePlot12x12FramesXCheck ExecutePlot12x12PbvsPbp ExecutePlot12x12PPMCBoost ExecutePlot12x12Symmetrized

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

ExecutePlot12x12PPbPbP: plot_12x12_ppbpbp.cpp
	g++ plot_12x12_ppbpbp.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlot12x12PPbPbP `root-config --cflags --glibs`

ExecutePlot12x12Sideband: plot_12x12_sideband.cpp
	g++ plot_12x12_sideband.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlot12x12Sideband `root-config --cflags --glibs`

ExecutePlot12x12CombinedSigned: plot_12x12_combined_signed.cpp
	g++ plot_12x12_combined_signed.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlot12x12CombinedSigned `root-config --cflags --glibs`

ExecutePlot12x12FramesXCheck: plot_12x12_frames_xcheck.cpp
	g++ plot_12x12_frames_xcheck.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlot12x12FramesXCheck `root-config --cflags --glibs`

ExecutePlot12x12PbvsPbp: plot_12x12_pPb_vs_Pbp.cpp
	g++ plot_12x12_pPb_vs_Pbp.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlot12x12PbvsPbp `root-config --cflags --glibs`

ExecutePlot12x12PPMCBoost: plot_12x12_ppmc_boost.cpp
	g++ plot_12x12_ppmc_boost.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlot12x12PPMCBoost `root-config --cflags --glibs`

ExecutePlot12x12Symmetrized: plot_12x12_symmetrized.cpp
	g++ plot_12x12_symmetrized.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecutePlot12x12Symmetrized `root-config --cflags --glibs`
