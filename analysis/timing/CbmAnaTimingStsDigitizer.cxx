/** CbmAnaTimingStsDigitizer.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-07-07
 **/

#include "CbmAnaTimingStsDigitizer.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairMCPoint.h"
#include "CbmMCEpoch.h"
#include "TClonesArray.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"


// -----   Default constructor   -------------------------------------------
CbmAnaTimingStsDigitizer::CbmAnaTimingStsDigitizer():
  fEvent(0),fHistoName("histo.root"),
  fDigiScheme(new CbmStsDigiScheme()),
  fStationId(1),fSectorId(62),fSideId(0),fChannelId(0),fEpoch(1)
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmAnaTimingStsDigitizer::CbmAnaTimingStsDigitizer(const char* name, TString histoName):
  FairTask(name),fEvent(0),fHistoName(histoName),
  fDigiScheme(new CbmStsDigiScheme()),
  fStationId(1),fSectorId(62),fSideId(0),fChannelId(0),fEpoch(1)
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmAnaTimingStsDigitizer::~CbmAnaTimingStsDigitizer(){}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingStsDigitizer::SetParContainers(){
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb = ana->GetRuntimeDb();
  rtdb->getContainer("FairBaseParSet");
  rtdb->getContainer("CbmGeoPassivePar");
  rtdb->getContainer("CbmGeoStsPar");
  rtdb->getContainer("CbmGeoMuchPar");
  rtdb->getContainer("CbmFieldPar");
  // Get STS geometry parameter container
  fGeoPar = (CbmGeoStsPar*) rtdb->getContainer("CbmGeoStsPar");

  // Get STS digitisation parameter container
  fDigiPar = (CbmStsDigiPar*) rtdb->getContainer("CbmStsDigiPar");
}
// -------------------------------------------------------------------------


// -----   Public method Init (abstract in base class)  --------------------
InitStatus CbmAnaTimingStsDigitizer::Init(){
  FairRootManager* fManager = FairRootManager::Instance();
  // Build digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;
  
  fStsDigis       = (TClonesArray*) fManager->GetObject("StsDigi");
  fStsDigiMatches = (TClonesArray*) fManager->GetObject("StsDigiMatch");
  if (fEpoch)
    fMcEpoch         = (CbmMCEpoch*)   fManager->GetObject("MCEpoch.");
  else 
    fStsPoints   = (TClonesArray*) fManager->GetObject("StsPoint");

  
  fDigiTimeAll = new TH1D("fDigiTimeAll","DigiTimeAll",1000,0,1000);
  fDigiTimeAll->SetLineColor(kBlue);
  
  fDigiTimeSector = new TH1D("fDigiTimeSector","DigiTimeSector",1000,0,1000);
  fDigiTimeSector->SetLineColor(kBlue);
  
  fDigiVsSector = new TH1D("fDigiVsSector","DigiVsSector",100,0,100);
  fDigiVsSector->SetLineColor(kBlue);
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingStsDigitizer::Exec(Option_t* opt){
  if (fEpoch) fStsPoints = fMcEpoch->GetPoints(kMUCH);
  
  Int_t nStsPoints = fStsPoints ? fStsPoints->GetEntriesFast() : 0;
  Int_t nStsDigis  = fStsDigis  ? fStsDigis->GetEntriesFast() : 0;
  if (fVerbose>-1) printf(" Event: %4i",fEvent++);
  if (fVerbose>0) printf(" nMuchPoints: %4i",nStsPoints);
  if (fVerbose>0) printf(" nMuchDigis: %4i",nStsDigis);
  if (fVerbose>-1) printf("\n");
   
  for (Int_t i=0;i<nStsPoints;i++){
    FairMCPoint* point = (FairMCPoint*) fStsPoints->At(i);
    Double_t x = point->GetX();
    Double_t y = point->GetY();
    Double_t t = point->GetTime();
    if (fVerbose>2) printf("  Sts point: %5i time=%8.1f",i,t);
    if (fVerbose>2) printf(" Detector Id: %8i",point->GetDetectorID());
    if (fVerbose>2) printf("\n");
  }
  
  for (Int_t i=0;i<nStsDigis;i++) {
    CbmStsDigi* digi = (CbmStsDigi*) fStsDigis->At(i);
    Int_t station = digi->GetStationNr();
    Int_t sector  = digi->GetSectorNr();
    Int_t side    = digi->GetSide();
    Int_t channel = digi->GetChannelNr();
    Int_t time    = digi->GetTime();
    Int_t detId   = digi->GetDetectorId();
    
    if (fVerbose>2) printf("  Digi: %5i time=%8i detId=%8i",i,time,detId);
    if (fVerbose>2) printf(" station=%i", station);
    if (fVerbose>2) printf(" sector=%i", sector);
    if (fVerbose>2) printf(" side=%i", side);
    if (fVerbose>2) printf("\n");
    fDigiTimeAll->Fill(time);
    if (station==fStationId && side==fSideId) fDigiVsSector->Fill(sector);
    if (station==fStationId && side==fSideId && sector==fSectorId)  fDigiTimeSector->Fill(time);
  }
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingStsDigitizer::Finish(){
  TCanvas* c1 = new TCanvas("c1","c1",1000,800);
  c1->Divide(1,2);
  c1->cd(1);
  fDigiTimeAll->Draw();
  c1->cd(2);
  fDigiTimeSector->Draw();

  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
  fDigiTimeAll->Write();
  fDigiVsSector->Write();
  fDigiTimeSector->Write();
  f->Close();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaTimingStsDigitizer);
