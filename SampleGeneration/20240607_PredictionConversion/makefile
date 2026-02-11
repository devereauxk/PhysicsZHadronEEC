default: ExecuteHepMC

ExecuteHepMC: ConvertHepMC.cpp
	g++ ConvertHepMC.cpp -O3 -I. -I$(ProjectBase)/CommonCode/include -I./include \
		-o ExecuteHepMC `root-config --cflags --glibs` \
		$(ProjectBase)/CommonCode/library/Messenger.o \