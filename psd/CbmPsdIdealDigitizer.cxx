// -------------------------------------------------------------------------
// -----                CbmPsdIdealDigitizer source file             -----
// -----                  Created 15/05/12  by     Alla & modified by SELIM -----
// -------------------------------------------------------------------------
#include <iostream>

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "CbmPsdDigi.h"
#include "CbmPsdIdealDigitizer.h"
//#include "CbmZdcPoint.h"
#include "CbmPsdPoint.h"
#include "TMath.h"

using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmPsdIdealDigitizer::CbmPsdIdealDigitizer() :
  FairTask("Ideal Psd Digitizer",1),
  fPointArray(NULL),
  fDigiArray(NULL),
  fNDigis(0)
{ 
  //  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmPsdIdealDigitizer::~CbmPsdIdealDigitizer() 
{
  if ( fDigiArray ) {
    fDigiArray->Delete();
    delete fDigiArray;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmPsdIdealDigitizer::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman )
  {
      LOG(FATAL) << "CbmPsdIdealDigitizer::Init: RootManager not instantised!" << FairLogger::endl;    // SELIM: precaution
      return kFATAL;
  }

  // Get input array
  //fPointArray = (TClonesArray*) ioman->GetObject("ZDCPoint");
  fPointArray = (TClonesArray*) ioman->GetObject("PsdPoint");
  if ( ! fPointArray )
  {
      LOG(FATAL) << "CbmPsdIdealDigitizer::Init: No PSDPoint array!" << FairLogger::endl;             // SELIM: precaution
      return kERROR;
  }

  // Create and register output array
  fDigiArray = new TClonesArray("CbmPsdDigi", 1000);
  ioman->Register("PsdDigi", "PSD", fDigiArray, kTRUE);

  cout << "-I- CbmPsdIdealDigitizer: Intialisation successfull " << kSUCCESS<< endl;
  return kSUCCESS;

}


// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmPsdIdealDigitizer::Exec(Option_t* opt) {

  cout<<" CbmPsdIdealDigitizer::Exec begin "<<endl;
  // Reset output array
  if ( ! fDigiArray ) Fatal("Exec", "No PsdDigi array");
  Reset();  // SELIM: reset!!!
   
  // Declare some variables
  //CbmZdcPoint* point = NULL;
  CbmPsdPoint* point = NULL;
  Int_t modID   = -1;        // module ID
  Int_t scinID = -1;        // #sciillator
  Double_t x, y, z;         // Position
  Double_t edep[100][100];                      // SELIM: can include up to 100 modules & 100 layers (can be extended) 
  memset(edep, 0, 10000*sizeof(Double_t));

  TVector3 pos;       // Position vector
  fNDigis=0;

  for (Int_t imod=0; imod<100; imod++)          // SELIM: can include up to 100 modules & 100 layers (can be extended)
  {
      for (Int_t isec=0; isec<100; isec++)
      {
	  edep[isec][imod] = 0.;
      }
  }

  // Loop over ZdcPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  cout<<" nPoints "<<nPoints<<endl;

  Int_t modID_min, modID_max;
  modID_min = 100;
  modID_max = 0;

  Int_t sec;  

  for (Int_t iPoint=0; iPoint<nPoints; iPoint++)
  {
      //point = (CbmZdcPoint*) fPointArray->At(iPoint);
      point = (CbmPsdPoint*) fPointArray->At(iPoint);
      if ( ! point ) continue;

      // Detector ID
      modID = point->GetModuleID();              // SELIM: correction scintID <-> modID !!!!!!
      scinID = point->GetDetectorID();

      sec = Int_t((scinID - 1)/6) + 1;           // SELIM: section number from 1 -> 10

      edep[sec][modID] += (Double_t) point->GetEnergyLoss(); // SELIM: overcast in double!      
      
      //if ( sec > modID_max) modID_max = sec;
      //if ( sec < modID_min) modID_min = sec;
  }// Loop over MCPoints

  //cout << "modID in: " << modID_min << ", " << modID_max << endl;

  for (Int_t imod=0; imod<100; imod++)           // SELIM: can include up to 100 modules 
  {                  
      for (Int_t isec=0; isec<100; isec++)
      {	
	if (edep[isec][imod]>0.)
	{
	  new ((*fDigiArray)[fNDigis]) CbmPsdDigi(isec, imod, edep[isec][imod]);
	  fNDigis++;	  
	}
      }   // section
  }//module

  // Event summary
  cout << "-I- CbmPsdIdealDigitizer: " <<fNDigis<< " CbmPsdDigi created." << endl;

}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmPsdIdealDigitizer::Reset() {
 fNDigis = 0;
 if ( fDigiArray ) fDigiArray->Delete();

}


ClassImp(CbmPsdIdealDigitizer)
