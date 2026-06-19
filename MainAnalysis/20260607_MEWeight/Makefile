PROJBASE = $(ProjectBase)
COMMONDIR = $(PROJBASE)/CommonCode

CXXFLAGS = -O2 -std=c++17 $(shell root-config --cflags) -I$(COMMONDIR)/include
LDFLAGS = $(shell root-config --glibs)

default: Execute

Execute: CalculateMEWeight.cpp $(COMMONDIR)/library/Messenger.o
	g++ $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f Execute
