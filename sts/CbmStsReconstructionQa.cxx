// -------------------------------------------------------------------------
// -----                  CbmStsReconstructionQa source file               -----
// -----                  Created 06/02/07  by R. Karabowicz               -----
// -------------------------------------------------------------------------


// Includes from sts
#include "CbmStsReconstructionQa.h"

#include "CbmStsHit.h"
#include "CbmStsDigi.h"
#include "CbmStsPoint.h"
#include "CbmStsTrack.h"
#include "CbmStsTrackMatch.h"
#include "CbmGeoStsPar.h"

// Includes from base
#include "FairGeoNode.h"
#include "CbmGeoPassivePar.h"
#include "FairGeoVector.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from ROOT
#include "TCanvas.h"
#include "TPad.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TList.h"
#include "TVector3.h"

// Includes from C++
#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::setprecision;
using std::flush;

// -----   Default constructor   -------------------------------------------
CbmStsReconstructionQa::CbmStsReconstructionQa(Int_t iVerbose) 
  : FairTask("STSReconstructionQA", iVerbose) { 
  fOnlineAnalysis = kFALSE;
  fMinHits = 4;
  fQuota   = 0.7;

  fShowStation1 = 2;
  fShowStation2 = 5;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsReconstructionQa::CbmStsReconstructionQa(Bool_t visualizeBool, Int_t minHits, Double_t quota,
				       Int_t iVerbose) 
  : FairTask("STSReconstructionQA", iVerbose) {
  fOnlineAnalysis = visualizeBool;
  fMinHits = minHits;
  fQuota   = quota;

  fShowStation1 = 2;
  fShowStation2 = 5;

  fPartPdgTable[0] =    11; // electron
  fPartPdgTable[1] = -  11; // positron
  fPartPdgTable[2] =   211; // pi +
  fPartPdgTable[3] = - 211; // pi -
  fPartPdgTable[4] =   321; // kaon +
  fPartPdgTable[5] = - 321; // kaon -
  fPartPdgTable[6] =  2212; // proton
  fPartPdgTable[7] = -2212; // antiproton
  fPartPdgTable[8] = -7777; // don't use
  fPartPdgTable[9] = -7777; // don't use

}
// -------------------------------------------------------------------------

  
  
// -----   Destructor   ----------------------------------------------------
CbmStsReconstructionQa::~CbmStsReconstructionQa() { 

  fHistoList->Delete();
  delete fHistoList;
  fOccupHList->Delete();
  delete fOccupHList;
}
// -------------------------------------------------------------------------



// -----   Public method SetParContainers   --------------------------------
void CbmStsReconstructionQa::SetParContainers() {

  // Get Run
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) {
    cout << "-E- " << GetName() << "::SetParContainers: No FairRunAna!" 
	 << endl;
    return;
  }

  // Get Runtime Database
  FairRuntimeDb* runDb = run->GetRuntimeDb();
  if ( ! run ) {
    cout << "-E- " << GetName() << "::SetParContainers: No runtime database!" 
	 << endl;
    return;
  }

  // Get passive geometry parameters
  fPassGeo = (CbmGeoPassivePar*) runDb->getContainer("CbmGeoPassivePar");
  if ( ! fPassGeo ) {
    cout << "-E- " << GetName() << "::SetParContainers: "
	 << "No passive geometry parameters!" << endl;
    return;
  }

  // Get STS geometry parameters
  fStsGeo = (CbmGeoStsPar*) runDb->getContainer("CbmGeoStsPar");
  if ( ! fStsGeo ) {
    cout << "-E- " << GetName() << "::SetParContainers: "
	 << "No STS geometry parameters!" << endl;
    return;
  }

}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmStsReconstructionQa::Init() {

  cout << "==========================================================="
       << endl;;
  cout << GetName() << ": Initialising..." << endl;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- " << GetName() << "::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get MCTrack array
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTracks ) {
    cout << "-E- " << GetName() << "::Init: No MCTrack array!" << endl;
    return kFATAL;
  }

  // Get StsPoint array
  fStsPoints = (TClonesArray*) ioman->GetObject("STSPoint");
  if ( ! fStsPoints ) {
    cout << "-E- " << GetName() << "::Init: No StsPoint array!" << endl;
    return kFATAL;
  }
   
  // Get StsHit array
  fStsHits = (TClonesArray*) ioman->GetObject("STSHit");
  if ( ! fStsHits ) {
    cout << "-E- " << GetName() << "::Init: No StsHit array!" << endl;
    return kFATAL;
  }

  // Get StsTrack array
  fStsTracks = (TClonesArray*) ioman->GetObject("STSTrack");
  if ( ! fStsTracks ) {
    cout << "-E- " << GetName() << "::Init: No StsTrack array!" << endl;
    return kERROR;
  }

  // Get StsTrackMatch array
  fMatches = (TClonesArray*) ioman->GetObject("STSTrackMatch");
  if ( ! fMatches ) {
    cout << "-E- " << GetName() << "::Init: No StsTrackMatch array!" 
	 << endl;
    return kERROR;
  }

  // Get StsDigis array
  fStsDigis = (TClonesArray*) ioman->GetObject("STSDigi");
  if ( ! fMatches ) {
    cout << "-E- " << GetName() << "::Init: No StsDigi array!" 
	 << endl;
    return kERROR;
  }

  // Get PrimaryVertex array
  //  fPVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
//   CbmVertex* hohoho = (CbmVertex*) ioman->GetObject("PrimaryVertex");
//   if ( ! hohoho ) {
//     cout << "-E- " << GetName() << "::Init: No PrimaryVertex array!" 
// 	 << endl;
//     return kERROR;
//   }

  // Get the geometry of target and STS
  InitStatus geoStatus = GetGeometry();
  if ( geoStatus != kSUCCESS ) {
    cout << "-E- " << GetName() << "::Init: Error in reading geometry!"
	 << endl;
    return geoStatus;
  }

  // Create histograms
  CreateHistos();
  Reset();

  if ( fOnlineAnalysis ) {
    fOnlineCanvas = new TCanvas("StsRecoOnline","Sts reconstruction online",10,10,600,900);
    fOnlinePad[0] = new TPad("titlePad",   "Title pad"                     ,0.01,0.91,0.99,0.99);
    fOnlinePad[1] = new TPad("efficiencyPad","Efficiency pad"              ,0.01,0.61,0.39,0.89);
    fOnlinePad[2] = new TPad("resolutionPad","Momentum resolution pad"     ,0.01,0.31,0.39,0.59);
    fOnlinePad[7] = new TPad("ghostsPad"    ,"Ghosts nof hits pad"         ,0.01,0.01,0.39,0.29);
    fOnlinePad[3] = new TPad("hitVspoint3Pad","Hit vs point on st.3 pad"   ,0.41,0.66,0.69,0.89);
    fOnlinePad[4] = new TPad("hitVspoint6Pad","Hit vs point on st.6 pad"   ,0.71,0.66,0.99,0.89);
    fOnlinePad[5] = new TPad("hitVspntXY3Pad","Hit vs point XY on st.3 pad",0.41,0.41,0.69,0.64);
    fOnlinePad[6] = new TPad("hitVspntXY6Pad","Hit vs point XY on st.6 pad",0.71,0.41,0.99,0.64);
    fOnlinePad[8] = new TPad("printoutPad","Print information pad"         ,0.39,0.00,1.00,0.41);
    for ( Int_t ipad = 0 ; ipad < 9 ; ipad++ ) {
      fOnlinePad[ipad]->SetFillColor(0);
      fOnlinePad[ipad]->SetBorderMode(0);
      fOnlinePad[ipad]->Draw();
    }
    
    fOnlinePad[0]->cd();
    TLegend* brp = new TLegend(0.1,0.1,0.9,0.9,"Online STS reconstruction");
    brp->SetTextAlign(22);
    brp->SetTextSize(0.6);
    brp->SetTextColor(1);
    brp->SetBorderSize(0);
    brp->SetFillColor(0);
    brp->Draw();
    fOnlinePad[0]->Update();
  }

  // Output
  cout << "   Minimum number of STS hits   : " << fMinHits << endl;
  cout << "   Matching quota               : " << fQuota << endl;
  cout << "   Target position ( " << fTargetPos.X() << ", " 
       << fTargetPos.Y() << ", " << fTargetPos.Z() << ") " << endl;
  cout << "   Number of STS stations : " << fNStations << endl;
  if (fActive) cout << "   *****   Task is ACTIVE   *****" << endl;
  cout << "==========================================================="
       << endl << endl;

  return geoStatus;

}
// -------------------------------------------------------------------------



