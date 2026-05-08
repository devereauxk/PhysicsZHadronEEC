default: ExecuteCompatibility ExecuteProjectionAudit

ExecuteCompatibility: compute_pPbPbp_compatibility.cpp
	g++ compute_pPbPbp_compatibility.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteCompatibility `root-config --cflags --glibs`

ExecuteProjectionAudit: audit_projection_integrals.cpp
	g++ audit_projection_integrals.cpp -O3 -I. -I /home/kdeverea/PhysicsZHadronEEC/CommonCode/include \
		-o ExecuteProjectionAudit `root-config --cflags --glibs`

clean:
	rm -f ExecuteCompatibility ExecuteProjectionAudit
