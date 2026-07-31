default: ExecuteInclusive ExecuteComposite

ExecuteInclusive: plot_inclusive.cpp
	g++ plot_inclusive.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-I/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/include \
		-I/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central \
		-o ExecuteInclusive `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteComposite: plot_composite.cpp
	g++ plot_composite.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-I/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/include \
		-I/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central \
		-o ExecuteComposite `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

clean:
	rm -f ExecuteInclusive ExecuteComposite
