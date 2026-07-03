default: ExecuteBootstrap

ExecuteBootstrap: compute_bootstrap.cpp
	g++ compute_bootstrap.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteBootstrap `root-config --cflags --glibs`

clean:
	rm -f ExecuteBootstrap
