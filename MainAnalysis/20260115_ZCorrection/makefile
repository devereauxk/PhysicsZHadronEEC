default: ExecuteCorrelationAnalysis

ExecuteCorrelationAnalysis: CorrelationAnalysis.cpp
	g++ CorrelationAnalysis.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include -I./include \
		-o ExecuteCorrelationAnalysis `root-config --cflags --glibs` \
		/home/kdeverea/PhysicsZHadronEEC/CommonCode/library/Messenger.o

TestRun: ExecuteCorrelationAnalysis
	./ExecuteCorrelationAnalysis --Input XXX1.root,XXX2.root --Output output.root