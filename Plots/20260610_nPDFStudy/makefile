default: ExecuteNPDF ExecuteNPDFScan ExecuteNPDFIndividual

ExecuteNPDF: plot_nPDF.cpp
	g++ plot_nPDF.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteNPDF `root-config --cflags --glibs`

ExecuteNPDFScan: plot_nPDF_scan.cpp
	g++ plot_nPDF_scan.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteNPDFScan `root-config --cflags --glibs`

ExecuteNPDFIndividual: plot_nPDF_individual.cpp
	g++ plot_nPDF_individual.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteNPDFIndividual `root-config --cflags --glibs`

clean:
	rm -f ExecuteNPDF ExecuteNPDFScan ExecuteNPDFIndividual
