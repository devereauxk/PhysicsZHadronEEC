default: ExecutePbPbOverlay ExecutePPEE ExecutePPEEMC

ExecutePbPbOverlay: plot_pbpb_overlay.cpp
	g++ plot_pbpb_overlay.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include \
		-o ExecutePbPbOverlay `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecutePPEE: plot_pp_ee.cpp
	g++ plot_pp_ee.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include \
		-o ExecutePPEE `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecutePPEEMC: plot_pp_ee_mc.cpp
	g++ plot_pp_ee_mc.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include \
		-o ExecutePPEEMC `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o

clean:
	rm -f ExecutePbPbOverlay ExecutePPEE ExecutePPEEMC
