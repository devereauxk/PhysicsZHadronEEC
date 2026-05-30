default: clean ExecuteClosureTest ExecuteZEtaPhiDiagnostic ExecuteZCorrections

ExecuteClosureTest: plot_closure.cpp
	g++ plot_closure.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteClosureTest `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteZEtaPhiDiagnostic: plot_Z_etaphi_diagnostic.cpp
	g++ plot_Z_etaphi_diagnostic.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteZEtaPhiDiagnostic `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteZCorrections: plot_Z_corrections.cpp
	g++ plot_Z_corrections.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteZCorrections `root-config --cflags --glibs`

clean:
	rm -f ExecuteClosureTest ExecuteZEtaPhiDiagnostic ExecuteZCorrections