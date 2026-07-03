default: ExecuteToyMC

ExecuteToyMC: compute_toymc.cpp
	g++ compute_toymc.cpp -O3 -I. -I $(ProjectBase)/CommonCode/include \
		-o ExecuteToyMC `root-config --cflags --glibs`

clean:
	rm -f ExecuteToyMC
