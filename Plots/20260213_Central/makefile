default: clean ExecuteCentralPlot ExecuteCentralSBPlot ExecuteZSpectrumPlot ExecuteZMassPlot ExecuteDataMCComparison ExecuteCentralCombinedPlot ExecuteEnergyExtrapolationPlot ExecutePPPlot ExecutePUPlot ExecuteVZPlot ExecuteCentralOverlayPPbPbPPlot ExecutePaperPlot ExecutePPEEPlot ExecuteCorr2DMapBin12x12 ExecuteCorr2DMapPPbPbP

ExecuteCentralPlot: plot_central.cpp
	g++ plot_central.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteCentralPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteCentralSBPlot: plot_central_sb.cpp
	g++ plot_central_sb.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteCentralSBPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteZSpectrumPlot: plot_zspectrum.cpp
	g++ plot_zspectrum.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteZSpectrumPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteZMassPlot: plot_zmass.cpp
	g++ plot_zmass.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteZMassPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteDataMCComparison: plot_dataMCComparison.cpp
	g++ plot_dataMCComparison.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteDataMCComparison `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteCentralCombinedPlot: plot_central_combined.cpp
	g++ plot_central_combined.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteCentralCombinedPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteEnergyExtrapolationPlot: plot_energyExtrapolation.cpp
	g++ plot_energyExtrapolation.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteEnergyExtrapolationPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecutePPPlot: plot_pp.cpp
	g++ plot_pp.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecutePPPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteCentralOverlayPPbPbPPlot: plot_central_overlay_PPbPbP.cpp
	g++ plot_central_overlay_PPbPbP.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteCentralOverlayPPbPbPPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecutePUPlot: plot_PU.cpp
	g++ plot_PU.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecutePUPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteVZPlot: plot_VZ.cpp
	g++ plot_VZ.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteVZPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecutePaperPlot: plot_paper.cpp
	g++ plot_paper.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecutePaperPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteCorr2DMapPPbPbP: plot_2d_pPbPbp_comparison.cpp
	g++ plot_2d_pPbPbp_comparison.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteCorr2DMapPPbPbP `root-config --cflags --glibs`

clean:
	rm -f ExecuteCentralPlot
	rm -f ExecuteCentralSBPlot
	rm -f ExecuteZMassPlot
	rm -f ExecuteZSpectrumPlot
	rm -f ExecuteDataMCComparison
	rm -f ExecuteCentralCombinedPlot
	rm -f ExecuteEnergyExtrapolationPlot
	rm -f ExecutePPPlot
	rm -f ExecuteCentralOverlayPPbPbPPlot
	rm -f ExecutePUPlot
	rm -f ExecuteVZPlot
	rm -f ExecutePaperPlot
	rm -f ExecutePPEEPlot
	rm -f ExecuteCorr2DMapBin12x12
	rm -f ExecuteCorr2DMapPPbPbP

ExecutePPEEPlot: plot_pp_ee.cpp
	g++ plot_pp_ee.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecutePPEEPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteCorr2DMapBin12x12: plot_2d_corr_bin12x12.cpp
	g++ plot_2d_corr_bin12x12.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteCorr2DMapBin12x12 `root-config --cflags --glibs`
