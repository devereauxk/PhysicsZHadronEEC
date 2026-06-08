default: ExecuteCompatibility ExecuteProjectionAudit ExecuteKS ExecuteCompatibilitySym ExecuteOverlaySym

ExecuteCompatibility: compute_pPbPbp_compatibility.cpp
	g++ compute_pPbPbp_compatibility.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteCompatibility `root-config --cflags --glibs`

ExecuteProjectionAudit: audit_projection_integrals.cpp
	g++ audit_projection_integrals.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteProjectionAudit `root-config --cflags --glibs`

ExecuteKS: compute_pPbPbp_ks.cpp
	g++ compute_pPbPbp_ks.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteKS `root-config --cflags --glibs` -lMathMore

ExecuteCompatibilitySym: compute_pPbPbp_compatibility_sym.cpp
	g++ compute_pPbPbp_compatibility_sym.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteCompatibilitySym `root-config --cflags --glibs` -lMathMore

ExecuteOverlaySym: plot_pPbPbp_sym_overlay.cpp
	g++ plot_pPbPbp_sym_overlay.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteOverlaySym `root-config --cflags --glibs`

clean:
	rm -f ExecuteCompatibility ExecuteProjectionAudit ExecuteKS ExecuteCompatibilitySym ExecuteOverlaySym
