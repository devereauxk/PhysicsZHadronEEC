default: ExecuteJewel ExecuteJewelPPEnergy ExecuteJewelPPDiagnostics ExecuteJewelPPb

ExecuteJewel: plot_jewel.cpp
	g++ plot_jewel.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteJewel `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteJewelPPEnergy: plot_jewel_pp_energy.cpp
	g++ plot_jewel_pp_energy.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteJewelPPEnergy `root-config --cflags --glibs`

ExecuteJewelPPDiagnostics: plot_jewel_pp_diagnostics.cpp
	g++ plot_jewel_pp_diagnostics.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteJewelPPDiagnostics `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteJewelPPb: plot_jewel_pPb.cpp
	g++ plot_jewel_pPb.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteJewelPPb `root-config --cflags --glibs`

clean:
	rm -f ExecuteJewel ExecuteJewelPPEnergy ExecuteJewelPPDiagnostics ExecuteJewelPPb