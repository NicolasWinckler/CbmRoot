/** CbmMuchMaximalChargeQa.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 01.02.08
 *@version 1.0
 **
 ** CBM task class to find upper charge limit for pads.
 ** Task level RECO.
 **/

#include "CbmMuchMaximalChargeQa.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchDigi.h"
#include "CbmMuchPoint.h"
#include "CbmMuchPad.h"

#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmRootManager.h"
#include "CbmMCTrack.h"

#include "TCanvas.h"
#include "TF1.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include <iostream>

using std::cout;
using std::endl;

//-----  Standard constructor  ----------------------------------
CbmMuchMaximalChargeQa::CbmMuchMaximalChargeQa(const char *name, Int_t verbose)
  :CbmTask(name, verbose){
  fEvent = 0;
  fDigiScheme = CbmMuchDigiScheme::Instance();
}
//---------------------------------------------------------------

//-----  Destructor  --------------------------------------------
CbmMuchMaximalChargeQa::~CbmMuchMaximalChargeQa(){
  if(fDigiScheme) fDigiScheme->FreeInstance();
}
//---------------------------------------------------------------

//-----  Exec method  -------------------------------------------
void CbmMuchMaximalChargeQa::Exec(Option_t* option){
  Bool_t verbose = (fVerbose>1);
  printf("CbmMuchMaximalChargeQa::Exec()");
  printf("  Event:%i\n",++fEvent);

  // Fill charge histogram
  Int_t nDigis = fDigis->GetEntriesFast();
  for(Int_t iDigi = 0; iDigi < nDigis; iDigi++){
    CbmMuchDigi* digi = (CbmMuchDigi*)fDigis->At(iDigi);
    CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*)fDigiMatches->At(iDigi);

    CbmMuchPad* pad = fDigiScheme->GetPad(digi);
    Double_t dx = pad->GetDx();
    Double_t dy = pad->GetDy();
    Double_t area = dx*dy;

    if(fQHistsMap.find(area) == fQHistsMap.end()){
      char *hName;
      sprintf(hName, "hist%f", area);
      fQHistsMap[area] = new TH1D(hName, "Charge collected by pads", 100, 0, 500000);
    }

    Int_t nPoints = digiMatch->GetNPoints();
    for(Int_t iPoint = 0; iPoint < nPoints; iPoint++){
      CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(iPoint);

      // Get particle's characteristics
      Int_t trackID = point->GetTrackID();
      if(trackID < 0) continue;
      CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(trackID);
      if(NULL == mcTrack) continue;
      Int_t pdgCode = mcTrack->GetPdgCode();
      if(fabs(pdgCode) == 13){
	// Fill charge
	UInt_t charge = digi->GetCharge();
	map<Double_t, TH1D*>::iterator it = fQHistsMap.find(area);
	(*it).second->Fill(charge);
	break;
      }
    }
  }
}
//---------------------------------------------------------------

//-----  Finish method  -----------------------------------------
void CbmMuchMaximalChargeQa::Finish(){
  for(map<Double_t, TH1D*>::iterator iter = fQHistsMap.begin(); iter != fQHistsMap.end(); iter++){
    Double_t area = (*iter).first;
    TH1D* hist = (*iter).second;
    char* cName;
    sprintf(cName, "Area %f cm^{2}", area);
    TCanvas *c1 = new TCanvas(cName, cName, 10, 10, 500, 500);
    hist->Draw();

    Double_t integral = hist->Integral();
    for(Int_t iBin = 0; iBin < hist->GetNbinsX(); iBin++){     
      Double_t int1 = hist->Integral(iBin, hist->GetNbinsX());
      if(int1/integral < 0.03){
	cout << cName << ": UpperThresholdCharge = " << 5000*iBin << endl;
	break;
      }
    }
  }

}
//---------------------------------------------------------------

//-----  SetParContainers method  -------------------------------
void CbmMuchMaximalChargeQa::SetParContainers(){
  // Get run and runtime database
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get MUCH geometry parameter container
  fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");

  // Get MUCH digitisation parameter container
  fDigiPar = (CbmMuchDigiPar*) db->getContainer("CbmMuchDigiPar");
}
//---------------------------------------------------------------

//-----  Init method  -------------------------------------------
InitStatus CbmMuchMaximalChargeQa::Init(){
  // Get input arrays
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No CbmRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
  fPoints = (TClonesArray*)  ioman->GetObject("MuchPoint");
  fDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");
  fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");

  // Build digitization scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    return kSUCCESS;
  }
  return kERROR;
}
//---------------------------------------------------------------

//-----  ReInit method  -----------------------------------------
InitStatus CbmMuchMaximalChargeQa::ReInit(){
  // Clear digitization scheme
  fDigiScheme->Clear();

  // Build new digitization scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    return kSUCCESS;
  }

  return kERROR;
}
//---------------------------------------------------------------


ClassImp(CbmMuchMaximalChargeQa)
