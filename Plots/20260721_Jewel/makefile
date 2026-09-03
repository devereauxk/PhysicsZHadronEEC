COMMON = /home/kdeverea/PhysicsZHadronEEC/CommonCode
ROOTFLAGS = $(shell root-config --cflags --glibs)
MESSENGER = $(COMMON)/library/Messenger.o

default: ExecuteJewelComposite

ExecuteJewelComposite: plot_jewel_composite.cpp
	g++ plot_jewel_composite.cpp -O3 -I$(COMMON)/include -o ExecuteJewelComposite $(ROOTFLAGS) $(MESSENGER)

clean:
	rm -f ExecuteJewelComposite
