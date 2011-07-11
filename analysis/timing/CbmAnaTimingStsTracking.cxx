/** CbmAnaTimingStsTracking.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-07-11
 **/

#include "CbmAnaTimingStsTracking.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairMCPoint.h"
#include "CbmMCEpoch.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h"
#include "TClonesArray.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "CbmStsHit.h"
#include "TVector3.h"
#include <map>
#include <vector>
#include "CbmStsTrack.h"

using std::vector;
using std::multimap;

// -----   Default constructor   -------------------------------------------
CbmAnaTimingStsTracking::CbmAnaTimingStsTracking():
    FairTask("Task"),fEvent(0),fHistoName("histo.root")
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmAnaTimingStsTracking::CbmAnaTimingStsTracking(const char* name, TString histoName):
  FairTask(name),fEvent(0),fHistoName(histoName)
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmAnaTimingStsTracking::~CbmAnaTimingStsTracking(){}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingStsTracking::SetParContainers(){
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmGeoMuchPar");
  rtdb->getContainer("CbmFieldPar");
}
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmAnaTimingStsTracking::Init(){
  FairRootManager* fManager = FairRootManager::Instance();
  fDigis       = (TClonesArray*) fManager->GetObject("StsDigi");
  fDigiMatches = (TClonesArray*) fManager->GetObject("StsDigiMatch");
  fHits        = (TClonesArray*) fManager->GetObject("StsHit");
  fMcEpoch     = (CbmMCEpoch*)   fManager->GetObject("MCEpoch.");
  fStsTracks   = (TClonesArray*) fManager->GetObject("StsTrack");
  fTrackTime   = new TH1D("fTrackTime","fTrackTime",1000,0,1000);
  fTrackTime->SetLineColor(kBlue);
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingStsTracking::Exec(Option_t* opt){
  Int_t nTracks = fStsTracks->GetEntriesFast();
  printf("nTracks=%i\n",nTracks);
  for (Int_t i=0;i<nTracks;i++){
    CbmStsTrack* track = (CbmStsTrack*) fStsTracks->At(i);
    Double_t t = track->GetTime();
    fTrackTime->Fill(t);
  }
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingStsTracking::Finish(){
  TCanvas* c1 = new TCanvas("c1","c1",1000,400);
  fTrackTime->Draw();
  
  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
  fTrackTime->Write();
  f->Close();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaTimingStsTracking);
