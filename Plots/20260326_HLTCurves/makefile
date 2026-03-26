default: clean ExecuteHLTCurvePlot

ExecuteHLTCurvePlot: PlotHLTEfficiency.cpp
	g++ PlotHLTEfficiency.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteHLTCurvePlot `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

clean:
	rm -f ExecuteHLTCurvePlot
