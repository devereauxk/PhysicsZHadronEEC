void SetPad(TPad *P)
{
   if(P == nullptr)
      return;
   P->SetLeftMargin(0);
   P->SetTopMargin(0);
   P->SetRightMargin(0);
   P->SetBottomMargin(0);
   P->SetTickx(false);
   P->SetTicky(false);
   P->Draw();
}

void SetWorld(TH2D *H)
{
   if(H == nullptr)
      return;
   H->SetStats(0);
   H->GetXaxis()->SetTickLength(0);
   H->GetYaxis()->SetTickLength(0);
}

void SetAxis(TGaxis *A)
{
   if(A == nullptr)
      return;
   A->SetLabelFont(42);
   A->SetLabelSize(0.035);
   A->SetMaxDigits(6);
   A->SetMoreLogLabels();
   A->Draw();
}

TH1D *GetHistogram(TFile *F, string ToPlot, string Tag, int Color)
{
   TH1D *H  = (TH1D *)F->Get(Form("%s_%s", ToPlot.c_str(), Tag.c_str()));
   if(H == nullptr)
      return nullptr;

   static int ID = 0;
   ID = ID + 1;
   H = (TH1D *)H->Clone(Form("H%d", ID));

   double Integral = 1;//HN->GetBinContent(1);

   H->Scale(1 / Integral);

   H->SetStats(0);
   H->SetMarkerStyle(20);
   H->SetLineWidth(2);
   H->SetMarkerSize(2);
   
   H->SetMarkerColor(Color);
   H->SetLineColor(Color);
   
   return H;
}


