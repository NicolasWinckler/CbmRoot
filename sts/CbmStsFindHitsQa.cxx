//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsFindHitsQa source file               -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------

#include "CbmStsFindHitsQa.h"

#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsCluster.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsPoint.h"
#include "CbmStsHit.h"
#include "CbmStsSensor.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

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

#include <iomanip>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::fixed;
using std::right;
using std::left;
using std::setw;
using std::setprecision;
using std::set;
using std::map;

// -----   Default constructor   ------------------------------------------
CbmStsFindHitsQa::CbmStsFindHitsQa() : CbmTask("STS Hit Finder", 1) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fMCTracks  = NULL;          // MCtrack
  fStsPoints = NULL;          // StsPoints
  fStsHits   = NULL;          // StsHits
  fMatches   = NULL;          // StsTrackMatch
  fStsDigis  = NULL;          // StsDigi
  fStsClusters  = NULL;          // StsCluster

  fNStations = 0;
  fNEvents   = 0;
  fTime1     = 0.;
  

  fDigiScheme = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsFindHitsQa::CbmStsFindHitsQa(Int_t iVerbose) 
  : CbmTask("STSFindHitsQa", iVerbose) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fMCTracks  = NULL;          // MCtrack
  fStsPoints = NULL;          // StsPoints
  fStsHits   = NULL;          // StsHits
  fMatches   = NULL;          // StsTrackMatch
  fStsClusters  = NULL;          // StsCluster

  fNStations = 0;
  fNEvents   = 0;
  fTime1     = 0.;
  

  fDigiScheme = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmStsFindHitsQa::CbmStsFindHitsQa(const char* name, Int_t iVerbose) 
  : CbmTask(name, iVerbose) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fMCTracks  = NULL;          // MCtrack
  fStsPoints = NULL;          // StsPoints
  fStsHits   = NULL;          // StsHits
  fMatches   = NULL;          // StsTrackMatch
  fStsClusters  = NULL;          // StsCluster

  fNStations = 0;
  fNEvents   = 0;
  fTime1     = 0.;
  

  fDigiScheme = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsFindHitsQa::~CbmStsFindHitsQa() {
  fHistoList->Delete();
  delete fHistoList;
  for ( Int_t istat = 0 ; istat < fNStations ; istat++ ) {
    fHistoListPS[istat]->Delete();
  }
  delete[] fHistoList;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsFindHitsQa::Exec(Option_t* opt) {

  fTimer.Start();
  Bool_t warn = kFALSE;

  // Check for digi scheme
  if ( ! fDigiScheme ) {
    cerr << "-E- " << fName << "::Exec: No digi scheme!" << endl;
    return;
  }

  CbmStsStation* station = NULL;
  CbmStsSector*  sector  = NULL;
  for ( Int_t istat = fNStations ; istat > 0 ; ) {
    station = fDigiScheme->GetStation(--istat);
    for ( Int_t isect = station->GetNSectors() ; isect > 0 ; ) {
      sector = station->GetSector(--isect);
      for ( Int_t isens = sector->GetNSensors() ; isens > 0 ; ) {
	fNofHits      [istat][isect][--isens] = 0;
	//	fNofPoints    [istat][isect][  isens] = 0;
	//	fNofRecoPoints[istat][isect][  isens] = 0;
      }
      for ( Int_t iside = 0 ; iside < 2 ; iside++ ) {
	fNofFiredDigis[istat][isect][iside] = 0;
	for ( Int_t ichip = 0 ; ichip < 8 ; ichip++ ) 
	  fNofDigisPChip[istat][isect][iside][ichip] = 0;
      }
    }
  }
  
  // digi occupancy
  if ( fStsClusters ) {
    Int_t nofStsClusters = fStsClusters->GetEntriesFast();
    for ( Int_t iclus = 0 ; iclus < nofStsClusters ; iclus++ ) {
      CbmStsCluster *stsCluster     = (CbmStsCluster*)fStsClusters->At(iclus);
      //    cout << "cluster " << iclus+1 << " in detector id = " << stsCluster->GetDetectorId() << " :" << endl;
      //     cout << "   -> maximum at " << stsCluster->GetChannel() << ", width = " << stsCluster->GetNDigis() << endl;
      //     cout << " in station " << stsCluster->GetStationNr() << endl;
      //     cout << " in sector  " << stsCluster->GetSectorNr () << endl;
      //     cout << " on side    " << stsCluster->GetSide     () << endl;
      fhClusterLength[stsCluster->GetStationNr()-1][stsCluster->GetSectorNr()-1][stsCluster->GetSide()  ]->Fill(stsCluster->GetNDigis());
    }
  }

  // digi occupancy
  Int_t nofStsDigis = fStsDigis->GetEntriesFast();
  for ( Int_t idigi = 0 ; idigi < nofStsDigis ; idigi++ ) {
    CbmStsDigi *stsDigi     = (CbmStsDigi*)fStsDigis->At(idigi);
    //  cout << "digi side = " << stsDigi->GetSide() << endl;
    fNofFiredDigis[stsDigi->GetStationNr()-1][stsDigi->GetSectorNr()-1][stsDigi->GetSide()] += 1;
    fNofDigisPChip[stsDigi->GetStationNr()-1][stsDigi->GetSectorNr()-1][stsDigi->GetSide()][(Int_t)(stsDigi->GetChannelNr()/125)] += 1;
  }

  Int_t nofStsHits = fStsHits->GetEntriesFast();
  Int_t nofStsPoints = fStsPoints->GetEntriesFast();
  Int_t hitStationLimits[2][100];

  for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
    hitStationLimits[0][ist] = -1;
    hitStationLimits[1][ist] = -1;
  }

  // check for limits of hit indices on different stations...
  for ( Int_t ihit = 0 ; ihit < nofStsHits ; ihit++ ) {
    CbmStsHit *stsHit     = (CbmStsHit*)fStsHits->At(ihit);
    fNofHits[stsHit->GetStationNr()-1][stsHit->GetSectorNr()-1][stsHit->GetSensorNr()-1] += 1; // count hits per sensor
    if ( hitStationLimits[0][stsHit->GetStationNr()-1] == -1 )
      hitStationLimits[0][stsHit->GetStationNr()-1] = ihit;
    CbmStsHit *stsHitBack = (CbmStsHit*)fStsHits->At(nofStsHits-ihit-1);
    if ( hitStationLimits[1][stsHitBack->GetStationNr()-1] == -1 ) {
      hitStationLimits[1][stsHitBack->GetStationNr()-1] = nofStsHits-ihit;
    }
  }
  //  cout << "limits got " << endl;

  for ( Int_t istat = 0 ; istat < fDigiScheme->GetNStations() ; istat++ ) {
    station = fDigiScheme->GetStation(istat);
    for ( Int_t isect = 0 ; isect < station->GetNSectors() ; isect++ ) {
      sector = station->GetSector(isect);
      for ( Int_t isens = 0 ; isens < sector->GetNSensors() ; isens++ ) {
	//	cout << "filling   " << istat << " " << isect << " " << isens << endl;
	fhNofHits[istat][isect][isens] -> Fill(fNofHits[istat][isect][isens]);
	//	cout << "---> done " << isens << endl;
      }
      for ( Int_t iside = 0 ; iside < 2 ; iside++ ) {
	fhNofFiredDigis[istat][isect][iside]->Fill(fNofFiredDigis[istat][isect][iside]);
	for ( Int_t ichip = 0 ; ichip < 8 ; ichip++ ) {
	  if ( fhNofDigisPChip[istat][isect][iside][ichip] )
	    fhNofDigisPChip[istat][isect][iside][ichip]->Fill(fNofDigisPChip[istat][isect][iside][ichip]);
	}
      }

    }
  }
  //  cout << "hit maps filled " << endl;

  for ( Int_t ipnt = 0 ; ipnt < nofStsPoints ; ipnt++ ) {
    CbmStsPoint *stsPoint = (CbmStsPoint*)fStsPoints->At(ipnt);

    //    cout << "point " << ipnt << flush;

    Double_t xin = stsPoint->GetXIn();
    Double_t yin = stsPoint->GetYIn();
    Double_t zin = stsPoint->GetZIn();
    gGeoManager->FindNode(xin,yin,zin);
    TGeoNode* curNode = gGeoManager->GetCurrentNode();
    
    //    cout << " in node \"" << curNode->GetName() << "\"" << flush;
    CbmStsSensor* sensor = fDigiScheme->GetSensorByName(curNode->GetName());
    Int_t stationNr = sensor->GetStationNr();

    Int_t startHit = hitStationLimits[0][stationNr-1];
    Int_t finalHit = hitStationLimits[1][stationNr-1];

    //    cout << " at station " << stationNr << " ( " << startHit << " - " << finalHit << " )" << endl;
    
    fhEnergyLoss[stationNr-1]->Fill(stsPoint->GetXIn(),stsPoint->GetYIn(),stsPoint->GetEnergyLoss());

//     Float_t zP = stsPoint->GetZ();
//     Float_t xP = stsPoint->GetX(zP);
//     Float_t yP = stsPoint->GetY(zP);

    Bool_t foundPnt = kFALSE;

    if ( startHit == -1 && finalHit == -1 ) continue;
    
    for ( Int_t ihit = startHit ; ihit < finalHit ; ihit++ ) {
      CbmStsHit *stsHit= (CbmStsHit*)fStsHits->At(ihit);
      if ( ( TMath::Abs(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ())) < .1 ) &&
 	   ( TMath::Abs(stsHit->GetY()-stsPoint->GetY(stsHit->GetZ())) < .1 ) ) {
 	fhHitPointCorrelation[stationNr-1]->Fill(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ()),
						 stsHit->GetY()-stsPoint->GetY(stsHit->GetZ()));
	
	fhHitPointPuls[stationNr-1]       ->Fill((stsHit->GetX()-stsPoint->GetX(stsHit->GetZ()))/sensor->GetSigmaX(),(stsHit->GetY()-stsPoint->GetY(stsHit->GetZ()))/sensor->GetSigmaY());
	
	if ( stsPoint->GetY(stsHit->GetZ()) > 0. )
	  fhHitPointCorrelationU[stationNr-1]->Fill(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ()),
						    stsHit->GetY()-stsPoint->GetY(stsHit->GetZ()));
	if ( stsPoint->GetY(stsHit->GetZ()) < 0. )
	  fhHitPointCorrelationB[stationNr-1]->Fill(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ()),
						    stsHit->GetY()-stsPoint->GetY(stsHit->GetZ()));
      }
      
      if ( ( TMath::Abs(stsHit->GetX()-stsPoint->GetX(stsHit->GetZ())) < .01 ) &&
	   ( TMath::Abs(stsHit->GetY()-stsPoint->GetY(stsHit->GetZ())) < .02 ) )
	foundPnt = kTRUE;
    }
    
    fNofPoints    [stationNr-1][sensor->GetSectorNr()-1][sensor->GetSensorNr()-1] += 1;
    //  cout << stationNr-1 << " " << sensor->GetSectorNr()-1 << " " << sensor->GetSensorNr()-1 << "/" << flush;
    fhPoints      [stationNr-1]->Fill(stsPoint->GetX(stsPoint->GetZ()),
				      stsPoint->GetY(stsPoint->GetZ()));
    if ( foundPnt ) {
      fNofRecoPoints[stationNr-1][sensor->GetSectorNr()-1][sensor->GetSensorNr()-1] += 1;
      fhRecoPoints  [stationNr-1]->Fill(stsPoint->GetX(stsPoint->GetZ()),
					stsPoint->GetY(stsPoint->GetZ()));
    }
  }


  //cout << "correlations got" << endl;  

  // Event summary
  if ( fVerbose ) {
    cout << "----------   StsFindHitsQa : Event " << fNEvents+1 << " summary   ------------"
	 << endl;
    cout << "-----------------------------------------------------------" 
	 << endl;
    cout << endl;
  }
  else
    cout << "\r+ " << setw(15) << left << fName << ": event " << fNEvents+1 << "  " << setprecision(4) 
	 << setw(8) << fixed << right << fTimer.RealTime() << "s." << endl; 

  //cout << "--------        " << fNofRecoPoints[1][4][0] << " / " << fNofPoints[1][4][0] << endl;

  fNEvents++;
  fTime1 += fTimer.RealTime();
}
// -------------------------------------------------------------------------

    


