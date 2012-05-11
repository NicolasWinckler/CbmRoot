#if !defined(__CINT__) || defined(__MAKECINT__)
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TVector3.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TROOT.h"

#include "../../base/FairRadLenPoint.h"

#include <iostream>
#include <vector>
using std::cout;
using std::endl;
using std::vector;
#endif

Int_t ana_radlength(Int_t nEvents=100)
{

  TString dir = gSystem->Getenv("VMCWORKDIR");
  TString file = dir + "/gconfig/basiclibs.C";
  gROOT->LoadMacro(file);
//  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  gROOT->ProcessLine("basiclibs()");
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");

  TString inFile = "data/test.mc.root";
  TFile* f = new TFile(inFile);
  TTree *t=(TTree *) f->Get("cbmsim") ;

  TClonesArray* radlen_array = new TClonesArray("FairRadLenPoint");
  t->SetBranchAddress("RadLen", &radlen_array);

  TH1D* hisRadLen = new TH1D("hisRadLen","Radiation Length", 1000,0,100);

  int startEvent = 0;

  TVector3 vecs,veco;
  std::map<int,int> trackHitMap;

  for (Int_t j=startEvent; j<(nEvents+startEvent) && j<t->GetEntriesFast(); j++) {
    t->GetEntry(j);
    if ( 0 == j%1000 ) { 
      cout<<">>>> Event No "<<j<<endl;
    }

    std::vector<double> RadLengthOnTrack (10000,0.0); //trackID, vector with points on track
    std::vector<double> TrackLength (10000,0.0); //trackID, vector with points on track

    for (Int_t i=0; i<radlen_array->GetEntriesFast(); i++) {
//      cout<<"Point No "<<i<<endl;

      FairRadLenPoint *point=(FairRadLenPoint*)radlen_array->At(i);

//      cout << "Track ID: " << point->GetTrackID() << std::endl;

      TVector3 pos, posOut, res;
      pos = point->GetPosition();
      posOut = point->GetPositionOut();
      res = posOut - pos;

      RadLengthOnTrack[point->GetTrackID()] += res.Mag()/point->GetRadLength()*100;
      TrackLength[point->GetTrackID()] += res.Mag();
    }

    for (int k = 0; k < RadLengthOnTrack.size(); k++) {
      if (RadLengthOnTrack[k] > 0) {
//         std::cout << "Full TrackLength: " << TrackLength[k] << std::endl;
//         std::cout << "Full RadiationLength: " << RadLengthOnTrack[k] << std::endl;
	 hisRadLen->Fill(RadLengthOnTrack[k]);
      }
    }
  }

  TCanvas* can1 = new TCanvas();
  gStyle->SetPalette(1);
  can1->cd(1);
  hisRadLen->DrawCopy();

  return 0;
}
