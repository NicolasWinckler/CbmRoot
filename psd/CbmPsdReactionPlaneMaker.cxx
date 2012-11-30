// -------------------------------------------------------------------------
// -----                CbmPsdReactionPlaneMaker.h source file             -----
// -----                  Created 15/05/12  by     Alla &Marina                -----
// -------------------------------------------------------------------------
#include <iostream>

#include "TClonesArray.h"

#include "FairRootManager.h"

#include "CbmPsdReactionPlaneMaker.h"
#include "CbmPsdHit.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdReactionPlaneMaker::CbmPsdReactionPlaneMaker() :
  FairTask("PsdReactionPlaneMaker",1),
  fHitArray(NULL)
 { 
  //  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdReactionPlaneMaker::~CbmPsdReactionPlaneMaker() 
{
   if ( fHitArray ) {
    fHitArray->Delete();
    delete fHitArray;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmPsdReactionPlaneMaker::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- CbmPsdReactionPlaneMaker::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray*) ioman->GetObject("PsdHit");
  if ( ! fHitArray ) {
    cout << "-W- CbmPsdReactionPlaneMaker::Init: "
	 << "No PSD hits array!" << endl;
    return kERROR;
  }

   cout << "-I- CbmPsdReactionPlaneMaker: Intialisation successfull " << kSUCCESS<< endl;
  return kSUCCESS;

}


// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmPsdReactionPlaneMaker::Exec(Option_t* opt) {


  // Reset output array
   if ( ! fHitArray ) Fatal("Exec", "No PsdHit array");
   //  Reset();
   
  // Declare some variables
  CbmPsdHit* hit = NULL;
  Float_t edep[44];
 
  for (Int_t imod=0; imod<44; imod++)  edep[imod]=0;
 
  // Loop over PsdHits
  Int_t nHits = fHitArray->GetEntriesFast();
  cout<<" nHits "<<nHits<<endl;
  for (Int_t ihit=0; ihit<nHits; ihit++) {
    hit = (CbmPsdHit*) fHitArray->At(ihit);
    if ( !hit) continue;
    Int_t mod = hit->GetModuleID();
     edep[mod] = hit->GetEdep(mod);
       cout<<"CbmPsdReactionPlaneMaker "<< ihit<<" mod "<< mod<<" edep "<<hit->GetEdep(mod)<<endl;
  }// Loop over hits
  
  
  //  hit->SetRP(555);
  // hit->SetB(111);
  
  // }//module

  // Event summary
  cout << "-I- CbmPsdReactionPlaneMaker: " << " RP CbmPsdHits created." << endl;

}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmPsdReactionPlaneMaker::Reset() {
  fNHits = 0;
  if ( fHitArray ) fHitArray->Delete();
  
}


ClassImp(CbmPsdReactionPlaneMaker)
