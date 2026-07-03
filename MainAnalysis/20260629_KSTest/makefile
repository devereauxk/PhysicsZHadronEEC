default: ExecuteKS

ExecuteKS: compute_ks.cpp
	g++ compute_ks.cpp -O3 -I. -I $(ProjectBase)/CommonCode/include \
		-o ExecuteKS `root-config --cflags --glibs` -lMathMore

clean:
	rm -f ExecuteKS
