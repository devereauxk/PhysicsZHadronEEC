

default: TestRun

Prepare:
	mkdir -p Samples
	# this is in lxplus
	ln -sf /eos/cms/store/group/phys_heavyions/chenyi/PPb2016/Forest/DYtoMuMu_M-30_pPb-EmbEPOS_8p16_Powheg/pPb816Summer16DR-pPbEmb_80X_mcRun2_pA_v4-v2/AODSIM/DYtoMuMu_M-30_pPb-EmbEPOS_8p16_Powheg/20250424_ZHadronPbPMCWithMuTreeGen/250424_175151/0000// Samples/PAMC
	ln -sf /eos/cms/store/group/phys_heavyions/chenyi/PPb2016/Forest/DYtoMuMu_M-30_PbP-EmbEPOS_8p16_Powheg/pPb816Summer16DR-PbPEmb_80X_mcRun2_pA_v4-v1/AODSIM/DYtoMuMu_M-30_PbP-EmbEPOS_8p16_Powheg/20250424_ZHadronPPbMCWithMuTreeGen/250424_175137/0000// Samples/APMC
	ln -sf /eos/cms/store/group/phys_heavyions/chenyi/PPb2016/Forest/PASingleMuon/PARun2016C-PromptReco-v1/AOD/PASingleMuon/20250414_ZHadronSingleMuonWithMuTree_TriggerGated_MuTree/250419_234027// Samples/PAData

TestRun: Execute
	./Execute --Input Samples/PAMC/HiForestAOD_1.root --Output SkimReco.root --MinZPT 20 --Fraction 0.05 \
		--TrackEfficiencyPath ../Corrections/Tracking/Hijing_8TeV_dataBS.root \
		--TrackEfficiencyPathLoose ../Corrections/Tracking/Hijing_8TeV_MB_eff_v3_loose.root \
		--TrackEfficiencyPathTight ../Corrections/Tracking/Hijing_8TeV_MB_eff_v3_tight.root \
		--DoGenLevel false --DoTrackResidual false --WriteAllTrackSelectionTrees true
	./Execute --Input Samples/PAMC/HiForestAOD_1.root --Output SkimGen.root --MinZPT 20 --Fraction 0.05 \
		--TrackEfficiencyPath ../Corrections/Tracking/Hijing_8TeV_dataBS.root --DoGenLevel true \
		--DoTrackResidual false

RunValidation: AuditMultitree Execute
	./run_multitree_validation.sh

Execute: ReduceForest.cpp
	g++ ReduceForest.cpp -o Execute -I./include \
		`root-config --cflags --glibs` \
		-I$(ProjectBase)/CommonCode/include $(ProjectBase)/CommonCode/library/Messenger.o

AuditMultitree: AuditMultitree.cpp
	g++ AuditMultitree.cpp -o AuditMultitree -I./include \
		`root-config --cflags --glibs` \
		-I$(ProjectBase)/CommonCode/include $(ProjectBase)/CommonCode/library/Messenger.o
