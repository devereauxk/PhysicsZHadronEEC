default: clean ExecuteClosureTest ExecuteCentralPlot ExecuteZSpectrumPlot ExecuteZMassPlot ExecuteDataMCComparison ExecuteCentralCombinedPlot

ExecuteClosureTest: plot_closure.cpp
	g++ plot_closure.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteClosureTest `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteCentralPlot: plot_central.cpp
	g++ plot_central.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteCentralPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteZSpectrumPlot: plot_zspectrum.cpp
	g++ plot_zspectrum.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteZSpectrumPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteZMassPlot: plot_zmass.cpp
	g++ plot_zmass.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteZMassPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteDataMCComparison: plot_dataMCComparison.cpp
	g++ plot_dataMCComparison.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteDataMCComparison `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteCentralCombinedPlot: plot_central_combined.cpp
	g++ plot_central_combined.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteCentralCombinedPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

clean:
	rm -f ExecuteClosureTest
	rm -f ExecuteCentralPlot
	rm -f ExecuteZMassPlot
	rm -f ExecuteZSpectrumPlot
	rm -f ExecuteDataMCComparison
	rm -f ExecuteCentralCombinedPlot
	