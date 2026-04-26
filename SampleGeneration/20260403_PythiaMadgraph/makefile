default: ExecuteConvertHepMC3 ExecuteShowerLHE ExecuteQuickDndEta ExecuteCompareOfficialPP

ProjectBase ?= $(abspath ../..)
PYTHIA_BASE ?= /home/kdeverea/pythia
PYTHIA8_CONFIG = $(PYTHIA_BASE)/pythia8310/bin/pythia8-config
COMMON_FLAGS = -O3 -I. -I$(ProjectBase)/CommonCode/include
ROOT_COMPAT_INC = -I$(ProjectBase)/SampleGeneration/20240607_PredictionConversion/include
ROOT_FLAGS = `root-config --cflags --glibs`
PYTHIA8_FLAGS = `$(PYTHIA8_CONFIG) --cxxflags`
PYTHIA8_LIB = `$(PYTHIA8_CONFIG) --libs`
HEPMC3_INC = -I$(PYTHIA_BASE)/hepmc3/include
HEPMC3_LIB = -L$(PYTHIA_BASE)/hepmc3/lib64 -Wl,-rpath,$(PYTHIA_BASE)/hepmc3/lib64 -lHepMC3
LHAPDF6_INC = -I$(PYTHIA_BASE)/lhapdf6/include
LHAPDF6_LIB = -L$(PYTHIA_BASE)/lhapdf6/lib -Wl,-rpath,$(PYTHIA_BASE)/lhapdf6/lib -lLHAPDF

ExecuteConvertHepMC3: ConvertHepMC3.cpp Helper.h
	g++ ConvertHepMC3.cpp -o ExecuteConvertHepMC3 $(COMMON_FLAGS) $(HEPMC3_INC) \
		$(ROOT_COMPAT_INC) $(ROOT_FLAGS) $(HEPMC3_LIB) \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecuteShowerLHE: ShowerLHE.cpp
	g++ ShowerLHE.cpp -o ExecuteShowerLHE -O3 $(COMMON_FLAGS) $(PYTHIA8_FLAGS) \
		$(HEPMC3_INC) $(LHAPDF6_INC) $(PYTHIA8_LIB) $(HEPMC3_LIB) $(LHAPDF6_LIB)

ExecuteQuickDndEta: QuickDndEta.cpp
	g++ QuickDndEta.cpp -o ExecuteQuickDndEta $(COMMON_FLAGS) $(ROOT_FLAGS) \
		$(ProjectBase)/CommonCode/library/Messenger.o

ExecuteCompareOfficialPP: CompareOfficialPP.cpp
	g++ CompareOfficialPP.cpp -o ExecuteCompareOfficialPP $(COMMON_FLAGS) $(ROOT_FLAGS) \
		$(ProjectBase)/CommonCode/library/Messenger.o

clean:
	rm -f ExecuteConvertHepMC3 ExecuteShowerLHE ExecuteQuickDndEta ExecuteCompareOfficialPP
