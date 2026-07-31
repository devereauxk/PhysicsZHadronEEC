COMMON = /home/kdeverea/PhysicsZHadronEEC/CommonCode
ROOTFLAGS = $(shell root-config --cflags --glibs)
MESSENGER = $(COMMON)/library/Messenger.o

default: ExecuteJewelComparison ExecuteJewelZPt ExecuteJewelScanPanel

ExecuteJewelComparison: plot_jewel_comparison.cpp
	g++ plot_jewel_comparison.cpp -O3 -I$(COMMON)/include -o ExecuteJewelComparison $(ROOTFLAGS) $(MESSENGER)

ExecuteJewelZPt: plot_jewel_zpt.cpp
	g++ plot_jewel_zpt.cpp -O3 -I$(COMMON)/include -o ExecuteJewelZPt $(ROOTFLAGS) $(MESSENGER)

ExecuteJewelScanPanel: plot_jewel_scan_panel.cpp
	g++ plot_jewel_scan_panel.cpp -O3 -I$(COMMON)/include -o ExecuteJewelScanPanel $(ROOTFLAGS) $(MESSENGER)

clean:
	rm -f ExecuteJewelComparison ExecuteJewelZPt ExecuteJewelScanPanel
