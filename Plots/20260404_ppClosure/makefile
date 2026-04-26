default: clean ExecuteClosureTest ExecuteComparePP ExecuteCompareProduction

ExecuteClosureTest: plot_closure.cpp
	g++ plot_closure.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteClosureTest `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteComparePP: plot_comparison.cpp
	g++ plot_comparison.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteComparePP `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteCompareProduction: plot_production.cpp
	g++ plot_production.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteCompareProduction `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

clean:
	rm -f ExecuteClosureTest ExecuteComparePP ExecuteCompareProduction