// -----   Public method ReInit   ------------------------------------------
InitStatus CbmStsReconstructionQa::ReInit() {

  cout << "==========================================================="
       << endl;;
  cout << GetName() << ": Reinitialising..." << endl;

  // Get the geometry of target and STS
  InitStatus geoStatus = GetGeometry();
  if ( geoStatus != kSUCCESS ) {
    cout << "-E- " << GetName() << "::ReInit: Error in reading geometry!"
	 << endl;
    return geoStatus;
  }

  // Output
  cout << "   Target position ( " << fTargetPos.X() << ", " 
       << fTargetPos.Y() << ", " << fTargetPos.Z() << ") " << endl;
  cout << "   Number of STS stations : " << fNStations << endl;
  if (fActive) cout << "   *****   Task is ACTIVE   *****" << endl;
  cout << "==========================================================="
       << endl << endl;

  return geoStatus;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsReconstructionQa::Exec(Option_t* opt) {
  // Timer
  fTimer.Start();

  // Eventwise counters
  Int_t nRec     = 0;
  Int_t nGhosts  = 0;
  Int_t nClones  = 0;
  Int_t nAll     = 0;
  Int_t nAcc     = 0;
  Int_t nRecAll  = 0;
  Int_t nPrim    = 0;
  Int_t nRecPrim = 0;
  Int_t nRef     = 0;
  Int_t nRecRef  = 0;
  Int_t nSec     = 0;
  Int_t nRecSec  = 0;
  TVector3 vertex;
  TVector3 momentum;

  // Fill hit and track maps
  FillHitMap();
  FillMatchMap(nRec, nGhosts, nClones);

  for ( Int_t istat = 0 ; istat < fNStations ; istat++ ) {
    for ( Int_t isect = 0 ; isect < fNSectors[istat] ; isect++ ) {
      fNofHits[istat][isect] = 0;
      for ( Int_t iside = 0 ; iside < 2 ; iside++ ) {
	fNofFiredDigis[istat][isect][iside] = 0;
	for ( Int_t ichip = 0 ; ichip < 8 ; ichip++ ) 
	  fNofDigisPChip[istat][isect][iside][ichip] = 0;
      }
    }
  }

  // Loop over MCTracks
  Int_t nMC = fMCTracks->GetEntriesFast();
  for (Int_t iMC=0; iMC<nMC; iMC++) {
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMC);
    if ( ! mcTrack ) {
      cout << "-E- " << GetName() << "::Exec: "
	   << "No MCTrack at index " << iMC
	   << endl;
      Fatal("Exec", "No MCTrack in array");
    }

    mcTrack->GetStartVertex(vertex);
    Bool_t isPrim = kFALSE;
    mcTrack->GetMomentum(momentum);
    Double_t mom = momentum.Mag();

    if ( (vertex-fTargetPos).Mag() < 1. )
      isPrim = kTRUE;

    Int_t pdgC = mcTrack->GetPdgCode();

    Int_t toSave = -1;
    if ( momentum.Z()>0.1 && momentum.Z()<2.6 )
      toSave = (Int_t)((momentum.Z()-.1)*10.);
      //  if ( TMath::Abs(momentum.Z()-1.)<0.5 )

    if ( isPrim ) 
      if ( toSave >= 0 ) {
	if ( pdgC > 0 ) 
	  fhDirEmiPrimP[toSave]->Fill(momentum.X(),momentum.Y());
	else
	  fhDirEmiPrimM[toSave]->Fill(momentum.X(),momentum.Y());
      }

    // Check reconstructability; continue only for reconstructable tracks
    nAll++;
    if ( fHitMap.find(iMC) == fHitMap.end() ) continue; // No hits
    Int_t nHits = fHitMap[iMC];

    if ( nHits < fMinHits ) continue;                   // Too few hits
    nAcc++;

    // Check origin of MCTrack
    if ( isPrim ) nPrim++;
    else nSec++;

    //    cout << ( isPrim ? "p" : "s" ) << nHits << flush;

    // Get momentum
    Bool_t isRef = kFALSE;
    if ( mom > 1. && isPrim) {
      isRef = kTRUE;
      nRef++;
    }

    // Fill histograms for reconstructable tracks
    fhMomAccAll->Fill(mom);
    fhNpAccAll->Fill(Double_t(nHits));
    if ( isPrim) {
      fhMomAccPrim->Fill(mom);
      fhNpAccPrim->Fill(Double_t(nHits));
      if ( toSave >= 0 ) {
	if ( pdgC > 0 ) 
	  fhDirAccPrimP[toSave]->Fill(momentum.X(),momentum.Y());
	else
	  fhDirAccPrimM[toSave]->Fill(momentum.X(),momentum.Y());
      }
    }
    else {
      fhMomAccSec->Fill(mom);
      fhNpAccSec->Fill(Double_t(nHits));
      fhZAccSec->Fill(vertex.Z());
    }

    // Get matched StsTrack
    Int_t    iRec  = -1;
    Double_t quali =  0.;
    Bool_t   isRec = kFALSE;
    if (fMatchMap.find(iMC) != fMatchMap.end() ) {
      /*      if ( isPrim ) {
	cout << "mc mom = " << mom << "  (" 
	     << mcTrack->GetMomentum().X() << ", "
	     << mcTrack->GetMomentum().Y() << ", "
	     << mcTrack->GetMomentum().Z() << ") " << endl;
	     }*/
      iRec  = fMatchMap[iMC];
      isRec = kTRUE;
      CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(iRec);
      if ( ! stsTrack ) {
	cout << "-E- " << GetName() << "::Exec: "
	     << "No StsTrack for matched MCTrack " << iMC << endl;
	Fatal("Exec", "No StsTrack for matched MCTrack");
      }
      quali = fQualiMap[iMC];
      if ( quali < fQuota ) {
	cout << "-E- " << GetName() << "::Exec: "
	     << "Matched StsTrack " << iRec << " is below matching "
	     << "criterion ( " << quali << ")" << endl;
	Fatal("Exec", "Match below matching quota");
      }
      CbmStsTrackMatch* match = (CbmStsTrackMatch*) fMatches->At(iRec);
      if ( ! match ) {
	cout << "-E- " << GetName() << "::Exec: "
	     << "No StsTrackMatch for matched MCTrack " << iMC << endl;
	Fatal("Exec", "No StsTrackMatch for matched MCTrack");
      }
      Int_t nTrue  = match->GetNofTrueHits();
      Int_t nWrong = match->GetNofWrongHits();
      Int_t nFake  = match->GetNofFakeHits();
      Int_t nHits  = stsTrack->GetNStsHits();
      if ( nTrue + nWrong + nFake != nHits ) {
	cout << "True " << nTrue << " wrong " << nWrong << " Fake "
	     << nFake << " Hits " << nHits << endl;
	Fatal("Exec", "Wrong number of hits");
      }

      // Verbose output
      if ( fVerbose > 4 )
	cout << "-I- " << GetName() << ": "
	     << "MCTrack " << iMC << ", hits "
	     << nHits << ", StsTrack " << iRec << ", hits " << nHits 
	     << ", true hits " << nTrue << endl;

      // Fill histograms for reconstructed tracks
      if ( stsTrack->GetParamFirst()->GetQp() )
	fhMomResAll->Fill(mom,100.*(mom-1./TMath::Abs(stsTrack->GetParamFirst()->GetQp()))/mom);
      nRecAll++;
      fhMomRecAll->Fill(mom);
      fhNpRecAll->Fill(Double_t(nHits));
      if ( isPrim ) {
	nRecPrim++;
	fhMomRecPrim->Fill(mom);
	fhNpRecPrim->Fill(Double_t(nHits));

	if ( toSave >= 0 ) {
	  if ( pdgC > 0 ) 
	    fhDirRecPrimP[toSave]->Fill(momentum.X(),momentum.Y());
	  else
	    fhDirRecPrimM[toSave]->Fill(momentum.X(),momentum.Y());
	}

	if ( isRef ) nRecRef++;
	if ( stsTrack->GetParamFirst()->GetQp() )
	  fhMomResPrim->Fill(mom,100.*(mom-1./TMath::Abs(stsTrack->GetParamFirst()->GetQp()))/mom);
      }
      else {
	nRecSec++;
	fhMomRecSec->Fill(mom);
	fhNpRecSec->Fill(Double_t(nHits));
	fhZRecSec->Fill(vertex.Z());
	if ( stsTrack->GetParamFirst()->GetQp() )
	  fhMomResSec->Fill(mom,100.*(mom-1./TMath::Abs(stsTrack->GetParamFirst()->GetQp()))/mom);
      }

    }  // Match found in map?

    Int_t partPdgCode = mcTrack->GetPdgCode();
    for ( Int_t itemp = 0 ; itemp < 10 ; itemp++ ) {
      if ( fPartPdgTable[itemp] == -7777 ) break;
      if ( fPartPdgTable[itemp] != partPdgCode ) continue;
      fhMomAccPart[itemp]->Fill(mom);
      if ( isRec ) 
	fhMomRecPart[itemp]->Fill(mom);
    }    

  } // Loop over MCTracks

  Int_t nofStsHits = fStsHits->GetEntriesFast();
  Int_t nofStsPoints = fStsPoints->GetEntriesFast();
  //  cout << "there are " << nofStsPoints << " points and " << nofStsHits << " hits." << endl;
  Int_t   hitStationLimits[2][100];

  for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
    hitStationLimits[0][ist] = -1;
    hitStationLimits[1][ist] = -1;
  }

  // nof digis
  Int_t nofStsDigis = fStsDigis->GetEntriesFast();
  for ( Int_t idigi = 0 ; idigi < nofStsDigis ; idigi++ ) {
    CbmStsDigi *stsDigi     = (CbmStsDigi*)fStsDigis->At(idigi);
    fNofFiredDigis[stsDigi->GetStationNr()-1][stsDigi->GetSectorNr()][stsDigi->GetSide()] += 1;

    fNofDigisPChip[stsDigi->GetStationNr()-1][stsDigi->GetSectorNr()][stsDigi->GetSide()][(Int_t)(stsDigi->GetChannelNr()/125)] += 1;
  }


  // check for limits of hit indices on different stations...
  for ( Int_t ihit = 0 ; ihit < nofStsHits ; ihit++ ) {
    CbmStsHit *stsHit     = (CbmStsHit*)fStsHits->At(ihit);
    fNofHits[stsHit->GetStationNr()-1][stsHit->GetSectorNr()] += 1; // count hits per sector
    if ( hitStationLimits[0][stsHit->GetStationNr()-1] == -1 )
      hitStationLimits[0][stsHit->GetStationNr()-1] = ihit;
    CbmStsHit *stsHitBack = (CbmStsHit*)fStsHits->At(nofStsHits-ihit-1);
    if ( hitStationLimits[1][stsHitBack->GetStationNr()-1] == -1 ) {
      hitStationLimits[1][stsHitBack->GetStationNr()-1] = nofStsHits-ihit;
    }
  }

