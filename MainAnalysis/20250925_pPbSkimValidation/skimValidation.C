#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <iostream>

// example usage
// root -l -b -q "skimValidation.C(\"HiForestMiniAOD_Hijing_merged_DEBUG.root\", \"skimValidation.root\", false)"
// root -l -b -q "skimValidation.C(\"HiForestMiniAOD_Hijing_merged_DEBUG.root\", \"skimValidation_cut.root\", true)"

void skimValidation(const char* inputFile = "HiForestMiniAOD_Hijing_merged_DEBUG.root", 
                    const char* outputFile = "skimValidation.root",
                    bool isForest = true,
                    bool applyEventCuts = true,
                    bool applyTrackCuts = true) {

    // Open the input ROOT file
    TFile* finput = TFile::Open(inputFile, "READ");
    if (!finput || finput->IsZombie()) {
        std::cerr << "Error: Unable to open file " << inputFile << std::endl;
        return;
    }

    // output histogram initialization
    TH1D* hNEvtPassCuts = new TH1D("hNEvtPassCuts", "Number of events passing cuts", 6, 0.5, 6.5);
    hNEvtPassCuts->GetXaxis()->SetBinLabel(1, "Total Events");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(2, "+ CC");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(3, "+ PV");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(4, "+ nVtx>0");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(5, "+ !isFakeVtx");
    hNEvtPassCuts->GetXaxis()->SetBinLabel(6, "+ abs(VZ)<15");

    TH1D* hzY           = new TH1D("hzY", "Z Boson Rapidity", 100, -5, 5);
    TH1D* hzPt          = new TH1D("hzPt", "Z Boson pT", 100, 0, 700);
    TH1D* hgenZY        = new TH1D("hgenZY", "Gen Z Boson Rapidity", 100, -5, 5);
    TH1D* hgenZPt       = new TH1D("hgenZPt", "Gen Z Boson pT", 100, 0, 700);
    TH1D* hEventWeight  = new TH1D("hEventWeight", "Event Weight", 100, 0.8, 1.2);
    TH1D* hZWeight      = new TH1D("hZWeight", "Z Weight", 100, 0.8, 1.2);

    TH1D* htrackPt              = new TH1D("htrackPt", "Track pT", 100, 0, 100);
    TH1D* htrackEta             = new TH1D("htrackEta", "Track Eta", 100, -5, 5);
    TH1D* htrackWeight          = new TH1D("htrackWeight", "Track Weight", 100, 0, 5);
    TH1D* htrackResidualWeight   = new TH1D("htrackResidualWeight", "Track Residual Weight", 100, 0, 5);
    TH1D* hMult                 = new TH1D("hMult", "Event Multiplicity", 100, 0, 500);
    TH1D* htrackPt_trackWeight  = new TH1D("htrackPt_trackWeight", "Track pT weighted by trackWeight", 100, 0, 100);
    TH1D* htrackEta_trackWeight = new TH1D("htrackEta_trackWeight", "Track Eta weighted by trackWeight", 100, -5, 5);
    TH1D* hMult_trackWeight   = new TH1D("hMult_trackWeight", "Event Multiplicity weighted by trackWeight", 100, 0, 500);


    // Define branch variables
    TTree *tree;

    float ZWeight, EventWeight;

    std::vector<float> *zY = nullptr;
    std::vector<float> *zPt = nullptr;
    std::vector<float> *genZY = nullptr;
    std::vector<float> *genZPt = nullptr;
    std::vector<float> *trackPt = nullptr;
    std::vector<float> *trackEta = nullptr;
    std::vector<float> *trackWeight = nullptr;
    std::vector<float> *trackResidualWeight = nullptr;

    // if forest, get forest
    if (isForest) {
        return;
        /*
        tree = (TTree *)finput->Get("hltanalysis/HltTree");
        tree->AddFriend("skimanalysis/HltTree"); // Add the skim tree as a friend
        tree->AddFriend("hiEvtAnalyzer/HiTree"); // Add the event tree as a friend
        tree->AddFriend("ppTracks/trackTree");   // Add the track tree as a friend
        tree->AddFriend("particleFlowAnalyser/pftree"); // Add the particle flow tree as a friend

        // event-level
        tree->SetBranchAddress("nRun", &nRun);
        tree->SetBranchAddress("nEv", &nEv);
        tree->SetBranchAddress("nLumi", &nLumi);
        tree->SetBranchAddress("nVtx", &nVtx);
        tree->SetBranchAddress("hiBin", &hiBin);
        tree->SetBranchAddress("Npart", &Npart);
        tree->SetBranchAddress("Ncoll", &Ncoll);
        tree->SetBranchAddress("hiHF_pf", &hiHF_pf);
        tree->SetBranchAddress("pclusterCompatibilityFilter", &ClusterCompatibilityFilter);
        tree->SetBranchAddress("pprimaryVertexFilter", &PVFilter);

        // vertex-level
        tree->SetBranchAddress("ptSumVtx", &ptSumVtx); // best vertex is the one with the highest ptSum
        tree->SetBranchAddress("xVtx", &xVtx);
        tree->SetBranchAddress("yVtx", &yVtx);
        tree->SetBranchAddress("zVtx", &zVtx);
        tree->SetBranchAddress("xErrVtx", &xErrVtx);
        tree->SetBranchAddress("yErrVtx", &yErrVtx);
        tree->SetBranchAddress("zErrVtx", &zErrVtx);
        tree->SetBranchAddress("isFakeVtx", &isFakeVtx);
        tree->SetBranchAddress("nTracksVtx", &nTracksVtx);
        tree->SetBranchAddress("chi2Vtx", &chi2Vtx);
        tree->SetBranchAddress("ndofVtx", &ndofVtx);

        // track-level
        tree->SetBranchAddress("trkPt", &trkPt);
        tree->SetBranchAddress("trkPtError", &trkPtError);
        tree->SetBranchAddress("trkEta", &trkEta);
        tree->SetBranchAddress("highPurity", &highPurity);
        tree->SetBranchAddress("trkPhi", &trkPhi);
        tree->SetBranchAddress("trkCharge", &trkCharge);
        tree->SetBranchAddress("trkNHits", &trkNHits);
        tree->SetBranchAddress("trkNPixHits", &trkNPixHits);
        tree->SetBranchAddress("trkNLayers", &trkNLayers);
        tree->SetBranchAddress("trkNormChi2", &trkNormChi2);
        tree->SetBranchAddress("pfEnergy", &pfEnergy);
        tree->SetBranchAddress("pfId", &pfId);
        tree->SetBranchAddress("pfE", &pfE);
        tree->SetBranchAddress("pfEta", &pfEta);
        tree->SetBranchAddress("trkDxyAssociatedVtx", &trkDxyAssociatedVtx);
        tree->SetBranchAddress("trkDxyErrAssociatedVtx", &trkDxyErrAssociatedVtx);
        tree->SetBranchAddress("trkDzAssociatedVtx", &trkDzAssociatedVtx);
        tree->SetBranchAddress("trkDzErrAssociatedVtx", &trkDzErrAssociatedVtx);
        tree->SetBranchAddress("trkAssociatedVtxIndx", &trkAssociatedVtxIndx);
        */

    } // if just skim
    else {
        tree = (TTree*)finput->Get("Tree");

        // event-level
        tree->SetBranchAddress("zY", &zY);
        tree->SetBranchAddress("zPt", &zPt);
        tree->SetBranchAddress("genZY", &genZY);
        tree->SetBranchAddress("genZPt", &genZPt);
        tree->SetBranchAddress("EventWeight", &EventWeight);
        tree->SetBranchAddress("ZWeight", &ZWeight);

        // track-level
        tree->SetBranchAddress("trackPt", &trackPt);
        tree->SetBranchAddress("trackEta", &trackEta);
        tree->SetBranchAddress("trackWeight", &trackWeight);
        tree->SetBranchAddress("trackResidualWeight", &trackResidualWeight);

    }

    // Loop over the events
    int nEntries = tree->GetEntries();
    for (int i = 0; i < nEntries; ++i) {

        if (i % 1000 == 0) {
            std::cout << "Processing event " << i << " / " << nEntries << std::endl;
        }

        bool passedCuts = false;

        tree->GetEntry(i);

        /*
        // determine the best vertex if forest
        // if skim, best vertex is already found (VX, VY, VZ)
        int bestVertexIdx = -1;
        if (isForest) {
            for (int j = 0; j < nVtx; ++j) {
                if (((bestVertexIdx == -1 && ptSumVtx->at(j) > 0) || (bestVertexIdx != -1 && ptSumVtx->at(j) > ptSumVtx->at(bestVertexIdx)))) {
                    bestVertexIdx = j;
                }
            }
        }

        // Fill the event count histogram
        hNEvtPassCuts->Fill(1); // Total events

        // Apply the CC filter
        if (ClusterCompatibilityFilter) {
            hNEvtPassCuts->Fill(2); // CC filter passed
        }

        // Apply the PV filter
        if (ClusterCompatibilityFilter && PVFilter) {
            hNEvtPassCuts->Fill(3); // PV filter passed
        }

        // Apply the nVtx filter
        if (ClusterCompatibilityFilter && PVFilter && nVtx > 0) {
            hNEvtPassCuts->Fill(4); // nVtx filter passed
        }

        // Apply the isFakeVtx filter
        if (isForest) {
            if (ClusterCompatibilityFilter && PVFilter && nVtx > 0 && !isFakeVtx->at(0)) {
                hNEvtPassCuts->Fill(5); // isFakeVtx filter passed
                //passedCuts = true;
            }
        } else {
            if (ClusterCompatibilityFilter && PVFilter && nVtx > 0 && !isFakeVtx_skim) {
                hNEvtPassCuts->Fill(5); // isFakeVtx filter passed
                //passedCuts = true;
            }
        }

        // Apply the VZ filter
        if (ClusterCompatibilityFilter && PVFilter && nVtx > 0 && 
            ((isForest && !isFakeVtx->at(0) && fabs(zVtx->at(0)) < 15.0) || 
             (!isForest && !isFakeVtx_skim && fabs(VZ) < 15.0))) {
            hNEvtPassCuts->Fill(6); // VZ filter passed
            passedCuts = true;
        }

        // apply cuts if requested
        if (applyEventCuts && !passedCuts) continue;
        */

        // event-level
        if (zY->size() < 1 ||  genZY->size() < 1) continue;
        hzY->Fill(zY->at(0));
        hzPt->Fill(zPt->at(0));
        hgenZY->Fill(genZY->at(0));
        hgenZPt->Fill(genZPt->at(0));
        hEventWeight->Fill(EventWeight);
        hZWeight->Fill(ZWeight);

        // loop over tracks
        int mult = 0;
        float mult_weighted = 0.0;
        for (int j = 0; j < trackPt->size(); ++j) {

            // apply track cuts
            /*
            if (applyTrackCuts) {

                if(abs(trkCharge->at(j)) != 1)
                    continue;

                if(highPurity->at(j) == false)
                    continue;
                
                if (trkPt->at(j) < 0.1)
                    continue;
                
                double RelativeUncertainty = trkPtError->at(j)/ trkPt->at(j);
                if(trkPt->at(j) > 10 && RelativeUncertainty > 0.1)
                    continue;

                if(fabs(trkDxyAssociatedVtx->at(j)) / trkDxyErrAssociatedVtx->at(j) > 3)
                    continue;

                if(fabs(trkDzAssociatedVtx->at(j)) / trkDzErrAssociatedVtx->at(j) > 3)
                    continue;

                if (fabs(trkEta->at(j)) > 2.4)
                    continue;

                if (trkPt->at(j) > 500)
                    continue;

            }
                    */

            htrackEta->Fill(trackEta->at(j));
            htrackPt->Fill(trackPt->at(j));
            htrackWeight->Fill(trackWeight->at(j));
            htrackResidualWeight->Fill(trackResidualWeight->at(j));
            htrackPt_trackWeight->Fill(trackPt->at(j), trackWeight->at(j));
            htrackEta_trackWeight->Fill(trackEta->at(j), trackWeight->at(j));
            mult++;
            mult_weighted += trackWeight->at(j);
            
        }

        hMult->Fill(mult);
        hMult_trackWeight->Fill(mult, mult_weighted);
        
    }

    // write to output file
    TFile* foutput = new TFile(outputFile, "RECREATE");
    hzY->Write();
    hzPt->Write();
    hgenZY->Write();
    hgenZPt->Write();
    hEventWeight->Write();
    hZWeight->Write();
    htrackPt->Write();
    htrackEta->Write();
    htrackWeight->Write();
    htrackResidualWeight->Write();
    hMult->Write();
    htrackPt_trackWeight->Write();
    htrackEta_trackWeight->Write();
    hMult_trackWeight->Write();

    foutput->Close();
    finput->Close();

}
