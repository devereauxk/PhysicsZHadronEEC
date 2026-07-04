default: ExecuteResultPlot ExecuteThreePanel

ExecuteResultPlot: plot_central_combined.cpp
	g++ plot_central_combined.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-I/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/include \
		-I/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central \
		-o ExecuteResultPlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

ExecuteThreePanel: plot_three_panel.cpp
	g++ plot_three_panel.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-I/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central/include \
		-I/home/kdeverea/PhysicsZHadronEEC/Plots/20260213_Central \
		-o ExecuteThreePanel `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

clean:
	rm -f ExecuteResultPlot ExecuteThreePanel