//   for ( Int_t istat = 0 ; istat < fNStations ; istat++ ) {
//     for ( Int_t isect = 0 ; isect < fNSectors[istat] ; isect++ ) {
//       fhNofHits[istat][isect] -> Fill(fNofHits[istat][isect]);
//       for ( Int_t iside = 0 ; iside < 2 ; iside++ ) {
// 	fhNofFiredDigis[istat][isect][iside]->Fill(fNofFiredDigis[istat][isect][iside]);
// 	for ( Int_t ichip = 0 ; ichip < 8 ; ichip++ ) {
// 	  if ( fhNofDigisPChip[istat][isect][iside][ichip] )
// 	    fhNofDigisPChip[istat][isect][iside][ichip]->Fill(fNofDigisPChip[istat][isect][iside][ichip]);
// 	}
//       }

//     }
//   }
/*
  for ( Int_t ipnt = 0 ; ipnt < nofStsPoints ; ipnt++ ) {
    CbmStsPoint *stsPoint = (CbmStsPoint*)fStsPoints->At(ipnt);

    Int_t startHit = hitStationLimits[0][fStationNrFromMcId[stsPoint->GetDetectorID()]];
    Int_t finalHit = hitStationLimits[1][fStationNrFromMcId[stsPoint->GetDetectorID()]];
    
    //    fhEnergyLoss[fStationNrFromMcId[stsPoint->GetDetectorID()]]->Fill(stsPoint->GetXIn(),stsPoint->GetYIn(),stsPoint->GetEnergyLoss());

//     Float_t zP = stsPoint->GetZ();
//     Float_t xP = stsPoint->GetX(zP);
//     Float_t yP = stsPoint->GetY(zP);

    if ( startHit == -1 && finalHit == -1 ) continue;
    
    for ( Int_t ihit = startHit ; ihit < finalHit ; ihit++ ) {
      CbmStsHit *stsHit= (CbmStsHit*)fStsHits->At(ihit);
      if ( ( TMath::Abs(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ())) < .1 ) &&
 	   ( TMath::Abs(stsHit->GetY()-stsPoint->GetY(stsHit->GetZ())) < .1 ) )
	// 	fhHitPointCorrelation[fStationNrFromMcId[stsPoint->GetDetectorID()]]->Fill(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ()),
 										   stsHit->GetY()-stsPoint->GetY(stsHit->GetZ()));
//       if ( ( TMath::Abs(stsHit->GetX()-stsPoint->GetX(stsPoint->GetZ())) < 1. ) &&
//  	   ( TMath::Abs(stsHit->GetY()-stsPoint->GetY(stsPoint->GetZ())) < 1. ) )
//  	fhHitPointCorrelation[fStationNrFromMcId[stsPoint->GetDetectorID()]]->Fill(stsHit->GetX()-stsPoint->GetX(stsPoint->GetZ()),
//  										   stsHit->GetY()-stsPoint->GetY(stsPoint->GetZ()));
    }
  }*/
  
  // Calculate efficiencies
  Double_t effAll  = 1.;
  if ( nAcc ) effAll  = Double_t(nRecAll)  / Double_t(nAcc);
  Double_t effPrim = 1.;
  if ( nPrim ) effPrim = Double_t(nRecPrim) / Double_t(nPrim);
  Double_t effRef  = 1.;
  if ( nRef ) effRef = Double_t(nRecRef)  / Double_t(nRef);
  Double_t effSec  = 1.;
  if ( nSec ) effSec  = Double_t(nRecSec)  / Double_t(nSec);

  fhRefTracks   ->SetBinContent(fNEvents+1,nRef);
  fhRecRefTracks->SetBinContent(fNEvents+1,nRecRef);

  // Event summary
  if ( fVerbose > 1 ) {
    cout << "----------   StsReconstructionQa : Event " << fNEvents+1 << " summary   ------------"
	 << endl;
    cout << "MCTracks   : " << nAll << ", reconstructable: " << nAcc
	 << ", reconstructed: " << nRecAll << endl;
    cout << "Vertex     : reconstructable: " << nPrim << ", reconstructed: " 
	 << nRecPrim << ", efficiency " << effPrim*100. << "%" << endl;
    cout << "Reference  : reconstructable: " << nRef  << ", reconstructed: " 
	 << nRecRef  << ", efficiency " << effRef*100. << "%" << endl;
    cout << "Non-vertex : reconstructable: " << nSec << ", reconstructed: "
	 << nRecSec << ", efficiency " << effSec*100. << "%" << endl;
    cout << "STSTracks " << nRec << ", ghosts " << nGhosts 
	 << ", clones " << nClones << endl;
    cout << "-----------------------------------------------------------" 
	 << endl;
    cout << endl;
  }
  if ( fVerbose == 1 ) {
    cout << "\r+ " << setw(15) << left << fName << ": event " << fNEvents+1 << "  " << setprecision(4) 
	 << setw(8) << fixed << right << fTimer.RealTime()
	 << " s, efficiency all " << effAll*100. << " %, vertex " 
	 << effPrim*100. << " %, reference " << effRef*100. << " %" << endl; 
  }

  // Increase counters
  fNAccAll  += nAcc;
  fNAccPrim += nPrim;
  fNAccRef  += nRef;
  fNAccSec  += nSec;
  fNRecAll  += nRecAll;
  fNRecPrim += nRecPrim;
  fNRecRef  += nRecRef;
  fNRecSec  += nRecSec;
  fNGhosts  += nGhosts;
  fNClones  += nClones;
  fNStsTracks += nRec;
  fNEvents++;
  fTime += fTimer.RealTime();

  effRef  = 1.;
  if ( fNAccRef )
    effRef  = Double_t(fNRecRef)  / Double_t(fNAccRef);

  if ( fOnlineAnalysis ) {
    fOnlinePad[1]->cd();
    DivideHistos(fhMomRecAll,  fhMomAccAll,  fhMomEffAll );
    DivideHistos(fhMomRecPrim, fhMomAccPrim, fhMomEffPrim);
    DivideHistos(fhMomRecSec,  fhMomAccSec,  fhMomEffSec );
    fhMomEffAll->SetAxisRange(0.,1.1,"Y");
    fhMomEffAll->SetLineWidth(2);
    fhMomEffAll->SetLineColor(1);
    fhMomEffAll->SetTitle("Efficiency");
    fhMomEffAll->Draw();
    fhMomEffPrim->SetLineWidth(2);
    fhMomEffPrim->SetLineColor(2);
    fhMomEffPrim->Draw("same");
    fhMomEffSec->SetLineWidth(2);
    fhMomEffSec->SetLineColor(3);
    fhMomEffSec->Draw("same");
    TLegend* effLeg = new TLegend(0.3,0.15,0.48,0.4);
    effLeg->SetBorderSize(0);
    effLeg->SetFillColor(0);
    effLeg->AddEntry(fhMomEffAll, "all" ,"pl");
    effLeg->AddEntry(fhMomEffPrim,"prim","pl");
    effLeg->AddEntry(fhMomEffSec, "sec" ,"pl");
    effLeg->Draw();
    TLine* oneLine = new TLine(0.0,1.0,10.0,1.0);
    oneLine->SetLineStyle(2);
    oneLine->Draw();
    fOnlinePad[1]->Update();
    fOnlinePad[2]->cd();
    if ( fhMomResPrim->Integral() ) 
      fOnlinePad[2]->SetLogz();
    fhMomResPrim->SetAxisRange(0.,3.,"Y");
    fhMomResPrim->Draw("cont0");
    for ( Int_t ibin = fhMomResPrim->GetXaxis()->GetNbins() ; ibin > 1 ; ibin-- ) {
      TF1* gausFit = new TF1("gausFit","gaus");
      TH1F* tempProjY = (TH1F*)fhMomResPrim->ProjectionY("tempProjY",ibin,ibin);
      tempProjY->Fit("gausFit","QN","",-5.,5.);
      fhLowBand->SetBinContent(ibin,gausFit->GetParameter(1)-gausFit->GetParameter(2));
      fhLowBand->SetBinError(ibin,0.01);
      fhHigBand->SetBinContent(ibin,gausFit->GetParameter(2));
      fhHigBand->SetBinError(ibin,gausFit->GetParError(2));
//       fhHigBand->SetBinContent(ibin,gausFit->GetParameter(1)+gausFit->GetParameter(2));
//       fhHigBand->SetBinError(ibin,0.01);
    }
    fhLowBand->SetMarkerSize(0.2);
    fhLowBand->SetLineWidth(2);
    fhHigBand->SetMarkerSize(0.1);
    fhHigBand->SetLineWidth(2);
    fhLowBand->Draw("Psame");
    fhHigBand->Draw("Psame");
    fOnlinePad[2]->Update();
    fOnlinePad[7]->cd();
    fhNhGhosts->Draw();
    fOnlinePad[7]->Update();

    TH1F* projX[100];
    TH1F* projY[100];
    TF1*   fitX[100];
    TF1*   fitY[100];
    Double_t resolution[2][100];

    /*    for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
      projX[ist] = (TH1F*)fhHitPointCorrelation[ist]->ProjectionX(Form("projX%i",ist+1));
      projY[ist] = (TH1F*)fhHitPointCorrelation[ist]->ProjectionY(Form("projY%i",ist+1));
      fitX[ist] = new TF1(Form("fitX%i",ist+1),"gaus",-0.02,0.02);
      fitY[ist] = new TF1(Form("fitY%i",ist+1),"gaus",-0.02,0.02);
      projX[ist]->SetAxisRange(-0.02,0.02,"X");
      projY[ist]->SetAxisRange(-0.02,0.02,"X");
      projX[ist]->Fit(fitX[ist],"QN","",-0.02,0.02);
      projY[ist]->Fit(fitY[ist],"QN","",-0.02,0.02);
      resolution[0][ist] = 10000.*fitX[ist]->GetParameter(2);
      resolution[1][ist] = 10000.*fitY[ist]->GetParameter(2);
      }*/

    fOnlinePad[3]->cd();
    //    fhHitPointCorrelation[fShowStation1]->Draw("col");
    fOnlinePad[3]->Update();
    fOnlinePad[4]->cd();
    //    fhHitPointCorrelation[fShowStation2]->Draw("col");
    fOnlinePad[4]->Update();

    fOnlinePad[5]->cd();
    /*projX[fShowStation1]->SetLineWidth(2);
    projY[fShowStation1]->SetLineWidth(2);
    projX[fShowStation1]->SetLineColor(2);
    projY[fShowStation1]->SetLineColor(3);
    projY[fShowStation1]->SetXTitle("#Delta x, y [cm]");
    projX[fShowStation1]->Draw();
    projY[fShowStation1]->Draw("same");
    //    fitY[fShowStation1]->Draw("same");
    //    fitX[fShowStation1]->Draw("same");
    TLegend* legend1 = new TLegend(0.55,0.6,1.0,0.94);
    legend1->SetBorderSize(0);
    legend1->SetFillColor(0);
    legend1->AddEntry(projX[fShowStation1],
		      Form("X,#sigma=%3.2f#mum",resolution[0][fShowStation1]),"l");
    legend1->AddEntry(projY[fShowStation1],
		      Form("Y,#sigma=%3.2f#mum",resolution[1][fShowStation1]),"l");
		      legend1->Draw();*/
    fOnlinePad[5]->Update();

    fOnlinePad[6]->cd();
    /*    projX[fShowStation2]->SetLineWidth(2);
    projY[fShowStation2]->SetLineWidth(2);
    projX[fShowStation2]->SetLineColor(2);
    projY[fShowStation2]->SetLineColor(3);
    projX[fShowStation2]->SetXTitle("#Delta x, y [cm]");
    projX[fShowStation2]->Draw();
    projY[fShowStation2]->Draw("same");
    //    fitY[fShowStation2]->Draw("same");
    //    fitX[fShowStation2]->Draw("same");
    TLegend* legend2 = new TLegend(0.55,0.6,1.0,0.94);
    legend2->SetBorderSize(0);
    legend2->SetFillColor(0);
    legend2->AddEntry(projX[fShowStation2],
		      Form("X,#sigma=%3.2f#mum",resolution[0][fShowStation2]),"l");
    legend2->AddEntry(projY[fShowStation2],
		      Form("Y,#sigma=%3.2f#mum",resolution[1][fShowStation2]),"l");
		      legend2->Draw();*/
    fOnlinePad[6]->Update();

    TF1* allEffFit = new TF1 ("allEffFit","pol0",1.,10.);
    fhMomEffAll->Fit(allEffFit,"QN","",1,10);
    Double_t allEff = allEffFit->GetParameter(0);
    effAll = 1.;
    if ( fhMomAccAll->Integral() )  
      effAll = fhMomRecAll->Integral()/fhMomAccAll->Integral();
    TF1* primEffFit = new TF1 ("primEffFit","pol0",1.,10.);
    fhMomEffPrim->Fit(primEffFit,"QN","",1,10);
    Double_t primEff = primEffFit->GetParameter(0);
    effPrim = 1.;
    if ( fhMomAccPrim->Integral() ) 
      effPrim = fhMomRecPrim->Integral()/fhMomAccPrim->Integral();
    TF1* secEffFit = new TF1 ("secEffFit","pol0",1.,10.);
    fhMomEffSec->Fit(secEffFit,"QN","",1,10);
    Double_t secEff = secEffFit->GetParameter(0);
    effSec = 1.;
    if ( fhMomAccSec->Integral() ) 
      effSec = fhMomRecSec->Integral()/fhMomAccSec->Integral();

    TF1*  momentumResFuncPrim = new TF1("momentumResFuncPrim","gaus",-10.,10.);
    TH1F* momentumResHistPrim = (TH1F*)fhMomResPrim->ProjectionY("momentumResHistPrim");
    momentumResHistPrim->Fit(momentumResFuncPrim,"QN","",-10.,10.);
    Double_t momentumResolutionPrim = momentumResFuncPrim->GetParameter(2);
    TF1*  momentumResFuncAll = new TF1("momentumResFuncAll","gaus",-10.,10.);
    TH1F* momentumResHistAll = (TH1F*)fhMomResAll->ProjectionY("momentumResHistAll");
    momentumResHistAll->Fit(momentumResFuncAll,"QN","",-10.,10.);
    Double_t momentumResolutionAll = momentumResFuncAll->GetParameter(2);

    fOnlinePad[8]->cd();
    TPaveText* printoutPave = new TPaveText(0.0,0.0,1.0,1.0);
    printoutPave->SetTextAlign(23);
    printoutPave->SetTextSize(0.05);
    printoutPave->SetTextColor(1);
    printoutPave->SetBorderSize(0);
    printoutPave->SetFillColor(0);
    printoutPave->AddText(Form("%i events",fNEvents));
    printoutPave->AddText(Form("%3.2f prim, %3.2f sec, %3.2f gh, %3.2f cl",
			       Double_t (fNRecPrim)/Double_t (fNEvents),
			       Double_t (fNRecSec) /Double_t (fNEvents),
			       Double_t (fNGhosts) /Double_t (fNEvents),
			       Double_t (fNClones) /Double_t (fNEvents)));
    printoutPave->AddText("Single Hit Resolutions:");
    for ( Int_t ist = 0 ; ist < fNStations ; ist++ )
      if ( resolution[0][ist] > 0.01 )
	printoutPave->AddText(Form("st#%i,#sigma_{x}=%3.2f#mum,#sigma_{y}=%3.2f#mum",
				   ist+1,resolution[0][ist],resolution[1][ist]));
    printoutPave->AddText("Tracking efficiencies (p>1.0 GeV/c):");
    printoutPave->AddText(Form("all = %2.2f%%(%2.2f%%)",100.*effAll,100.*allEff));
    printoutPave->AddText(Form("vertex = %2.2f%%(%2.2f%%)",100.*effPrim,100.*primEff));
    printoutPave->AddText(Form("reference = %2.2f%%",100.*effRef));
    printoutPave->AddText(Form("non-vertex = %2.2f%%(%2.2f%%)",100.*effSec,100.*secEff));
    printoutPave->AddText(Form("Momentum resolution = %3.2f%%(%3.2f%%)",momentumResolutionAll,momentumResolutionPrim));
    fOnlinePad[8]->Clear();
    printoutPave->Draw();
    fOnlinePad[8]->Update();
  }
}
// -------------------------------------------------------------------------



