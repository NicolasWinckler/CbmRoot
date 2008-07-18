//* $Id: */

// -------------------------------------------------------------------------
// -----                   CbmStsDigiScheme source file                -----
// -----                  Created 31/08/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmStsDigiScheme.h"

#include "CbmGeoStsPar.h"
#include "CbmStsDigiPar.h"
#include "CbmStsSector.h"
#include "CbmStsSectorDigiPar.h"
#include "CbmStsStation.h"
#include "CbmStsStationDigiPar.h"

#include "CbmGeoMedium.h"
#include "CbmGeoNode.h"
#include "CbmGeoTransform.h"
#include "CbmGeoVector.h"

#include "TArrayD.h"

#include <iostream>
#include <map>

using std::flush;
using std::map;
using std::cout;
using std::endl;

// -----   Constructor   ---------------------------------------------------
CbmStsDigiScheme::CbmStsDigiScheme() {
  fStations = new TObjArray(10);
  fNSectors = fNChannels = 0;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsDigiScheme::~CbmStsDigiScheme() {
  if ( fStations) {
    fStations->Delete();
    delete fStations;
//     fSectorMap.clear();
//     fSectorMap.~map();
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
Bool_t CbmStsDigiScheme::Init(CbmGeoStsPar*  geoPar, 
			      CbmStsDigiPar* digiPar) {

  // Check availability of parameters
  if ( ! geoPar ) {
    cout << "-W- CbmStsDigiScheme::Init: "
	 << "No geometry parameters available!" << endl;
    return kFALSE;
  }
  if ( ! digiPar ) {
    cout << "-W-  CbmStsDigiScheme::Init: "
	 << "No digitisation parameters available!" << endl;
    return kFALSE;
  }
  
  // Loop over stations in DigiPar
  Int_t nStations = digiPar->GetNStations();
  CbmStsStationDigiPar* statPar = NULL;
  CbmStsSectorDigiPar* sectorPar = NULL;
  TObjArray* sensNodes = geoPar->GetGeoSensitiveNodes();
  for (Int_t iStation=0; iStation<nStations; iStation++) {

    // Get digitisation parameters of station
    statPar = (CbmStsStationDigiPar*) digiPar->GetStation(iStation);
    Int_t    statNr   = statPar->GetStationNr();
    Double_t statRot  = statPar->GetRotation();
    Int_t    nSectors = statPar->GetNSectors();

    // Get geometry parameters of station
    TString statName = Form("stsstation%02i",statNr);

    CbmGeoNode* geoStat = (CbmGeoNode*) (sensNodes->FindObject(statName));
    if ( ! geoStat ) {

      Int_t foundSectors = 0;
      TString sectorName = "";
      CbmStsStation* station;
      Int_t lookByIndex = 0;
      for (Int_t iSector=0; iSector<nSectors; iSector++) {
	sectorName = Form("stsstation%02isector%i",statNr,iSector+1);
	CbmGeoNode* geoSector;
	if ( !lookByIndex ) geoSector = (CbmGeoNode*) (sensNodes->FindObject(sectorName));
	else                geoSector = (CbmGeoNode*) (sensNodes->At(lookByIndex++));
	if ( !geoSector ) {
	  Int_t nofNodes = sensNodes->GetEntries();
	  TString tempLookName = Form("stsstation%02isector1#1",statNr);
	  for (Int_t it=0; it<nofNodes; it++) {
	    geoSector = (CbmGeoNode*) (sensNodes->At(it));
	    TString tempNodeName = geoSector->getName();
	    if ( tempNodeName.Contains(tempLookName.Data()) ) {
	      lookByIndex = it+1;
	      break;
	    }
	  }
	}
	if ( !geoSector ) {
	  cout << "-W- CbmStsDigiScheme::Init: Neither " 
	       << statName 
	       << " nor " << sectorName.Data() 
	       << " found among sensitive nodes " << endl;
	  break;
	}
	Int_t    mcId        = geoSector->getMCid();

	if ( lookByIndex ) {
	  TString cloneId = geoSector->getName();
	  cloneId.Remove(0,cloneId.Length()-2);
	  if ( cloneId[0] == '#' ) 
	    cloneId.Remove(0,1);
	  mcId = mcId + ( (atoi(cloneId.Data())-1) * 100000);
	}
	
	foundSectors++;
	CbmGeoTransform* transform = geoSector->getLabTransform();
	CbmGeoVector translat = transform->getTranslation();
	CbmGeoTransform center = geoSector->getCenterPosition();
	CbmGeoVector centerV = center.getTranslation();
	TArrayD* params = geoSector->getParameters();
	CbmGeoMedium* material = geoSector->getMedium();
	material->calcRadiationLength();
	sectorPar = (CbmStsSectorDigiPar*) statPar->GetSector(iSector);

	Double_t sectorZ     = translat.Z() + centerV.Z();
	Double_t sectorD     = 2. * params->At(2);
	Double_t sectorRL    = material->getRadiationLength();

	Int_t    sectorType   = sectorPar->GetType();
	Double_t sectorX      = sectorPar->GetX0();
	Double_t sectorY      = sectorPar->GetY0();
	Double_t sectorRot    = sectorPar->GetRotation();
	Double_t sectorXDim   = sectorPar->GetLx();
	Double_t sectorYDim   = sectorPar->GetLy();
	Double_t sectorDX     = sectorPar->GetDx();
	Double_t sectorDY     = sectorPar->GetDy();
	Double_t sectorStereo = sectorPar->GetStereo();

	Int_t    detId  = 2 << 24 | statNr << 16 | iSector << 1;


 	CbmStsSector* sector = new CbmStsSector(sectorName.Data(), detId, 
						sectorType, sectorX, sectorY, sectorZ,
 						sectorRot, sectorXDim, sectorYDim, 
 						sectorD, sectorDX, sectorDY, sectorStereo);

	if ( sectorX == 0 && sectorY == 0 ) {
	  cout << "SECTOR IS AT X = 0 " << endl;
	  cout << "\n x = " << centerV.X()
	       << "\n y = " << centerV.Y()
	       << "\n z = " << sectorZ 
	       << "\n d = " << sectorD 
	       << "\n RL = " << sectorRL 
	       << "\n mcId = " << mcId << endl;
	  cout << "\n    parameter 0 " << params->At(0)
	       << "\n    parameter 1 " << params->At(1)
	       << "\n    parameter 2 " << params->At(2) << endl;
	  cout << "\n dx = " << sectorDX 
	       << "\n dy = " << sectorDX 
	       << "\n stereo = " << sectorStereo
	       << "\n rotation = " << sectorRot << endl;
	}

	if ( iSector == 0 ) {
	  station = new CbmStsStation(statName, statNr, sectorZ,
				      sectorD, sectorRL, 0., 
				      100., statRot);
	  fStations->Add(station);
	  fStationMap[statNr] = station;
	}	
	station->AddSector(sector);

	fVolumeMap[mcId] = station;
	fSectorMap[mcId] = sector;
      }
      /*      if ( foundSectors == nSectors ) 
	cout << "   " << statName.Data() << " --> All (" << nSectors 
	<< ") of the sectors were found." << endl;*/

      fNSectors  += station->GetNSectors();
      fNChannels += station->GetNChannels();

      continue;
    }
    CbmGeoTransform* transform = geoStat->getLabTransform();
    CbmGeoVector translat = transform->getTranslation();
    CbmGeoTransform center = geoStat->getCenterPosition();
    CbmGeoVector centerV = center.getTranslation();
    TArrayD* params = geoStat->getParameters();
    CbmGeoMedium* material = geoStat->getMedium();
    material->calcRadiationLength();

    Double_t statZ    = translat.Z() + centerV.Z();
    Double_t statRmin = params->At(0);
    Double_t statRmax = params->At(1);
    Double_t statD    = 2. * params->At(2);
    Double_t statRL   = material->getRadiationLength();
    Int_t    mcId     = geoStat->getMCid();

    // Create new station
    CbmStsStation* station = new CbmStsStation(statName, statNr, statZ,
					       statD, statRL, statRmin, 
					       statRmax, statRot);
    fStations->Add(station);
    fStationMap[statNr] = station;
    fVolumeMap[mcId] = station;

    // Loop over sectors of station in DigiPar
    CbmStsSectorDigiPar* sectorPar = NULL;
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      sectorPar = statPar->GetSector(iSector);
      sectorPar->SetD(statD);
      sectorPar->SetZ0(statZ); 
      station->AddSector(sectorPar);
    }

    fNSectors  += station->GetNSectors();
    fNChannels += station->GetNChannels();

  } // Loop over stations


  return kTRUE;

}
// -------------------------------------------------------------------------


// -----   Public method Clear   -------------------------------------------
void CbmStsDigiScheme::Clear() {
  fStations->Delete();
  fStationMap.clear();
  fNSectors = fNChannels = 0;
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmStsDigiScheme::Reset() {
  for (Int_t iStation=0; iStation<fStations->GetEntries(); iStation++) 
    GetStation(iStation)->Reset();
}
// -------------------------------------------------------------------------



// -----   Public method GetStation   --------------------------------------
CbmStsStation* CbmStsDigiScheme::GetStation(Int_t iStation) {
  if ( ! fStations ) return NULL;
  return dynamic_cast<CbmStsStation*>(fStations->At(iStation));
}
// -------------------------------------------------------------------------



// -----   Public method GetStationByNr   ----------------------------------
CbmStsStation* CbmStsDigiScheme::GetStationByNr(Int_t stationNr) {
  if ( ! fStations ) return NULL;
  if ( fStationMap.find(stationNr) == fStationMap.end() ) return NULL;
  return fStationMap[stationNr];
}
// -------------------------------------------------------------------------



// -----   Public method GetStationByMcId   --------------------------------
CbmStsStation* CbmStsDigiScheme::GetStationByMcId(Int_t iVol) {
  if ( ! fStations ) return NULL;
  if ( fVolumeMap.find(iVol) == fVolumeMap.end() ) return NULL;
  return fVolumeMap[iVol];
}
// -------------------------------------------------------------------------



// -----   Public method GetSector   ---------------------------------------
CbmStsSector* CbmStsDigiScheme::GetSector(Int_t stationNr, Int_t sectorNr) {
  return ( GetStationByNr(stationNr)->GetSectorByNr(sectorNr) );
}
// -------------------------------------------------------------------------

// -----   Public method GetSectorByMcId   --------------------------------
CbmStsSector* CbmStsDigiScheme::GetSectorByMcId(Int_t iVol) {
  if ( ! fStations ) return NULL;
  if ( fSectorMap.find(iVol) == fSectorMap.end() ) return NULL;
  return fSectorMap[iVol];
}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void CbmStsDigiScheme::Print(Bool_t kLong) {
  cout << endl;
  cout << "===================  STS digitisation scheme   ================="
       << endl;
  for (Int_t iStat=0; iStat<GetNStations(); iStat++) {
    CbmStsStation* station = GetStation(iStat);
    station->Print(kLong);
  }
  cout << "================================================================"
       << endl << endl;
}
// -------------------------------------------------------------------------

  


ClassImp(CbmStsDigiScheme)
