default: ExecuteConvertHepMC

ProjectBase ?= $(abspath ../..)
COMMON_FLAGS = -O3 -I. -I$(ProjectBase)/CommonCode/include
ROOT_FLAGS = `root-config --cflags --glibs`

ExecuteConvertHepMC: ConvertHepMC.cpp Helper.h
	g++ ConvertHepMC.cpp -o ExecuteConvertHepMC $(COMMON_FLAGS) \
		$(ROOT_FLAGS) \
		$(ProjectBase)/CommonCode/library/Messenger.o
