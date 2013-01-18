/**
 * \file radlength_ana.C
 * \brief Macro runs radiathin length QA task.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
void radlength_ana(Int_t nEvents = 1000000)
{
   TString dir = "data/"; // Output directory
   TString mcFile = dir + "radlen.mc.root"; // MC transport file
   TString parFile = dir + "radlen.params.root"; // Parameter file
   TString radLengthQaFile = dir + "radlen.qa.root"; // MC transport file
   TString resultDir = "/test/";

   TStopwatch timer;
   timer.Start();

   gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
   loadlibs();

   // -----   Reconstruction run   -------------------------------------------
   FairRunAna *run= new FairRunAna();
   run->SetInputFile(mcFile);
   run->SetOutputFile(radLengthQaFile);
   // ------------------------------------------------------------------------

   CbmLitRadLengthQa* radLengthQa = new CbmLitRadLengthQa();
   radLengthQa->SetOutputDir(std::string(resultDir));
   run->AddTask(radLengthQa);

   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   parIo1->open(parFile.Data());
   rtdb->setFirstInput(parIo1);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();
   // ------------------------------------------------------------------------

   // -----   Initialize and run   --------------------------------------------
   run->Init();
   run->Run(0, nEvents);
   // ------------------------------------------------------------------------

   // -----   Finish   -------------------------------------------------------
   timer.Stop();
   std::cout << "Macro finished successfully." << std::endl;
   std::cout << "Output file is " << radLengthQaFile << std::endl;
   std::cout << "Parameter file is " << parFile << std::endl;
   std::cout << "Real time " << timer.RealTime() << " s, CPU time " << timer.CpuTime() << " s" << std::endl;
   // ------------------------------------------------------------------------

//   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
//   gROOT->ProcessLine("basiclibs()");
//   gSystem->Load("libGeoBase");
//   gSystem->Load("libParBase");
//   gSystem->Load("libBase");
//   gSystem->Load("libCbmBase");
//
//   TString inFile = "data/radlen.mc.root";
//   TFile* file = new TFile(inFile);
//   TTree* tree = (TTree*) file->Get("cbmsim") ;
//
//   TClonesArray* radLenArray = new TClonesArray("FairRadLenPoint");
//   tree->SetBranchAddress("RadLen", &radLenArray);
//
//   const Double_t radius = 550;
//   const Int_t nofBins = 100;
//   TH1D* hTotalTrackLengthH1 = new TH1D("hTotalTrackLengthH1", "hTotalTrackLengthH1", nofBins, 0, 10000);
//   TH1D* hTotalRadLenH1 = new TH1D("hTotalRadLenH1", "hTotalRadLenH1", nofBins, 0, 0);
//   TH2D* hTotalRadLenP2 = new TProfile2D("hTotalRadLenP2", "hTotalRadLenP2", nofBins, -radius, radius, nofBins, -radius, radius);
//
////   const Int_t NStations = 12;
////   const Int_t NBins = 1000;
////   const Int_t RMax = 550;
////   TProfile2D* hStaRadLen[NStations];
////
////   for ( Int_t i = 0; i < NStations; ++i ) {
////      TString name = "Radiation Thickness [%],";
////      name += " Station";
////      name += i+1;
////      hStaRadLen[i] = new TProfile2D(name, name, NBins,-RMax,RMax, NBins,-RMax,RMax);
////   }
//
////   Int_t startEvent = 0;
//
////   TVector3 vecs, veco;
////   std::map<Int_t,Int_t> trackHitMap;
//
//   for (Int_t iEvent = 0; iEvent < nEvents; iEvent++) {
//      tree->GetEntry(iEvent);
//      if (0 == iEvent % 10000) {
//         cout << ">>>> Event No " << iEvent << endl;
//      }
//
//      const Int_t nofTracks = 1; // Number of tracks in each event in the simulation. One track is used for simplicity.
//      vector<Double_t> radLengthOnTrack(nofTracks, 0.0); //trackID, vector with points on track
//      vector<Double_t> trackLength(nofTracks, 0.0); //trackID, vector with points on track
//      //vector<Double_t> RadThick(NStations, 0);
//
//      Double_t x, y;
//      for (Int_t i = 0; i < radLenArray->GetEntriesFast(); i++) {
//         FairRadLenPoint* point = (FairRadLenPoint*) radLenArray->At(i);
//
//         TVector3 pos = point->GetPosition();
//         TVector3 posOut = point->GetPositionOut();
//         TVector3 res = posOut - pos;
//
//         const TVector3 middle2 = (posOut + pos);
//         x = middle2.X()/2;
//         y = middle2.Y()/2;
////         const Double_t z = middle2.Z() / 2;
//
//         const Double_t radThick = res.Mag() / point->GetRadLength();
//         radLengthOnTrack[point->GetTrackID()] += radThick;
//         trackLength[point->GetTrackID()] += res.Mag();
//
//         // cout << radThick << endl;
//         // cout << pos.X() << " " << pos.Y() << " " << pos.Z() << " " << endl;
//         // cout << posOut.X() << " " << posOut.Y() << " " << posOut.Z() << " " << endl;
//
////         Int_t iSta =  pos.Z()/10 - 3 + 0.5; // suppose equdistant stations at 30-100 cm
////         Int_t iStaOut = posOut.Z()/10 - 3 + 0.5;
////         if ( iSta != iStaOut ) continue;
////         if ( iSta >= NStations || iSta < 0 ) continue;
////         RadThick[iSta] += radThick;
//      }
//
////      for ( Int_t i = 0; i < NStations; ++i ) {
////         // cout << i << " " << x << " " << y << " " << RadThick[i] << endl;
////         hStaRadLen[i]->Fill( x, y, RadThick[i] * 100 );
////      }
//
//      for (Int_t k = 0; k < radLengthOnTrack.size(); k++) {
//         //if (radLengthOnTrack[k] > 0) {
//            hTotalRadLenH1->Fill(radLengthOnTrack[k] * 100);
//            hTotalTrackLengthH1->Fill(trackLength[k]);
//            hTotalRadLenP2->Fill(x, y, radLengthOnTrack[k] * 100);
//         //}
//      }
//   }
//
//   TCanvas* canvas1 = new TCanvas("hTotalTrackLengthH1", "hTotalTrackLengthH1", 1000, 1000);
//   hTotalTrackLengthH1->Draw();
//
//   TCanvas* canvas2 = new TCanvas("hTotalRadLenH1", "hTotalRadLenH1", 1000, 1000);
//   hTotalRadLenH1->Draw();
//
//   TCanvas* canvas3 = new TCanvas("hTotalRadLenH2", "hTotalRadLenH2", 1000, 1000);
//   hTotalRadLenP2->GetXaxis()->SetTitle("x [cm]");
//   hTotalRadLenP2->GetYaxis()->SetTitle("y [cm]");
//   hTotalRadLenP2->GetZaxis()->SetTitle("radiation thickness [%]");
//   hTotalRadLenP2->Draw("colz");
//
////  TCanvas* can1 = new TCanvas();
////  can1->Divide(NStations/2, 2);
////  gStyle->SetPalette(1);
////  gStyle->SetOptStat(0);
////
////  TFile* rtFile = new TFile("radThick.root", "RECREATE");
////  rtFile->cd();
////
////  for ( Int_t i = 0; i < NStations; ++i ) {
////    can1->cd(i+1);
////    hStaRadLen[i]->GetXaxis()->SetTitle("x [cm]");
////    hStaRadLen[i]->GetYaxis()->SetTitle("y [cm]");
////    //hStaRadLen[i]->GetZaxis()->SetTitle("radiation thickness [%]");
////    hStaRadLen[i]->SetAxisRange(0, 2, "Z");
////    hStaRadLen[i]->Draw("colz");
////    hStaRadLen[i]->Write();
////  }
//////  can1->SaveAs("RadThick.pdf");
//////  can1->SaveAs("RadThick.eps");
//////  can1->SaveAs("RadThick.png");
////  can1->SaveAs("RadThick.gif");
//
//  return 0;
}
