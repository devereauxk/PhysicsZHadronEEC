default: clean ExecuteClosureTest ExecuteResidualPlot

ExecuteClosureTest: plot_closure.cpp
	g++ plot_closure.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteClosureTest `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteResidualPlot: plot_residualcorrection.cpp
	g++ plot_residualcorrection.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteResidualPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

clean:
	rm -f ExecuteClosureTest
	rm -f 