ProjectBase = /home/kdeverea/PhysicsZHadronEEC

ExecuteCentralCombinedPlot: plot_central_combined.cpp KylesPlotting.h MITHIG_CMSStyle.h
	g++ plot_central_combined.cpp -O3 -I. -I $(ProjectBase)/CommonCode/include \
		-o ExecuteCentralCombinedPlot `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o