// -----   Private method Finish   -----------------------------------------
void CbmStsReconstructionQa::Finish() {

  // Divide histograms for efficiency calculation
  DivideHistos(fhMomRecAll,  fhMomAccAll,  fhMomEffAll);
  DivideHistos(fhMomRecPrim, fhMomAccPrim, fhMomEffPrim);
  DivideHistos(fhMomRecSec,  fhMomAccSec,  fhMomEffSec);
  DivideHistos(fhNpRecAll,   fhNpAccAll,   fhNpEffAll);
  DivideHistos(fhNpRecPrim,  fhNpAccPrim,  fhNpEffPrim);
  DivideHistos(fhNpRecSec,   fhNpAccSec,   fhNpEffSec);
  DivideHistos(fhZRecSec,    fhZAccSec,    fhZEffSec);

  for ( Int_t itemp = 0 ; itemp < 25 ; itemp++ ) {
    fhDirAcMPrimM[itemp]->Divide(fhDirAccPrimM[itemp],fhDirEmiPrimM[itemp]);
    fhDirEffPrimM[itemp]->Divide(fhDirRecPrimM[itemp],fhDirAccPrimM[itemp]);
    fhDirAcMPrimP[itemp]->Divide(fhDirAccPrimP[itemp],fhDirEmiPrimP[itemp]);
    fhDirEffPrimP[itemp]->Divide(fhDirRecPrimP[itemp],fhDirAccPrimP[itemp]);
  }

  for ( Int_t itemp = 0 ; itemp < 10 ; itemp++ ) {
    if ( fPartPdgTable[itemp] == -7777 ) break;
    DivideHistos(fhMomRecPart[itemp], fhMomAccPart[itemp], fhMomEffPart[itemp]);
  }

  // Normalise histos for clones and ghosts to one event
  if ( fNEvents ) {
    fhNhClones->Scale(1./Double_t(fNEvents));
    fhNhGhosts->Scale(1./Double_t(fNEvents));
  }

  // Calculate integrated efficiencies and rates
  Double_t effAll  = 1.;
  if ( fNAccAll )  effAll  = Double_t(fNRecAll)  / Double_t(fNAccAll);
  Double_t effPrim = 1.;
  if ( fNAccPrim ) effPrim = Double_t(fNRecPrim) / Double_t(fNAccPrim);
  Double_t effRef  = 1.;
  if ( fNAccRef )  effRef  = Double_t(fNRecRef)  / Double_t(fNAccRef);
  Double_t effSec  = 1.;
  if ( fNAccSec )  effSec  = Double_t(fNRecSec)  / Double_t(fNAccSec);
  Double_t rateGhosts = Double_t(fNGhosts) / Double_t(fNEvents);
  Double_t rateClones = Double_t(fNClones) / Double_t(fNEvents);

  // Run summary to screen
  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "=====   " << fName << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Good events   : " << setw(6) << fNEvents << endl;
  cout << "===== Failed events : " << setw(6) << fNEventsFailed << endl;
  cout << "===== Average time  : " << setprecision(4) << setw(8) << right
       << fTime / Double_t(fNEvents)  << " s" << endl;
  cout << "===== " << endl;
  cout << "===== Efficiency all tracks       : " << effAll*100 << " % (" 
       << fNRecAll << "/" << fNAccAll <<")" << endl;
  cout << "===== Efficiency vertex tracks    : " << effPrim*100 << " % (" 
       << fNRecPrim << "/" << fNAccPrim <<")" << endl;
  cout << "===== Efficiency reference tracks : " << effRef*100 << " % (" 
       << fNRecRef << "/" << fNAccRef <<")" << endl;
  cout << "===== Efficiency secondary tracks : " << effSec*100 << " % (" 
       << fNRecSec << "/" << fNAccSec <<")" << endl;
  cout << "===== Ghost rate " << rateGhosts << " per event" << endl;
  cout << "===== Clone rate " << rateClones << " per event" << endl;
  cout << "============================================================"
       << endl;

  // Write histos to output
  gDirectory->mkdir("STSReconstructionQA");
  gDirectory->cd("STSReconstructionQA");
  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();

//   gDirectory->mkdir("STSOccupancy");
//   gDirectory->cd("STSOccupancy");
//   TIter nextO(fOccupHList);
//   while ( TH1* histo = ((TH1*)nextO()) ) histo->Write();

//   gDirectory->cd("..");
  gDirectory->cd("..");
}
// -------------------------------------------------------------------------