// -----   Private method CreateHistos   -----------------------------------
void CbmStsFindHitsQa::CreateHistos() {


  // Histogram list
  fHistoList = new TList();
  for ( Int_t ist = 0 ; ist < fNStations ; ist++ )
    fHistoListPS[ist] = new TList();

  // Momentum distributions
  Double_t minMom   =  0.;
  Double_t maxMom   = 10.;
  Int_t    nBinsMom = 40;

  CbmStsStation* station = NULL;
  CbmStsSector*  sector  = NULL;
  CbmStsSensor*  sensor  = NULL;

  fhHitFindingEfficiency = new TH1F("hHitFindingEfficiency","Hit finding efficiency",
				    1000000,
				    (Float_t)(2<<24)-0.5,
				    (Float_t)(2<<24)-0.5+1000000.);
//   cout << "histogram from " << fhHitFindingEfficiency->GetXaxis()->GetXmin() 
//        << " --- to " << fhHitFindingEfficiency->GetXaxis()->GetXmax() << endl;

  fHistoList->Add(fhHitFindingEfficiency);

  for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
    fhHitPointCorrelation[ist] = new TH2F(Form("hHitPointCorrelation%i",ist+1),
					  Form("Hit vs point correlation at station %i",ist+1),
					  500,-.1, .1,500,-.1,.1);
    fhHitPointCorrelation[ist]->SetXTitle("#Delta x [cm]");  
    fhHitPointCorrelation[ist]->SetYTitle("#Delta y [cm]");
    
    fhHitPointPuls[ist] = new TH2F(Form("hHitPointPuls%i",ist+1),
					  Form("Hit vs sigma at station %i",ist+1),
					  500,-3., 3.,500,-3.,3.);
    fhHitPointPuls[ist]->SetXTitle("#Pull x [cm]");  
    fhHitPointPuls[ist]->SetYTitle("#Pull y [cm]");
    
    
    fHistoListPS[ist]->Add(fhHitPointCorrelation[ist]);
    fHistoListPS[ist]->Add(fhHitPointPuls[ist]);

    fhHitPointCorrelationU[ist] = new TH2F(Form("h111HitPointCorrelationU%i",ist+1),
					   Form("Hit vs point correlation at station %i",ist+1),
					   500,-.1, .1,500,-.1,.1);
    fhHitPointCorrelationB[ist] = new TH2F(Form("h222HitPointCorrelationB%i",ist+1),
					   Form("Hit vs point correlation at station %i",ist+1),
					   500,-.1, .1,500,-.1,.1);
    fHistoListPS[ist]->Add(fhHitPointCorrelationU[ist]);
    fHistoListPS[ist]->Add(fhHitPointCorrelationB[ist]);

    fhPoints    [ist] = new TH2F(Form("hPoints%i",ist+1),
				 Form("Points on station %i",ist+1),
				 1000,-100.,100.,
				 1000,-100.,100.);
    fhRecoPoints[ist] = new TH2F(Form("hRecoPoints%i",ist+1),
				 Form("Reconstructed points on station %i",ist+1),
				 1000,-100.,100.,
				 1000,-100.,100.);
    fHistoListPS[ist]->Add(fhPoints    [ist]);
    fHistoListPS[ist]->Add(fhRecoPoints[ist]);

  }

  Double_t binningNofHits[501];
  binningNofHits[0] = -0.5;
  for ( Int_t itemp = 0 ; itemp < 100 ; itemp++ ) {
    binningNofHits[itemp+  1] = 0.5+(Double_t)itemp;
    binningNofHits[itemp+101] = 101.5+2.*(Double_t)itemp;
    binningNofHits[itemp+201] = 302.5+3.*(Double_t)itemp;
    binningNofHits[itemp+301] = 602.5+3.*(Double_t)itemp;
    binningNofHits[itemp+401] = 902.5+3.*(Double_t)itemp;
  }

  Double_t fSectorWidth = 0.;
  for ( Int_t istat = 0 ; istat < fNStations ; istat++ ) {
    station = fDigiScheme->GetStation(istat);

    fhEnergyLoss[istat] = new TH2F(Form("hEnergyLossSt%d",istat+1),
				   Form("Energy loss on station %d",istat+1),
				   200,-100.,100.,200,-100.,100.);
    fHistoListPS[istat]->Add(fhEnergyLoss[istat]); 
    for ( Int_t isect = 0 ; isect < station->GetNSectors() ; isect++ ) {
      sector = station->GetSector(isect);
      for ( Int_t isens = 0 ; isens < sector->GetNSensors() ; isens++ ) {
	sensor = sector->GetSensor(isens);
	fSectorWidth = 10.*sensor->GetLx();
	//	cout << "histo craeted for " << istat << " " << isect << " " << isens << endl;
	fhNofHits[istat][isect][isens] = new TH1F(Form("hNofHitsSt%dSect%dSens%d",istat+1,isect+1,isens+1),
						  Form("Number of hits in sector %d, sensor %d of station %d",isect+1,isens+1,istat+1),
						  500,binningNofHits);
	//					 500,-0.5,500.5);
	fHistoListPS[istat]->Add(fhNofHits[istat][isect][isens]); 
      }

      Int_t nofChips = (Int_t)(TMath::Ceil(fSectorWidth/7.5));  // fwidth in mm, 7.5mm = 125(channels)*60mum(pitch)
      Int_t lastChip = (Int_t)(TMath::Ceil(10.*fSectorWidth));
      lastChip = lastChip%75;
      lastChip = (Int_t)(lastChip/.6);
      //      cout << nofChips << " chips on " << istat+1 << " " << isect+1 << endl;

      TString addInfo = "";
      if ( nofChips != 8 ) {
	addInfo = Form(", only %d strips",lastChip);
	//	cout << fSectorWidth << " -> " << addInfo.Data() << endl;
      }

      for ( Int_t iside = 0 ; iside < 2 ; iside++ ) {
	fhClusterLength[istat][isect][iside] = new TH1F(Form("hClusterLength%cSt%dSect%d",(iside==0?'F':'B'),istat+1,isect+1),
							Form("Length of clusters on %s of sector %d of station %d",(iside==0?"front":"back"),isect+1,istat+1),
							501,-0.5,500.5);
	fHistoListPS[istat]->Add(fhClusterLength[istat][isect][iside]); 


	fhNofFiredDigis[istat][isect][iside] = new TH1F(Form("hNofFiredDigis%cSt%dSect%d",(iside==0?'F':'B'),istat+1,isect+1),
							Form("Number of digis on %s of sector %d of station %d",(iside==0?"front":"back"),isect+1,istat+1),
							501,-0.5,500.5);
	fHistoListPS[istat]->Add(fhNofFiredDigis[istat][isect][iside]); 
	
	for ( Int_t ichip = 0 ; ichip < nofChips ; ichip++ ) {
	  fhNofDigisPChip[istat][isect][iside][ichip] = new TH1F(Form("hNofFiredDigis%cSt%dSect%dChip%d",(iside==0?'F':'B'),istat+1,isect+1,ichip+1),
								 Form("Number of digis on %s on chip %d of sector %d of station %d%s",(iside==0?"front":"back"),ichip+1,isect+1,istat+1,(ichip==nofChips-1?addInfo.Data():"")),101,-0.5,100.5);
	  fHistoListPS[istat]->Add(fhNofDigisPChip[istat][isect][iside][ichip]); 
	}
      }
    }
  }

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmStsFindHitsQa::Reset() {


  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Reset();
  for ( Int_t ist = 0 ; ist < fNStations ; ist++ ) {
    TIter next0(fHistoListPS[ist]);
    while ( TH1* histo = ((TH1*)next0()) ) histo->Reset();
  }

}
// -------------------------------------------------------------------------


