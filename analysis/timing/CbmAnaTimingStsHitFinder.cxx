/** CbmAnaTimingStsHitFinder.cxx
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2011-07-07
 **/

#include "CbmAnaTimingStsHitFinder.h"
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

#include <map>
#include <vector>
using std::vector;
using std::multimap;

// -----   Default constructor   -------------------------------------------
CbmAnaTimingStsHitFinder::CbmAnaTimingStsHitFinder():
  fEvent(0),fDigiFile("much_digi.root"),fHistoName("histo.root")
{}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmAnaTimingStsHitFinder::CbmAnaTimingStsHitFinder(const char* name, TString histoName):
  FairTask(name),fEvent(0),fHistoName(histoName)
{}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmAnaTimingStsHitFinder::~CbmAnaTimingStsHitFinder(){}
// -------------------------------------------------------------------------

// -----  SetParContainers -------------------------------------------------
void CbmAnaTimingStsHitFinder::SetParContainers(){
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
InitStatus CbmAnaTimingStsHitFinder::Init(){
  FairRootManager* fManager = FairRootManager::Instance();
  fDigis       = (TClonesArray*) fManager->GetObject("StsDigi");
  fDigiMatches = (TClonesArray*) fManager->GetObject("StsDigiMatch");
  fHits        = (TClonesArray*) fManager->GetObject("StsHit");
  fMcEpoch     = (CbmMCEpoch*)   fManager->GetObject("MCEpoch.");
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void CbmAnaTimingStsHitFinder::Exec(Option_t* opt){
  Int_t nHits = fHits->GetEntriesFast();
  printf("nHits=%i\n",nHits);
}
// -------------------------------------------------------------------------


// -----   Public method Finish   ------------------------------------------
void CbmAnaTimingStsHitFinder::Finish(){
  TFile* f = new TFile(fHistoName.Data(),"RECREATE");
  f->Close();
}
// -------------------------------------------------------------------------

ClassImp(CbmAnaTimingStsHitFinder);
