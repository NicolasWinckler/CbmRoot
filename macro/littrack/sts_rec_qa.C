
#include "../../littrack/cbm/utils/CbmLitDrawHist.cxx"
#include "../../littrack/std/utils/CbmLitUtils.cxx"

const TString dir1  = "/data.local1/andrey/events/std_electron/";
const TFile *file1 = new TFile(dir1 + "global.tracks.0000.root");
const TString dir2  = "/data.local1/andrey/events/v09a_electron/";
const TFile *file2 = new TFile(dir2 + "global.tracks.0000.root");

const Int_t NEVENTS = 500;
const std::string IMAGEDIR = "./test/";

void sts_rec_qa()
{
   SetStyles();

   file1->cd();
   DrawStsAccRec("rec_qa_sts_mom_acc_rec_tracks_all", "hStsMomAccAll", "hStsMomRecAll", "hStsMomEffAll", "Momentum [GeV/c]");
   DrawStsAccRec("rec_qa_sts_mom_acc_rec_tracks_electron", "hStsMomAccElectron", "hStsMomRecElectron", "hStsMomEffElectron", "Momentum [GeV/c]");
   DrawStsAccRec("rec_qa_sts_np_acc_rec_tracks_all", "hStsNpAccAll", "hStsNpRecAll", "hStsNpEffAll", "Number of points");
   DrawStsAccRec("rec_qa_sts_np_acc_rec_tracks_electron", "hStsNpAccElectron", "hStsNpRecElectron", "hStsNpEffElectron", "Number of points");

   DrawStsEff("rec_qa_sts_mom_eff", "hStsMomEffAll", "hStsMomEffElectron", "Momentum [GeV/c]");
   DrawStsEff("rec_qa_sts_np_eff", "hStsNpEffAll", "hStsNpEffElectron", "Number of points");

   DrawMom();
}

void DrawStsAccRec(
      const std::string& canvasName,
      const std::string& accHist,
      const std::string& recHist,
      const std::string& effHist,
      const std::string& xName)
{
   TCanvas *canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
   canvas->SetGrid();

   TH1* acc1 = (TH1*)file1->Get(accHist.c_str());
   TH1* rec1 = (TH1*)file1->Get(recHist.c_str());
   TH1* eff1 = (TH1*)file1->Get(effHist.c_str());

   acc1->Scale(1./NEVENTS);
   rec1->Scale(1./NEVENTS);

   std::string hname1, hname2;
   hname1 = "Accepted";
   hname2 = "Reconstructed";
   canvas->cd();
   DrawHist1D(acc1, rec1, NULL, NULL,
         "STS tracks", xName, "Tracks/event", hname1, hname2, "", "",
         kLitLinearScale, kLitLinearScale, true, 0.7, 0.8, 0.99, 0.99);

 //  SaveCanvasAsImage(canvas, IMAGEDIR);
}

void DrawMom()
{
   TCanvas *canvas = new TCanvas("rec_qa_sts_mom_res", "rec_qa_sts_mom_res", 600, 500);
   canvas->SetGrid();
   canvas->cd();

   file1->cd();
   TH2F* mom1 = (TH2F*)file1->Get("fhStsMomresVsMom");
   mom1->FitSlicesY();

   TH1* momslice1 = gDirectory->Get("fhStsMomresVsMom_2");
   momslice1->Rebin(2);
   momslice1->Scale(1./2.);

   file2->cd();
   TH2F* mom2 = (TH2F*)file2->Get("fhStsMomresVsMom");
   mom2->FitSlicesY();

   TH1* momslice2 = gDirectory->Get("fhStsMomresVsMom_2");
   momslice2->Rebin(2);
   momslice2->Scale(1./2.);

   DrawHist1D(momslice1, momslice2, NULL, NULL,
            "Momentum resolution", "Momentum [GeV/c]", "Sigma [%]", "v11a", "v09a", "", "",
            kLitLinearScale, kLitLinearScale, true, 0.7, 0.8, 0.99, 0.99);
}

void DrawStsEff(
      const std::string& canvasName,
      const std::string& eff1Hist,
      const std::string& eff2Hist,
      const std::string& xName)
{
   TCanvas *canvas = new TCanvas(canvasName.c_str(), canvasName.c_str(), 600, 500);
   canvas->SetGrid();

   TH1* eff1 = (TH1*)file1->Get(eff1Hist.c_str());
   TH1* eff2 = (TH1*)file1->Get(eff2Hist.c_str());

   eff1->SetMinimum(0.);
   eff1->SetMaximum(100.);

   std::string hname1, hname2;
   hname1 = "All";
   hname2 = "Electrons";
   canvas->cd();
   DrawHist1D(eff1, eff2, NULL, NULL,
         "Efficiency", xName, "Efficiency [%]", hname1, hname2, "", "",
         kLitLinearScale, kLitLinearScale, true, 0.7, 0.8, 0.99, 0.99);

 //  SaveCanvasAsImage(canvas, IMAGEDIR);
}
