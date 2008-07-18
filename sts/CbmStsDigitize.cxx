//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsDigitize source file                 -----
// -----                  Created 30/08/06  by V. Friese               -----
// -------------------------------------------------------------------------

// Includes from STS
#include "CbmStsDigitize.h"

#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsPoint.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

// Includes from base
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"

#include <iostream>
#include <iomanip>
#include <map>

using std::cout;
using std::cerr;
using std::endl;
using std::pair;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::setprecision;
using std::map;



// -----   Default constructor   ------------------------------------------
CbmStsDigitize::CbmStsDigitize() : CbmTask("STS Digitizer", 1) {
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsDigitize::CbmStsDigitize(Int_t iVerbose) 
  : CbmTask("STSDigitize", iVerbose) { 
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmStsDigitize::CbmStsDigitize(const char* name, Int_t iVerbose) 
  : CbmTask(name, iVerbose) { 
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsDigitize::~CbmStsDigitize() { 
  if ( fGeoPar)    delete fGeoPar;
  if ( fDigiPar)   delete fDigiPar;
  if ( fDigis ) {
    fDigis->Delete();
    delete fDigis;
  }
  if ( fDigiMatches ) {
    fDigiMatches->Delete();
    delete fDigiMatches;
  }
  if ( fDigiScheme ) delete fDigiScheme;
  Reset();
}
// -------------------------------------------------------------------------

/*
// -----   Public method Exec   --------------------------------------------
void CbmStsDigitize::ExecOld(Option_t* opt) {

  // Reset all eventwise counters
  fTimer.Start();
  Reset();

  // Verbose screen output
  if ( fVerbose > 2 ) {
    cout << endl << "-I- " << fName << ": executing event" << endl;
    cout << "----------------------------------------------" << endl;
  }

  // Loop over all StsPoints
  if ( ! fPoints ) {
    cerr << "-W- " << fName << "::Exec: No input array (STSPoint) "
	 << endl;
    cout << "- " << fName << endl;
    return;
  }

  for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
    CbmStsPoint* point = (CbmStsPoint*) fPoints->At(iPoint);
    fNPoints++;

    // Get the station the point is in
    Int_t mcId = point->GetDetectorID();
    CbmStsStation* station = fDigiScheme->GetStationByMcId(mcId);
    if ( ! station ) {
      if ( fVerbose > 2 ) cout << "StsPoint " << iPoint 
			       << ", no digitisation (MC volume "
			       << mcId << ")" << endl;
      fNFailed++;
      continue;
    }
    Int_t stationNr = station->GetStationNr();

    // Take point coordinates in the midplane of the station
    if ( ! point->IsUsable() ) continue;
    Double_t xpt = point->GetX(station->GetZ());
    Double_t ypt = point->GetY(station->GetZ());

    // Get the sector the point is in
    Int_t sectorNr = -1;
    Int_t iChan    = -1;
    Int_t channelF = -1;
    Int_t channelB = -1;
    Int_t nSectors = station->GetNSectors();
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmStsSector* sector = station->GetSector(iSector);
      iChan = sector->GetChannel(xpt, ypt, 0);
      if ( iChan < 0 ) continue;  // Point not in this sector
      Int_t iDet  = sector->GetDetectorId();
      if ( sectorNr != -1 ) {
	cout.precision(2);
	cerr << "-W- " << fName << "::Exec: StsPoint " << iPoint
	     << " (coordinates " << xpt << ", " << ypt 
	     << ") found in station " << stationNr 
	     << " in sectors " << sector->GetSectorNr() << " and "
	     << sectorNr << endl;
	cout << "    Check overlaps of these sectors! " << endl;
        continue;
      }
      sectorNr = sector->GetSectorNr();
      channelF = iChan;

      // Treat front side
      pair<Int_t, Int_t> a(iDet, channelF);
      if ( fChannelMap.find(a) == fChannelMap.end() ) {
	// Channel not yet active. Create new Digi and Match.
	new ((*fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr,
					    0, channelF);
	new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(iPoint);
	fChannelMap[a] = fNDigis;
	fNDigis++;
      }
      else {
	// Channel already active. Update DigiMatch.
	Int_t iDigi = fChannelMap[a];
	CbmStsDigiMatch* match 
	  = dynamic_cast<CbmStsDigiMatch*>(fDigiMatches->At(iDigi));
	if ( match ) {
	  match->AddPoint(iPoint);
	  fNMulti++;
	}
      }

      // Treat back side (strip sensors only)
      if ( sector->GetType() < 2 || sector->GetType() > 3 ) continue;
      iChan = sector->GetChannel(xpt, ypt, 1);
      if ( iChan <0 ) {
	cerr << "-W- " << fName << "::Exec: No back side channel "
	     << " for StsPoint " << iPoint << ", station "
	     << stationNr << ", sector " << sectorNr 
	     << ", front side channel " << channelF << endl;
	continue;
      }
      iDet = iDet | (1<<31);  // for back side channel
      channelB = iChan;
      pair<Int_t, Int_t> b(iDet, channelB);
      if ( fChannelMap.find(b) == fChannelMap.end() ) {
	// Channel not yet active. Create new Digi and Match.
	new ((*fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr,
					    1, channelB);
	new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(iPoint);
	fChannelMap[b] = fNDigis;
	fNDigis++;
      }
      else {
	// Channel already active. Update DigiMatch.
	Int_t iDigi = fChannelMap[b];
	CbmStsDigiMatch* match 
	  = dynamic_cast<CbmStsDigiMatch*>(fDigiMatches->At(iDigi));
	if ( match ) {
	  match->AddPoint(iPoint);
	  fNMulti++;
	}
      }

    }  // Sector loop

    // Verbose screen output
    if ( fVerbose > 2 ) {
      cout.precision(6);
      cout << "StsPoint " << iPoint << ", station " << stationNr
	   << ", (" << xpt << ", " << ypt << ") cm, sector " 
	   << sectorNr << ", front " << channelF << ", back "
	   << channelB << endl;
    }


    // Not found in any sector?
    if ( sectorNr == -1 ) fNOutside++;      

  }  // StsPoint loop

  // Screen output
  fTimer.Stop();
  if ( ! fVerbose ) cout << "+ ";
  else              cout << "-I- ";
  cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
       << fixed << right << fTimer.RealTime()
       << " s, points " << fNPoints << ", failed " << fNFailed 
       << ", outside " << fNOutside << ", multihits " << fNMulti 
       << ", digis " << fNDigis << endl;

}
// -------------------------------------------------------------------------
*/
// -----   Public method Exec   --------------------------------------------
void CbmStsDigitize::Exec(Option_t* opt) {

  // Reset all eventwise counters
  fTimer.Start();
  Reset();

  // Verbose screen output
  if ( fVerbose > 2 ) {
    cout << endl << "-I- " << fName << ": executing event" << endl;
    cout << "----------------------------------------------" << endl;
  }

  // Loop over all StsPoints
  if ( ! fPoints ) {
    cerr << "-W- " << fName << "::Exec: No input array (STSPoint) "
	 << endl;
    cout << "- " << fName << endl;
    return;
  }

  for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
    CbmStsPoint* point = (CbmStsPoint*) fPoints->At(iPoint);
    fNPoints++;

    // Get the station the point is in
    Int_t mcId = point->GetDetectorID();
    CbmStsStation* station = fDigiScheme->GetStationByMcId(mcId);
    if ( ! station ) {
      if ( fVerbose > 2 ) cout << "StsPoint " << iPoint 
			       << ", no digitisation (MC volume "
			       << mcId << ")" << endl;
      fNFailed++;
      continue;
    }
    Int_t stationNr = station->GetStationNr();

    // Take point coordinates in the midplane of the station
    if ( ! point->IsUsable() ) continue;
    Double_t xpt = point->GetX(station->GetZ());
    Double_t ypt = point->GetY(station->GetZ());

    // Get the sector the point is in
    Int_t sectorNr = -1;
    Int_t iChan    = -1;
    Int_t channelF = -1;
    Int_t channelB = -1;
    Int_t nSectors = station->GetNSectors();

    CbmStsSector* sector;
    if ( !( sector = fDigiScheme->GetSectorByMcId(mcId)) ) {
      Bool_t sectorFound = kFALSE;
      for (Int_t iSector=0; iSector<nSectors; iSector++) {
	sector = station->GetSector(iSector);
	iChan = sector->GetChannel(xpt, ypt, 0);
	if ( iChan < 0 ) continue;
	sectorFound = kTRUE;
	break;
      }
      if ( !sectorFound ) continue;
    }
    else {
      iChan = sector->GetChannel(xpt, ypt, 0);
      Int_t cloneLookup = 1;
      while ( iChan < 0 ) {
	sector = fDigiScheme->GetSectorByMcId(mcId+cloneLookup*100000);
	if ( !sector ) break;
	iChan = sector->GetChannel(xpt, ypt, 0);
	cloneLookup++;
      }
    }

    if ( !sector ) 
      sector = fDigiScheme->GetSectorByMcId(mcId);

    if ( iChan < 0 ) {
      if ( sector->GetX0() != 0 ) {
	Float_t tempX = sector->GetX0();
	Float_t tempY = sector->GetY0();
	Float_t rot = sector->GetRotation();
	Float_t tempLX = sector->GetLx()/2.*TMath::Cos(rot)+sector->GetLy()/2.*TMath::Sin(rot);
	Float_t tempLY = sector->GetLy()/2.*TMath::Cos(rot)+sector->GetLx()/2.*TMath::Sin(rot);

	cout.precision(8);
	cout << fName << ". Warning: Point " << xpt << ", " << ypt << ", " << station->GetZ() << " not inside sector." << endl;
	/*	cout << "Sector from  " << tempX-tempLX << ", " << tempY-tempLY
	     << " to "          << tempX+tempLX << ", " << tempY+tempLY
	     << " AT Z = " << sector->GetZ0() << " and type = " << sector->GetType() << endl;
	cout << "sorry, point (" << xpt << ", " << ypt << ", " << station->GetZ() << ") not in the sector.... dz = " << TMath::Abs(point->GetZOut()-point->GetZIn()) << ". " << endl;
	cout << "sorry, poiIN (" << point->GetXIn() << ", " << point->GetYIn() << ", " << point->GetZIn() << ") not in the sector " << endl;
	cout << "sorry, poOUT (" << point->GetXOut() << ", " << point->GetYOut() << ", " << point->GetZOut() << ") not in the sector " << endl;*/
      }
      continue;  // Point not in this sector
    }
    Int_t iDet  = sector->GetDetectorId();
    if ( sectorNr != -1 ) {
      cout.precision(2);
      cerr << "-W- " << fName << "::Exec: StsPoint " << iPoint
	   << " (coordinates " << xpt << ", " << ypt 
	   << ") found in station " << stationNr 
	   << " in sectors " << sector->GetSectorNr() << " and "
	   << sectorNr << endl;
      cout << "    Check overlaps of these sectors! " << endl;
      continue;
    }
    sectorNr = sector->GetSectorNr();
    channelF = iChan;
    
    // Treat front side
    pair<Int_t, Int_t> a(iDet, channelF);
    if ( fChannelMap.find(a) == fChannelMap.end() ) {
      // Channel not yet active. Create new Digi and Match.
      new ((*fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr,
					  0, channelF);
      new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(iPoint);
      fChannelMap[a] = fNDigis;
      fNDigis++;
    }
    else {
      // Channel already active. Update DigiMatch.
      Int_t iDigi = fChannelMap[a];
      CbmStsDigiMatch* match 
	= dynamic_cast<CbmStsDigiMatch*>(fDigiMatches->At(iDigi));
      if ( match ) {
	match->AddPoint(iPoint);
	fNMulti++;
      }
    }
    
    // Treat back side (strip sensors only)
    if ( sector->GetType() < 2 || sector->GetType() > 3 ) continue;

    iChan = sector->GetChannel(xpt, ypt, 1);
    if ( iChan <0 ) {
      cerr << "-W- " << fName << "::Exec: No back side channel "
	   << " for StsPoint " << iPoint << ", station "
	   << stationNr << ", sector " << sectorNr 
	   << ", front side channel " << channelF << endl;
      continue;
    }
    iDet = iDet | (1<<31);  // for back side channel
    channelB = iChan;
    pair<Int_t, Int_t> b(iDet, channelB);
    if ( fChannelMap.find(b) == fChannelMap.end() ) {
      // Channel not yet active. Create new Digi and Match.
      new ((*fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr,
					  1, channelB);
      new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(iPoint);
      fChannelMap[b] = fNDigis;
      fNDigis++;
    }
    else {
      // Channel already active. Update DigiMatch.
      Int_t iDigi = fChannelMap[b];
      CbmStsDigiMatch* match 
	= dynamic_cast<CbmStsDigiMatch*>(fDigiMatches->At(iDigi));
      if ( match ) {
	  match->AddPoint(iPoint);
	  fNMulti++;
      }
    }

    // Verbose screen output
    if ( fVerbose > 2 ) {
      cout.precision(6);
      cout << "StsPoint " << iPoint << ", station " << stationNr
	   << ", (" << xpt << ", " << ypt << ") cm, sector " 
	   << sectorNr << ", front " << channelF << ", back "
	   << channelB << endl;
    }


    // Not found in any sector?
    if ( sectorNr == -1 ) fNOutside++;      

  }  // StsPoint loop

  // Screen output
  fTimer.Stop();
  if ( fVerbose ) {
    cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
	 << fixed << right << fTimer.RealTime()
	 << " s, points " << fNPoints << ", failed " << fNFailed 
	 << ", outside " << fNOutside << ", multihits " << fNMulti 
	 << ", digis " << fNDigis << endl;
  }
}
// -------------------------------------------------------------------------


 
// -----   Private method SetParContainers   -------------------------------
void CbmStsDigitize::SetParContainers() {

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
InitStatus CbmStsDigitize::Init() {

  // Get input array 
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No CbmRootManager");
  fPoints = (TClonesArray*) ioman->GetObject("STSPoint");

  // Register output array StsDigi
  fDigis = new TClonesArray("CbmStsDigi",1000);
  ioman->Register("STSDigi", "Digital response in STS", fDigis, kTRUE);

  // Register output array StsDigiMatches
  fDigiMatches = new TClonesArray("CbmStsDigiMatch",1000);
  ioman->Register("STSDigiMatch", "Digi Match in STS", fDigiMatches, kTRUE);

  // Build digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
    else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
    cout << "-I- " << fName << "::Init: "
	 << "STS digitisation scheme succesfully initialised" << endl;
    cout << "    Stations: " << fDigiScheme->GetNStations() 
	 << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
	 << fDigiScheme->GetNChannels() << endl;
    return kSUCCESS;
  }
  
  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsDigitize::ReInit() {

  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) return kSUCCESS;

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmStsDigitize::Reset() {
  fNPoints = fNFailed = fNOutside = fNMulti = fNDigis = 0;
  fChannelMap.clear();
  if ( fDigis ) fDigis->Clear();
  if ( fDigiMatches ) fDigiMatches->Clear();
}
// -------------------------------------------------------------------------





ClassImp(CbmStsDigitize)