// -----   Private method GetGeometry   ------------------------------------
InitStatus CbmStsReconstructionQa::GetGeometry() {

  // Get target geometry
  if ( ! fPassGeo ) {
    cout << "-W- " << GetName() << "::GetGeometry: No passive geometry!"
	 <<endl;
    fTargetPos.SetXYZ(0., 0., 0.);
    return kERROR;
  }
  TObjArray* passNodes = fPassGeo->GetGeoPassiveNodes();
  if ( ! passNodes ) {
    cout << "-W- " << GetName() << "::GetGeometry: No passive node array" 
	 << endl;
    fTargetPos.SetXYZ(0., 0., 0.);
    return kERROR;
  }
  FairGeoNode* target = (FairGeoNode*) passNodes->FindObject("targ");
  if ( ! target ) {
    cout << "-E- " << GetName() << "::GetGeometry: No target node" 
	 << endl;
    fTargetPos.SetXYZ(0., 0., 0.);
    return kERROR;
  }
  FairGeoVector targetPos = target->getLabTransform()->getTranslation();
  FairGeoVector centerPos = target->getCenterPosition().getTranslation();
  Double_t targetX = targetPos.X() + centerPos.X();
  Double_t targetY = targetPos.Y() + centerPos.Y();
  Double_t targetZ = targetPos.Z() + centerPos.Z();
  fTargetPos.SetXYZ(targetX, targetY, targetZ);
  
  // Get STS geometry
  if ( ! fStsGeo ) {
    cout << "-W- " << GetName() << "::GetGeometry: No passive geometry!"
	 <<endl;
    fNStations = 0;
    return kERROR;
  }
  TObjArray* stsNodes = fStsGeo->GetGeoSensitiveNodes();
  if ( ! stsNodes ) {
    cout << "-E- " << GetName() << "::GetGeometry: No STS node array" 
	 << endl;
    fNStations = 0;
    return kERROR;
  }
  Int_t tempNofStations = stsNodes->GetEntries();

  if ( fVerbose > 2 )  
    cout << "There are " << tempNofStations << " nodes" 
	 << (tempNofStations > 10 ? "!!!" : "" ) << endl;

  TString geoNodeName;
  fNStations = 0;
  TString stationNames[100];
  Bool_t stationKnown = kFALSE;

  for ( Int_t istat = 0 ; istat < 20 ; istat++ )
    fNSectors[istat] = 0;
  
  for ( Int_t ist = 0 ; ist < tempNofStations ; ist++ ) {

    FairGeoNode* stsNode = (FairGeoNode*)stsNodes->At(ist);
    if ( ! stsNode ) {
      cout << "-W- CbmStsDigiScheme::Init: station#" << ist
	   << " not found among sensitive nodes " << endl;
      continue;
    }
    geoNodeName = stsNode->GetName();
    //    TArrayD* params = stsNode->getParameters();

    stationKnown = kFALSE;

    FairGeoVector* pt1 = stsNode->getPoint(0);
    FairGeoVector* pt2 = stsNode->getPoint(1);
    Double_t sectorWidth = TMath::Abs(pt1->getX()-pt2->getX());
    //    cout << geoNodeName.Data() << " -> " << sectorWidth << " cm wide" << endl;

    // check if the node belongs to some station, save the MCId
    for ( Int_t ikst = 0 ; ikst < fNStations ; ikst++ )
      if ( geoNodeName.Contains(stationNames[ikst]) ) {
	fStationNrFromMcId[stsNode->getMCid()] = ikst;
	stationKnown = kTRUE;
	fWidthSectors[ikst][fNSectors[ikst]] = sectorWidth;
	fNSectors[ikst]++;
      }

    if ( stationKnown ) continue;

    // if not known, register it and save MCId
    fStationNrFromMcId[stsNode->getMCid()] = fNStations;
    fWidthSectors[fNStations][fNSectors[fNStations]] = sectorWidth;
    fNSectors[fNStations]++;

    // it will work only if the node name is organized as:
    // for station name is "stsstationXX", where XX is the station number (f.e. XX=07 for station number 7)
    // for sector  name is "stsstationXXanythingHereToDistinguishDifferentSectors"
    geoNodeName.Remove(12,geoNodeName.Length()-12);
    stationNames[fNStations] = geoNodeName.Data();
    fNStations++;

    if ( fVerbose > 3 )
      cout << "station #" << fNStations << " has MCID = " 
	   << stsNode->getMCid() << " and name " << stsNode->GetName() << endl;
    
    //    fStationsMCId[fNStations] = stsNode->getMCid(); // not used
  }
  cout << "    Stations: " << fNStations << " contain" << flush;
  for ( Int_t istat = 0 ; istat < fNStations ; istat++ )
    cout << "  " << fNSectors[istat] << flush;
  cout << "  sensors" << endl;

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Private method CreateHistos   -----------------------------------
void CbmStsReconstructionQa::CreateHistos() {

  // Histogram list
  fHistoList = new TList();
  //  fOccupHList = new TList();

  // Momentum distributions
  Double_t minMom   =  0.;
  Double_t maxMom   = 10.;
  Int_t    nBinsMom = 40;
  fhMomAccAll  = new TH1F("hMomAccAll", "all reconstructable tracks",
			 nBinsMom, minMom, maxMom);
  fhMomRecAll  = new TH1F("hMomRecAll", "all reconstructed tracks",
			 nBinsMom, minMom, maxMom);
  fhMomEffAll  = new TH1F("hMomEffAll", "efficiency all tracks",
			 nBinsMom, minMom, maxMom);
  fhMomEffAll->SetXTitle("p [GeV/c]");
  fhMomEffAll->SetYTitle("efficiency");
  fhMomAccPrim = new TH1F("hMomAccPrim", "reconstructable vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomRecPrim = new TH1F("hMomRecPrim", "reconstructed vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomEffPrim = new TH1F("hMomEffPrim", "efficiency vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomAccSec  = new TH1F("hMomAccSec", "reconstructable non-vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomRecSec  = new TH1F("hMomRecSec", "reconstructed non-vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomEffSec  = new TH1F("hMomEffSec", "efficiency non-vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomGhosts  = new TH1F("hMomGhosts", "momenta of ghosts",
			 nBinsMom, minMom, maxMom);
  fhMomClones  = new TH1F("hMomClones", "momenta of clones",
			 nBinsMom, minMom, maxMom);
  fHistoList->Add(fhMomAccAll);
  fHistoList->Add(fhMomRecAll);
  fHistoList->Add(fhMomEffAll);
  fHistoList->Add(fhMomAccPrim);
  fHistoList->Add(fhMomRecPrim);
  fHistoList->Add(fhMomEffPrim);
  fHistoList->Add(fhMomAccSec);
  fHistoList->Add(fhMomRecSec);
  fHistoList->Add(fhMomEffSec);
  fHistoList->Add(fhMomClones);
  fHistoList->Add(fhMomGhosts);

  for ( Int_t itemp = 0 ; itemp < 25 ; itemp++ ) {
    fhDirEmiPrimM[itemp] = new TH2F(Form("hDirEmiPrimM%d",itemp), "emitted vertex tracks",
				   1000,-1.,1.,1000,-1.,1.);
    fhDirAccPrimM[itemp] = new TH2F(Form("hDirAccPrimM%d",itemp), "reconstructable vertex tracks",
				   1000,-1.,1.,1000,-1.,1.);
    fhDirAcMPrimM[itemp] = new TH2F(Form("hDirAcMPrimM%d",itemp), "acceptance vertex tracks",
				   1000,-1.,1.,1000,-1.,1.);
    fhDirRecPrimM[itemp] = new TH2F(Form("hDirRecPrimM%d",itemp), "reconstructed vertex tracks",
				   1000,-1.,1.,1000,-1.,1.);
    fhDirEffPrimM[itemp] = new TH2F(Form("hDirEffPrimM%d",itemp), "efficiency vertex tracks",
				   1000,-1.,1.,1000,-1.,1.);
    fHistoList->Add(fhDirEmiPrimM[itemp]);
    fHistoList->Add(fhDirAccPrimM[itemp]);
    fHistoList->Add(fhDirAcMPrimM[itemp]);
    fHistoList->Add(fhDirRecPrimM[itemp]);
    fHistoList->Add(fhDirEffPrimM[itemp]);
    fhDirEmiPrimP[itemp] = new TH2F(Form("hDirEmiPrimP%d",itemp), "emitted vertex tracks",
				   1000,-1.,1.,1000,-1.,1.);
    fhDirAccPrimP[itemp] = new TH2F(Form("hDirAccPrimP%d",itemp), "reconstructable vertex tracks",
				   1000,-1.,1.,1000,-1.,1.);
    fhDirAcMPrimP[itemp] = new TH2F(Form("hDirAcMPrimP%d",itemp), "acceptance vertex tracks",
				   1000,-1.,1.,1000,-1.,1.);
    fhDirRecPrimP[itemp] = new TH2F(Form("hDirRecPrimP%d",itemp), "reconstructed vertex tracks",
				   1000,-1.,1.,1000,-1.,1.);
    fhDirEffPrimP[itemp] = new TH2F(Form("hDirEffPrimP%d",itemp), "efficiency vertex tracks",
				   1000,-1.,1.,1000,-1.,1.);
    fHistoList->Add(fhDirEmiPrimP[itemp]);
    fHistoList->Add(fhDirAccPrimP[itemp]);
    fHistoList->Add(fhDirAcMPrimP[itemp]);
    fHistoList->Add(fhDirRecPrimP[itemp]);
    fHistoList->Add(fhDirEffPrimP[itemp]);
  }

  
  for ( Int_t itemp = 0 ; itemp < 10 ; itemp++ ) {
    if ( fPartPdgTable[itemp] == -7777 ) break;
    if ( fVerbose > 3 ) 
      cout << "fpart pdg table content for itemp = " << itemp 
	   << " equals " << fPartPdgTable[itemp] << endl;
    fhMomAccPart[itemp] = new TH1F(Form("hMomAccPart%s%d",(fPartPdgTable[itemp]>0?"P":"M"),TMath::Abs(fPartPdgTable[itemp])),
				   Form("reconstruable particle%d tracks",fPartPdgTable[itemp]),
				   nBinsMom, minMom, maxMom);
    fhMomRecPart[itemp] = new TH1F(Form("hMomRecPart%s%d",(fPartPdgTable[itemp]>0?"P":"M"),TMath::Abs(fPartPdgTable[itemp])),
				   Form("reconstructed particle%d tracks",fPartPdgTable[itemp]),
				   nBinsMom, minMom, maxMom);
    fhMomEffPart[itemp] = new TH1F(Form("hMomEffPart%s%d",(fPartPdgTable[itemp]>0?"P":"M"),TMath::Abs(fPartPdgTable[itemp])),
				   Form("efficiency particle%d tracks",fPartPdgTable[itemp]),
				   nBinsMom, minMom, maxMom);
    fHistoList->Add(fhMomAccPart[itemp]);
    fHistoList->Add(fhMomRecPart[itemp]);
    fHistoList->Add(fhMomEffPart[itemp]);
  }


  // Number-of-points distributions
  Double_t minNp   = -0.5;
  Double_t maxNp   = 15.5;
  Int_t    nBinsNp = 16;
  fhNpAccAll  = new TH1F("hNpAccAll", "all reconstructable tracks",
			 nBinsNp, minNp, maxNp);
  fhNpRecAll  = new TH1F("hNpRecAll", "all reconstructed tracks",
			 nBinsNp, minNp, maxNp);
  fhNpEffAll  = new TH1F("hNpEffAll", "efficiency all tracks",
			 nBinsNp, minNp, maxNp);
  fhNpAccPrim = new TH1F("hNpAccPrim", "reconstructable vertex tracks",
			 nBinsNp, minNp, maxNp);
  fhNpRecPrim = new TH1F("hNpRecPrim", "reconstructed vertex tracks",
			 nBinsNp, minNp, maxNp);
  fhNpEffPrim = new TH1F("hNpEffPrim", "efficiency vertex tracks",
			 nBinsNp, minNp, maxNp);
  fhNpAccSec  = new TH1F("hNpAccSec", "reconstructable non-vertex tracks",
			 nBinsNp, minNp, maxNp);
  fhNpRecSec  = new TH1F("hNpRecSec", "reconstructed non-vertex tracks",
			 nBinsNp, minNp, maxNp);
  fhNpEffSec  = new TH1F("hNpEffSec", "efficiency non-vertex tracks",
			 nBinsNp, minNp, maxNp);
  fHistoList->Add(fhNpAccAll);
  fHistoList->Add(fhNpRecAll);
  fHistoList->Add(fhNpEffAll);
  fHistoList->Add(fhNpAccPrim);
  fHistoList->Add(fhNpRecPrim);
  fHistoList->Add(fhNpEffPrim);
  fHistoList->Add(fhNpAccSec);
  fHistoList->Add(fhNpRecSec);
  fHistoList->Add(fhNpEffSec);

  // z(vertex) distributions
  Double_t minZ    =  0.;
  Double_t maxZ    = 50.;
  Int_t    nBinsZ  = 50;
  fhZAccSec = new TH1F("hZAccSec", "reconstructable non-vertex tracks",
			 nBinsZ, minZ, maxZ);
  fhZRecSec = new TH1F("hZRecSecl", "reconstructed non-vertex tracks",
			 nBinsZ, minZ, maxZ);
  fhZEffSec = new TH1F("hZEffRec", "efficiency non-vertex tracks",
			 nBinsZ, minZ, maxZ);
  fHistoList->Add(fhZAccSec);
  fHistoList->Add(fhZRecSec);
  fHistoList->Add(fhZEffSec);

  // Number-of-hit distributions
  fhNhClones  = new TH1F("hNhClones", "number of hits for clones",
			 nBinsNp, minNp, maxNp);
  fhNhGhosts  = new TH1F("hNhGhosts", "number of hits for ghosts",
			 nBinsNp, minNp, maxNp);
  fhNhGhosts->SetXTitle("# of hits");
  fhNhGhosts->SetYTitle("yield [a.u.]");
  fHistoList->Add(fhNhClones);
  fHistoList->Add(fhNhGhosts);

  fhMomResAll  = new TH2F("hMomResAll", "momentum resolution vs p for all tracks",
			  nBinsMom,minMom,maxMom,
			  200,-10.,10.);
  fhMomResPrim = new TH2F("hMomResPrim","momentum resolution vs p for vertex tracks",
			  nBinsMom,minMom,maxMom,
			  200,-10.,10.);
  fhMomResPrim->SetXTitle("p [GeV/c]");
  fhMomResPrim->SetYTitle("#delta p/p [%%]");
  fhMomResSec  = new TH2F("hMomResSec", "momentum resolution vs p for non-vertex tracks",
			  nBinsMom,minMom,maxMom,
			  200,-10.,10.);
  fHistoList->Add(fhMomResAll);
  fHistoList->Add(fhMomResPrim);
  fHistoList->Add(fhMomResSec);
  fhLowBand = new TH1F("hLowBand","Lower Band",nBinsMom,minMom,maxMom);
  fhHigBand = new TH1F("hHigBand","Higher band",nBinsMom,minMom,maxMom);
  fHistoList->Add(fhLowBand);
  fHistoList->Add(fhHigBand);

  /*  for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
    fhHitPointCorrelation[ist] = new TH2F(Form("hHitPointCorrelation%i",ist+1),
					  Form("Hit vs point correlation at station %i",ist+1),
					  500,-.1, .1,500,-.1,.1);
    fhHitPointCorrelation[ist]->SetXTitle("#Delta x [cm]");  
    fhHitPointCorrelation[ist]->SetYTitle("#Delta y [cm]");
    fHistoList->Add(fhHitPointCorrelation[ist]);
    }*/

  fhPrimaryVertex = new TH3F("hPrimaryVertex","Primary vertex",2000,-0.1,0.1,2000,-0.1,0.1,2000,-0.1,0.1);
  fHistoList->Add(fhPrimaryVertex);

  fhRefTracks    = new TH1F ("hRefTracks"   ,"Nof reconstructed reference tracks",1000,-0.5,999.5);
  fhRecRefTracks = new TH1F ("hRecRefTracks","Nof reconstruable reference tracks",1000,-0.5,999.5);
  fHistoList->Add(fhRefTracks);
  fHistoList->Add(fhRecRefTracks);

  Char_t lett[5] = {'X','Y','Z','x','y'};

  for ( Int_t itemp = 0 ; itemp < 15 ; itemp++ ) {
    if ( itemp < 6 || itemp == 7) {
      fhStsTrackFCovEl[itemp] = new TH1F(Form("hStsTrackFCovEl%d",itemp),
					 Form("StsTrack ParamFirst cov. el. %d",itemp),
					 20000,-1.e-3,1.e-3);
      fhStsTrackLCovEl[itemp] = new TH1F(Form("hStsTrackLCovEl%d",itemp),
					 Form("StsTrack ParamLast cov. el. %d",itemp),
					 20000,-1.e-3,1.e-3);
    }
    else {
      if ( itemp == 6 ) {
	fhStsTrackFCovEl[itemp] = new TH1F(Form("hStsTrackFCovEl%d",itemp),
					   Form("StsTrack ParamFirst cov. el. %d",itemp),
					   20000,-1.e-3,1.e-3);
	fhStsTrackLCovEl[itemp] = new TH1F(Form("hStsTrackLCovEl%d",itemp),
					   Form("StsTrack ParamLast cov. el. %d",itemp),
					   20000,-1.e-3,1.e-3);
      }
      else {
	if ( itemp == 8 ) {
	  fhStsTrackFCovEl[itemp] = new TH1F(Form("hStsTrackFCovEl%d",itemp),
					     Form("StsTrack ParamFirst cov. el. %d",itemp),
					     20000,-1.e-2,1.e-2);
	  fhStsTrackLCovEl[itemp] = new TH1F(Form("hStsTrackLCovEl%d",itemp),
					     Form("StsTrack ParamLast cov. el. %d",itemp),
					     20000,-1.e-2,1.e-2);
	}
	else {
	  fhStsTrackFCovEl[itemp] = new TH1F(Form("hStsTrackFCovEl%d",itemp),
					     Form("StsTrack ParamFirst cov. el. %d",itemp),
					     20000,-1.e-2,1.e-2);
	  fhStsTrackLCovEl[itemp] = new TH1F(Form("hStsTrackLCovEl%d",itemp),
					     Form("StsTrack ParamLast cov. el. %d",itemp),
					     20000,-1.e-2,1.e-2);
	}
      }
    }

    fHistoList->Add(fhStsTrackFCovEl[itemp]);
    fHistoList->Add(fhStsTrackLCovEl[itemp]);

    if ( itemp >= 3 ) continue;
    Int_t    nof = 100 ;
    Double_t beg = -50.;
    Double_t end =  50.;
    if ( itemp == 2 ) { nof = 120; beg = -10.; end = 110.; }
    fhStsTrackFPos[itemp] = new TH1F(Form("hStsTrackFPos%c",lett[itemp]),
				     Form("StsTrack ParamFirst pos %c",lett[itemp]),
				     nof,beg,end);
    fhStsTrackLPos[itemp] = new TH1F(Form("hStsTrackLPos%c",lett[itemp]),
				     Form("StsTrack ParamLast pos %c",lett[itemp]),
				     nof,beg,end);
    fHistoList->Add(fhStsTrackFPos[itemp]);
    fHistoList->Add(fhStsTrackLPos[itemp]);

    if ( itemp >= 2 ) continue;
    fhStsTrackFDir[itemp] = new TH1F(Form("hStsTrackFDir%c",lett[itemp+3]),
				     Form("StsTrack ParamFirst dir %c",lett[itemp+3]),
				     100,-1.5,1.5);
    fhStsTrackLDir[itemp] = new TH1F(Form("hStsTrackLDir%c",lett[itemp+3]),
				     Form("StsTrack ParamLast dir %c",lett[itemp+3]),
				     100,-1.5,1.5);
    fHistoList->Add(fhStsTrackFDir[itemp]);
    fHistoList->Add(fhStsTrackLDir[itemp]);
  }
  fhStsTrackFMom  = new TH1F("hStsTrackFMom", "Momentum of rec. tracks ParFirst",  1000,-50.,50.);
  fhStsTrackLMom  = new TH1F("hStsTrackLMom", "Momentum of rec. tracks ParLast" ,  1000,-50.,50.);
  fhStsTrackChiSq = new TH1F("hStsTrackChiSq","Chi square of rec. tracks",1000,0.,1000.);
  fHistoList->Add(fhStsTrackFMom);
  fHistoList->Add(fhStsTrackLMom);
  fHistoList->Add(fhStsTrackChiSq);

  Double_t binningNofHits[501];
  binningNofHits[0] = -0.5;
  for ( Int_t itemp = 0 ; itemp < 100 ; itemp++ ) {
    binningNofHits[itemp+  1] = 0.5+(Double_t)itemp;
    binningNofHits[itemp+101] = 101.5+2.*(Double_t)itemp;
    binningNofHits[itemp+201] = 302.5+3.*(Double_t)itemp;
    binningNofHits[itemp+301] = 602.5+3.*(Double_t)itemp;
    binningNofHits[itemp+401] = 902.5+3.*(Double_t)itemp;
  }
//   for ( Int_t itemp = 0 ; itemp < 501 ; itemp++ ) 
//     cout << binningNofHits[itemp] << " " << flush;
//   cout << endl;

/*  for ( Int_t istat = 0 ; istat < fNStations ; istat++ ) {
    fhEnergyLoss[istat] = new TH2F(Form("hEnergyLossSt%d",istat+1),
				   Form("Energy loss on station %d",istat+1),
				   200,-100.,100.,200,-100.,100.);
    fOccupHList->Add(fhEnergyLoss[istat]); 
    for ( Int_t isect = 0 ; isect < fNSectors[istat] ; isect++ ) {
      fhNofHits[istat][isect] = new TH1F(Form("hNofHitsSt%dSect%d",istat+1,isect+1),
					 Form("Number of hits in sector %d of station %d",isect+1,istat+1),
					 500,binningNofHits);
					 //					 500,-0.5,500.5);
      fOccupHList->Add(fhNofHits[istat][isect]); 

      Int_t nofChips = (Int_t)(TMath::Ceil(fWidthSectors[istat][isect]/7.5));  // fwidth in mm, 7.5mm = 125(channels)*60mum(pitch)
      Int_t lastChip = (Int_t)(TMath::Ceil(10.*fWidthSectors[istat][isect]));
      lastChip = lastChip%75;
      lastChip = (Int_t)(lastChip/.6);

      TString addInfo = "";
      if ( nofChips != 8 ) {
	addInfo = Form(", only %d strips",lastChip);
	//	cout << fWidthSectors[istat][isect] << " -> " << addInfo.Data() << endl;
      }

      for ( Int_t iside = 0 ; iside < 2 ; iside++ ) {
	fhNofFiredDigis[istat][isect][iside] = new TH1F(Form("hNofFiredDigis%cSt%dSect%d",(iside==0?'F':'B'),istat+1,isect+1),
							Form("Number of digis on %s of sector %d of station %d",(iside==0?"front":"back"),isect+1,istat+1),
							501,-0.5,500.5);
	fOccupHList->Add(fhNofFiredDigis[istat][isect][iside]); 
	
	for ( Int_t ichip = 0 ; ichip < nofChips ; ichip++ ) {
	  fhNofDigisPChip[istat][isect][iside][ichip] = new TH1F(Form("hNofFiredDigis%cSt%dSect%dChip%d",(iside==0?'F':'B'),istat+1,isect+1,ichip+1),
								 Form("Number of digis on %s on chip %d of sector %d of station %d%s",(iside==0?"front":"back"),ichip+1,isect+1,istat+1,(ichip==nofChips-1?addInfo.Data():"")),
								 101,-0.5,100.5);
	  fOccupHList->Add(fhNofDigisPChip[istat][isect][iside][ichip]); 
	}
      }
    }
    }*/
}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmStsReconstructionQa::Reset() {

  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Reset();
  TIter next0(fOccupHList);
  while ( TH1* histo = ((TH1*)next0()) ) histo->Reset();

  fNAccAll = fNAccPrim = fNAccRef = fNAccSec = 0;
  fNRecAll = fNRecPrim = fNRecRef = fNRecSec = 0;
  fNGhosts = fNClones = fNEvents = 0;
  fNStsTracks = 0;
}
// -------------------------------------------------------------------------



// -----   Private method FillHitMap   -------------------------------------
void CbmStsReconstructionQa::FillHitMap() {
  fHitMap.clear();
  Int_t nHits = fStsHits->GetEntriesFast();
  for (Int_t iHit=0; iHit<nHits; iHit++) {
    CbmStsHit* hit = (CbmStsHit*) fStsHits->At(iHit);
    Int_t iMc = hit->GetRefIndex();
    if ( iMc < 0 ) continue;
    CbmStsPoint* stsPoint = (CbmStsPoint*) fStsPoints->At(iMc);
    Int_t iTrack = stsPoint->GetTrackID();
    fHitMap[iTrack]++;
  }
}
// -------------------------------------------------------------------------

    

// ------   Private method FillMatchMap   ----------------------------------
void CbmStsReconstructionQa::FillMatchMap(Int_t& nRec, Int_t& nGhosts,
				      Int_t& nClones) {

  // Clear matching maps
  fMatchMap.clear();
  fQualiMap.clear();
  
  // Loop over StsTracks. Check matched MCtrack and fill maps.
  nGhosts = 0;
  nClones = 0;
  nRec    = fStsTracks->GetEntriesFast();
  Int_t nMtc = fMatches->GetEntriesFast();
  if ( nMtc != nRec ) {
    cout << "-E- " << GetName() << "::Exec: Number of StsMatches ("
	 << nMtc << ") does not equal number of StsTracks ("
	 << nRec << ")" << endl;
    Fatal("Exec", "Inequal number of StsTrack and StsTrackMatch");
  }
  for (Int_t iRec=0; iRec<nRec; iRec++) {

    CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(iRec);
    if ( ! stsTrack ) {
      cout << "-E- " << GetName() << "::Exec: "
	   << "No StsTrack at index " << iRec << endl;
      Fatal("Exec", "No StsTrack in array");
      }
    Int_t nHits = stsTrack->GetNStsHits();
      
    FairTrackParam* trParF = (FairTrackParam*)stsTrack->GetParamFirst();
    FairTrackParam* trParL = (FairTrackParam*)stsTrack->GetParamLast();
    fhStsTrackFPos[0]->Fill(trParF->GetX());
    fhStsTrackFPos[1]->Fill(trParF->GetY());
    fhStsTrackFPos[2]->Fill(trParF->GetZ());
    fhStsTrackLPos[0]->Fill(trParL->GetX());
    fhStsTrackLPos[1]->Fill(trParL->GetY());
    fhStsTrackLPos[2]->Fill(trParL->GetZ());
    fhStsTrackFDir[0]->Fill(trParF->GetTx());fhStsTrackFDir[1]->Fill(trParF->GetTy());
    fhStsTrackLDir[0]->Fill(trParL->GetTx());fhStsTrackLDir[1]->Fill(trParL->GetTy());
    Int_t elToFill = 0;
    for ( Int_t ixx = 0 ; ixx < 5 ; ixx++ ) {
      for ( Int_t iyy = ixx ; iyy < 5 ; iyy++ ) {
	fhStsTrackFCovEl[elToFill]->Fill(trParF->GetCovariance(ixx,iyy));
	fhStsTrackLCovEl[elToFill]->Fill(trParL->GetCovariance(ixx,iyy));
	elToFill++;
      }
    }
    fhStsTrackFMom->Fill(trParF->GetQp());
    fhStsTrackLMom->Fill(trParL->GetQp());
    fhStsTrackChiSq->Fill(stsTrack->GetChi2());

    CbmStsTrackMatch* match = (CbmStsTrackMatch*) fMatches->At(iRec);
    if ( ! match ) {
      cout << "-E- " << GetName() << "::Exec: "
	   << "No StsTrackMatch at index " << iRec << endl;
      Fatal("Exec", "No StsTrackMatch in array");
    }
    Int_t nTrue = match->GetNofTrueHits();

    Int_t iMC = match->GetMCTrackId();
    if (iMC == -1 ) {       // no common point with MC, really ghastly!
      if ( fVerbose > 4 ) 
	cout << "-I- " << GetName() << ":" 
	     << "No MC match for StsTrack " << iRec << endl;
      fhNhGhosts->Fill(nHits);
      if ( stsTrack->GetParamFirst()->GetQp() ) 
	fhMomGhosts->Fill(1./TMath::Abs(stsTrack->GetParamFirst()->GetQp()));
      nGhosts++;
      continue;
    }

    // Check matching criterion (quota)
    Double_t quali = 1.;
    if ( nHits ) 
      quali = Double_t(nTrue) / Double_t(nHits);
    if ( quali >= fQuota ) {

      // No previous match for this MCTrack
      if ( fMatchMap.find(iMC) == fMatchMap.end() ) {
	fMatchMap[iMC] = iRec;
	fQualiMap[iMC] = quali;
      }

      // Previous match; take the better one
      else {
	if ( fVerbose > 4 ) 
	  cout << "-I- " << GetName() << ": "
	       << "MCTrack " << iMC << " doubly matched."
	       << "Current match " << iRec 
	       << ", previous match " << fMatchMap[iMC] 
	       << endl;
	if ( fQualiMap[iMC] < quali ) {
	  CbmStsTrack* oldTrack 
	    = (CbmStsTrack*) fStsTracks->At(fMatchMap[iMC]);	  
	  fhNhClones->Fill(Double_t(oldTrack->GetNStsHits()));
	  if ( oldTrack->GetParamFirst()->GetQp() )
	    fhMomClones->Fill(1./TMath::Abs(oldTrack->GetParamFirst()->GetQp()));
	  fMatchMap[iMC] = iRec;
	  fQualiMap[iMC] = quali;
	}
	else {
	  fhNhClones->Fill(nHits);
	  if ( stsTrack->GetParamFirst()->GetQp() )
	    fhMomClones->Fill(1./TMath::Abs(stsTrack->GetParamFirst()->GetQp()));
	}
	nClones++;
      }

    }
    
    // If not matched, it's a ghost
    else {
      if ( fVerbose > 4 )
	cout << "-I- " << GetName() << ":" 
	     << "StsTrack " << iRec << " below matching criterion "
	     << "(" << quali << ")" << endl;
      fhNhGhosts->Fill(nHits);
      if ( stsTrack->GetParamFirst()->GetQp() )
	fhMomGhosts->Fill(1./TMath::Abs(stsTrack->GetParamFirst()->GetQp()));
      nGhosts++;
    }

  }   // Loop over StsTracks

}
// -------------------------------------------------------------------------



// -----   Private method DivideHistos   -----------------------------------
void CbmStsReconstructionQa::DivideHistos(TH1* histo1, TH1* histo2,
				      TH1* histo3) {
  
  if ( !histo1 || !histo2 || !histo3 ) {
    cout << "-E- " << GetName() << "::DivideHistos: "
	 << "NULL histogram pointer" << endl;
    Fatal("DivideHistos", "Null histo pointer");
  }

  Int_t nBins = histo1->GetNbinsX();
  if ( histo2->GetNbinsX() != nBins || histo3->GetNbinsX() != nBins ) {
    cout << "-E- " << GetName() << "::DivideHistos: "
	 << "Different bin numbers in histos" << endl;
    cout << histo1->GetName() << " " << histo1->GetNbinsX() << endl;
    cout << histo2->GetName() << " " << histo2->GetNbinsX() << endl;
    cout << histo3->GetName() << " " << histo3->GetNbinsX() << endl;
   return;
  }

  Double_t c1, c2, c3, ce;
  for (Int_t iBin=0; iBin<nBins; iBin++) {
    c1 = histo1->GetBinContent(iBin);
    c2 = histo2->GetBinContent(iBin);
    if ( c2 ) {
      c3 = c1 / c2;
      if ( c3 <= 1. )
	ce = TMath::Sqrt( c3 * ( 1. - c3 ) / c2 );
      else
	ce = TMath::Sqrt( c3 * ( 1. + c3 ) / c2 );
    }
    else {
      c3 = 0.;
      ce = 0.;
    }
    histo3->SetBinContent(iBin, c3);
    histo3->SetBinError(iBin, ce);
  }

}
// -------------------------------------------------------------------------



ClassImp(CbmStsReconstructionQa)
