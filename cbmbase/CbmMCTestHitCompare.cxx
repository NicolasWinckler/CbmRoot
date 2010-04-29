// -------------------------------------------------------------------------
// -----                CbmMCTestHitCompare source file             -----
// -----                  Created 18/07/08  by T.Stockmanns        -----
// -------------------------------------------------------------------------
// libc includes
#include <iostream>

// Root includes
#include "TROOT.h"
#include "TClonesArray.h"

// framework includes
#include "FairRootManager.h"
#include "CbmMCTestHitCompare.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairHit.h"
#include "FairLinkedData.h"

#include "CbmStsPoint.h"
#include "CbmStsHit.h"
#include "CbmMCEntry.h"

#include "CbmDetectorList.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TList.h"
#include "TVector3.h"

// -----   Default constructor   -------------------------------------------
CbmMCTestHitCompare::CbmMCTestHitCompare()
	: FairTask("Creates CbmMC test")
{
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmMCTestHitCompare::~CbmMCTestHitCompare()
{
}

// -----   Public method Init   --------------------------------------------
InitStatus CbmMCTestHitCompare::Init()
{


//  fMCMatch->InitStage(kMCTrack, "", "MCTrack");



  FairRootManager* ioman = FairRootManager::Instance();
  	if (!ioman) {
  		std::cout << "-E- CbmMCTestHitCompare::Init: "
  				<< "RootManager not instantiated!" << std::endl;
  		return kFATAL;
  	}

  	fMCMatch = (CbmMCMatch*)ioman->GetObject("MCMatch");

  	fStripHit = (TClonesArray*)ioman->GetObject("StsHit");
  	fMCPoint = (TClonesArray*)ioman->GetObject("StsPoint");

	std::cout << "-I- CbmMCTestHitCompare::Init: Initialization successfull" << std::endl;
  CreateHistos();
  Reset();

  return kSUCCESS;
}


// -------------------------------------------------------------------------
void CbmMCTestHitCompare::SetParContainers()
{
  // Get Base Container
//  FairRun* ana = FairRun::Instance();
//  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

}


// -----   Public method Exec   --------------------------------------------
void CbmMCTestHitCompare::Exec(Option_t* opt)
{
	//fMCMatch->CreateArtificialStage(kMCTrack, "", "");

	CbmMCResult myResult = fMCMatch->GetMCInfo(kStsHit, kStsPoint);
	std::cout << myResult;
	for (int i = 0; i < myResult.GetNEntries(); i++){
		CbmMCEntry myLinks = myResult.GetMCLink(i);
		CbmStsHit* myHit = (CbmStsHit*)fStripHit->At(i);

		std::cout << "Hit Match for hit " << i << " at (" << myHit->GetX() << "," << myHit->GetY() << "," << myHit->GetZ() << ")" << std::endl;
		if (myLinks.GetNLinks()<2){
		  for (int j = 0; j < myLinks.GetNLinks(); j++){
		    if (myLinks.GetLink(j).GetType() == kStsPoint){
				
		      CbmStsPoint* myMCPoint = (CbmStsPoint*)fMCPoint->At(myLinks.GetLink(j).GetIndex());
		      //myMCTrack->Print(myLinks.GetFairLink(j).GetIndex());
		      std::cout << "MCPoint " << myLinks.GetLink(j).GetIndex() << " at (" << myMCPoint->GetX(myMCPoint->GetZ()) << "," << 
		      myMCPoint->GetY(myMCPoint->GetZ()) << "," << 
		      myMCPoint->GetZ() << ")" <<std::endl;
		      fhHitPointCorrelation -> Fill ( myHit->GetX()-myMCPoint->GetX(myMCPoint->GetZ()),
						      myHit->GetY()-myMCPoint->GetY(myMCPoint->GetZ()));
		      std::cout << "--------------------------------" << std::endl;
		    }
		  }
		}
		else {
		  for (int j = 0; j < myLinks.GetNLinks(); j++){
		    if (myLinks.GetLink(j).GetType() == kStsPoint){
				
		      CbmStsPoint* myMCPoint = (CbmStsPoint*)fMCPoint->At(myLinks.GetLink(j).GetIndex());
		      //myMCTrack->Print(myLinks.GetFairLink(j).GetIndex());
		      std::cout << "MCPoint " << myLinks.GetLink(j).GetIndex() << " at (" << myMCPoint->GetX(myMCPoint->GetZ()) << "," << 
		      myMCPoint->GetY(myMCPoint->GetZ()) << "," << 
		      myMCPoint->GetZ() << ")" <<std::endl;
		      if ( TMath::Abs (myHit->GetX()-myMCPoint->GetX(myMCPoint->GetZ())) < 1. && TMath::Abs (myHit->GetY()-myMCPoint->GetY(myMCPoint->GetZ())) < 1.) {
		        fhHitPointSCorrelationSmall -> Fill( myHit->GetX()-myMCPoint->GetX(myMCPoint->GetZ()),
						        myHit->GetY()-myMCPoint->GetY(myMCPoint->GetZ()));
		      }
		      else {
			fhHitPointSCorrelationLarge -> Fill( myHit->GetX()-myMCPoint->GetX(myMCPoint->GetZ()),
						              myHit->GetY()-myMCPoint->GetY(myMCPoint->GetZ()));
		      }
		      std::cout << "--------------------------------" << std::endl;
		    }
		  }
		}
		std::cout << std::endl;
	}
}
// -------------------------------------------------------------------------

    


// -----   Private method CreateHistos   -----------------------------------
void CbmMCTestHitCompare::CreateHistos() {


  // Histogram list
  fHistoList = new TList();
  

  fhHitPointCorrelation  = new TH2F("hHitPointCorrelation","Hit vs point correlation",
					  2500,-1, 1,2500,-1,1);
  fhHitPointSCorrelationSmall = new TH2F("hHitPointSCorrelationSmall","Hit vs points correlation small",
					  2500,-1, 1,2500,-1,1);
  fhHitPointSCorrelationLarge = new TH2F("hHitPointSCorrelationLarge","Hit vs point correlation large",
					  2500,-50, 50,2500,-50,50);
  fHistoList->Add(fhHitPointCorrelation);
  fHistoList->Add(fhHitPointSCorrelationSmall);
  fHistoList->Add(fhHitPointSCorrelationLarge);


}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmMCTestHitCompare::Reset() {


  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Reset();
//   for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
//     TIter next0(fHistoListPS[ist]);
//     while ( TH1* histo = ((TH1*)next0()) ) histo->Reset();
//   }

}
// -------------------------------------------------------------------------
void CbmMCTestHitCompare::Finish()
{
  gDirectory->mkdir("Histos");
  gDirectory->cd("Histos");
  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();


  gDirectory->cd("..");

}


ClassImp(CbmMCTestHitCompare);
