// -------------------------------------------------------------------------
// -----                CbmPsdHitProducer source file             -----
// -----                  Created 15/05/12  by     Alla                -----
// -------------------------------------------------------------------------
#include <iostream>

#include "TClonesArray.h"

#include "FairRootManager.h"

#include "CbmPsdDigi.h"
#include "CbmPsdHitProducer.h"
#include "CbmPsdHit.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdHitProducer::CbmPsdHitProducer() :
  FairTask("Ideal Psd Hit Producer",1),
  fHitArray(NULL),
  fDigiArray(NULL),
  fNHits(0)
{ 
  //  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdHitProducer::~CbmPsdHitProducer() 
{
  if ( fDigiArray ) {
    fDigiArray->Delete();
    delete fDigiArray;
  }
  if ( fHitArray ) {
    fHitArray->Delete();
    delete fHitArray;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmPsdHitProducer::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- CbmPsdHitProducer::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject("PsdDigi");
  if ( ! fDigiArray ) {
    cout << "-W- CbmPsdHitProducer::Init: "
	 << "No PSD digits array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("CbmPsdHit", 1000);
  ioman->Register("PsdHit", "PSD", fHitArray, kTRUE);

  fHitArray->Dump();
  cout << "-I- CbmPsdHitProducer: Intialisation successfull " << kSUCCESS<< endl;
  return kSUCCESS;

}


// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmPsdHitProducer::Exec(Option_t* opt) {


  // Reset output array
   if ( ! fDigiArray ) Fatal("Exec", "No PsdDigi array");
   Reset();
   
  // Declare some variables
  CbmPsdDigi* dig = NULL;
  Float_t edep[44];
 
  for (Int_t imod=0; imod<44; imod++)  edep[imod]=0;
 
  // Loop over PsdDigits
  Int_t nDigi = fDigiArray->GetEntriesFast();
  cout<<" nDigits "<<nDigi<<endl;
  for (Int_t idig=0; idig<nDigi; idig++) {
    dig = (CbmPsdDigi*) fDigiArray->At(idig);
    if ( ! dig) continue;
    Int_t mod = dig->GetModuleID();
    Int_t sec = dig->GetSectionID();
    edep[mod] += dig->GetEdep(sec,mod);
   }// Loop over MCPoints



  for (Int_t imod=0; imod<44; imod++) {
    if (edep[imod]>0) {
      new ((*fHitArray)[fNHits]) CbmPsdHit(imod, edep[imod]);
      fNHits++;
      //    cout<<"CbmPsdHitProducer "<<fNHits<<" "<<imod<<" "<<edep[imod]<<endl;
    }
  }   
  // }//module

  // Event summary
  cout << "-I- CbmPsdHitProducer: " <<fNHits<< " CbmPsdHits created." << endl;

}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmPsdHitProducer::Reset() {
  fNHits = 0;
  if ( fHitArray ) fHitArray->Delete();
  
}


ClassImp(CbmPsdHitProducer)