// -----   Private method SetParContainers   -------------------------------
void CbmStsFindHitsQa::SetParContainers() {


  // Get run and runtime database
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get STS geometry parameter container
  fGeoPar = (CbmGeoStsPar*) db->getContainer("CbmGeoStsPar");

  // Get STS digitisation parameter container
  fDigiPar = (CbmStsDigiPar*) db->getContainer("CbmStsDigiPar");

}
// -------------------------------------------------------------------------




// -----   Private method Init   -------------------------------------------
InitStatus CbmStsFindHitsQa::Init() {


  // Get input array
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No CbmRootManager");

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

  // Get StsDigis array
  fStsDigis = (TClonesArray*) ioman->GetObject("STSDigi");
  if ( ! fStsDigis ) {
    cout << "-E- " << GetName() << "::Init: No StsDigi array!" 
	 << endl;
    return kERROR;
  }

  // Get StsClusters array
  fStsClusters = (TClonesArray*) ioman->GetObject("STSCluster");
  if ( ! fStsClusters ) {
    cout << "-w- " << GetName() << "::Init: No StsCluster array!" 
	 << endl;
    //  return kERROR;
  }

  // Build digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;

  fNStations = fDigiScheme->GetNStations();

  CbmStsStation* station = NULL;
  CbmStsSector*  sector  = NULL;
  for ( Int_t istat = fNStations ; istat > 0 ; ) {
    station = fDigiScheme->GetStation(--istat);
    for ( Int_t isect = station->GetNSectors() ; isect > 0 ; ) {
      sector = station->GetSector(--isect);
      for ( Int_t isens = sector->GetNSensors() ; isens > 0 ; ) {
	fNofPoints    [istat][isect][--isens] = 0;
	fNofRecoPoints[istat][isect][  isens] = 0;
      }
    }
  }

  CreateHistos();
  Reset();

  // Control output
  if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
  else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
  cout << "-I- " << fName << "::Init: "
       << "STS digitisation scheme succesfully initialised" << endl;
  cout << "    Stations: " << fDigiScheme->GetNStations() 
       << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
       << fDigiScheme->GetNChannels() << endl;
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsFindHitsQa::ReInit() {


  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;

  // Control output
  if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
  else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
  cout << "-I- " << fName << "::Init: "
       << "STS digitisation scheme succesfully reinitialised" << endl;
  cout << "    Stations: " << fDigiScheme->GetNStations() 
       << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
       << fDigiScheme->GetNChannels() << endl;
  
  return kSUCCESS;

}
// -------------------------------------------------------------------------




// -----   Virtual method Finish   -----------------------------------------
void CbmStsFindHitsQa::Finish() {

  CbmStsStation* station = NULL;
  CbmStsSector*  sector  = NULL;
  for ( Int_t istat = 0 ; istat < fDigiScheme->GetNStations() ; istat++ ) {
    station = fDigiScheme->GetStation(istat);
    for ( Int_t isect = 0 ; isect < station->GetNSectors() ; isect++ ) {
      sector = station->GetSector(isect);
      for ( Int_t isens = 0 ; isens < sector->GetNSensors() ; isens++ ) {
	if ( fNofPoints[istat][isect][isens] ) {
	  fhHitFindingEfficiency->Fill(2 << 24 | (istat+1) << 16 | (isect+1) << 4 | (isens+1) << 1,
				       ((Float_t)(fNofRecoPoints[istat][isect][isens]))/
				       ((Float_t)(fNofPoints[istat][isect][isens])));
	}
      }
    }
  }

  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "===== " << fName << ": Run summary " << endl;
  cout << "===== Average time  : " << setprecision(4) << setw(8) << right
       << fTime1 / Double_t(fNEvents)  << " s" << endl;
  cout << "============================================================"
       << endl;
	
  gDirectory->mkdir("STSFindHitsQA");
  gDirectory->cd("STSFindHitsQA");
  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();

  for ( Int_t istat = 0 ; istat < fNStations ; istat++ ) {
    gDirectory->mkdir(Form("Station%d",istat+1));
    gDirectory->cd(Form("Station%d",istat+1));
    TIter nextO(fHistoListPS[istat]);
    while ( TH1* histo = ((TH1*)nextO()) ) histo->Write();

    gDirectory->cd("..");
  }
  gDirectory->cd("..");
}					       
// -------------------------------------------------------------------------

ClassImp(CbmStsFindHitsQa)


	
	
	
  
		      





 
