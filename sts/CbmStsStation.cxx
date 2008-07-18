// -------------------------------------------------------------------------
// -----                     CbmStsStation source file                 -----
// -----                  Created 31/03/05  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmStsStation.h"

#include "CbmStsSector.h"
#include "CbmStsSectorDigiPar.h"

#include "TMath.h"

#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::map;


// -----   Default constructor   -------------------------------------------
CbmStsStation::CbmStsStation() {
  fDetectorId = 0;
  fZ = fD = fRmin = fRmax = fRotation = 0.;
  fSectors = new TObjArray(100);
  fSectorMap.clear();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsStation::CbmStsStation(const char* name, Int_t iStation, Double_t z, 
			     Double_t d, Double_t rl, Double_t rmin, 
			     Double_t rmax, Double_t rotation) 
  : TNamed(name, "STS station") {
  fDetectorId = 2 << 24 | iStation << 16;
  fZ          = z;
  fD          = d;
  fRadLength  = rl;
  fRmin       = rmin;
  fRmax       = rmax;
  fRotation   = rotation;
  fSectors = new TObjArray(100);
  fSectorMap.clear();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsStation::~CbmStsStation() {
  if ( fSectors ) {
    fSectors->Delete();
    delete fSectors;
  }
}
// -------------------------------------------------------------------------



// -----   Public method GetNChannels   ------------------------------------
Int_t CbmStsStation::GetNChannels() {
  Int_t nChan = 0;
  for (Int_t iSect=0; iSect<GetNSectors(); iSect++) 
    nChan += GetSector(iSect)->GetNChannels();
  return nChan;
}
// -------------------------------------------------------------------------




// -----   Public method GetSectorByNr   -----------------------------------
CbmStsSector* CbmStsStation::GetSectorByNr(Int_t sectorNr) {
  if ( fSectorMap.find(sectorNr) != fSectorMap.end() ) {
    Int_t index = fSectorMap[sectorNr];
    return (CbmStsSector*) fSectors->At(index);
  }
  else {
    cout << "-W- CbmStsStation::GetSectorByNr: sector " << sectorNr
	 << " not found (station " << GetStationNr() << ")." << endl;
    return NULL;
  }
}
// -------------------------------------------------------------------------



// -----   Public method AddSector   ---------------------------------------
void CbmStsStation::AddSector(CbmStsSectorDigiPar* sectorPar) {

  // Get digitisation parameters
  Int_t iSector   = sectorPar->GetSectorNr();
  Int_t iType     = sectorPar->GetType();
  Double_t x0     = sectorPar->GetX0();
  Double_t y0     = sectorPar->GetY0();
  Double_t z0     = sectorPar->GetZ0();
  Double_t rotRel = sectorPar->GetRotation();
  Double_t lx     = sectorPar->GetLx();
  Double_t ly     = sectorPar->GetLy();
  Double_t d      = sectorPar->GetD();
  Double_t dx     = sectorPar->GetDx();
  Double_t dy     = sectorPar->GetDy();
  Double_t stereo = sectorPar->GetStereo();

  // Check for previous existence of sector
  if ( fSectorMap.find(iSector) != fSectorMap.end() ) {
    cout << "-W- " << fName << "::AddSector: Sector number " << iSector
	 << " already existing for station " << GetStationNr() << endl;
    return;
  }

  // Calculate detectorId and rotation in global c.s.
  Int_t    detId  = 2 << 24 | (GetStationNr() << 16) | (iSector << 1); 
  Double_t rotGlb = fRotation + rotRel;

  // Number of existing sectors
  Int_t nSectors = fSectors->GetEntries();

  // Create and add new sector
//   fSectors->Add(new CbmStsSector(detId, iType, x0, y0, rotGlb, lx, ly,
// 				 dx, dy, stereo));
  fSectors->Add(new CbmStsSector(Form("%ssector%i",GetName(),iSector),detId, iType, x0, y0, z0, rotGlb, lx, ly,
				 d, dx, dy, stereo));

  fSectorMap[iSector] = nSectors;

}
// -------------------------------------------------------------------------

// -----   Public method AddSector   ---------------------------------------
void CbmStsStation::AddSector(CbmStsSector* sector) {

  Int_t iSector  = sector->GetSectorNr();
  Int_t nSectors = fSectors->GetEntries();

  fSectors->Add(sector);

  fSectorMap[iSector] = nSectors;

}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmStsStation::Reset() {
  for (Int_t iSector=0; iSector<GetNSectors(); iSector++) 
    GetSector(iSector)->Reset();
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmStsStation::Print(Bool_t kLong) {
  cout << "Station Nr. ";
  cout.width(2);
  cout << GetStationNr() << ", z =  ";
  cout.width(3);
  cout << fZ << " cm, Rmin = ";
  cout.width(5);
  cout << fRmin << " cm, Rmax = " << fRmax << " cm, angle = ";
  cout.width(3);
  cout << fRotation * 180 / TMath::Pi() << " deg., sectors: ";
  cout.width(4);
  cout << GetNSectors() << ", channels: ";
  cout.width(8);
  cout << GetNChannels() << endl;
  if ( kLong ) 
    for (Int_t iSec=0; iSec<GetNSectors(); iSec++) 
      GetSector(iSec)->Print();
}
// -------------------------------------------------------------------------



ClassImp(CbmStsStation)
