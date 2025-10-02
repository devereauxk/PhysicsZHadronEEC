
default:  ExecuteDiff

TestRun: ExecuteDiff
	./Execute --OutputBase Test --SkipSystematics true

ExecuteDiff: MakePlotDiff.cpp
	g++ MakePlotDiff.cpp -o ExecuteDiff `root-config --cflags --libs` -I$(ProjectBase)/CommonCode/include/
