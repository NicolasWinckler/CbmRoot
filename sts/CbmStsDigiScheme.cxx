//* $Id: */

// -------------------------------------------------------------------------
// -----                   CbmStsDigiScheme source file                -----
// -----                  Created 31/08/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmStsDigiScheme.h"

#include "CbmGeoStsPar.h"
#include "CbmStsDigiPar.h"
#include "CbmStsSensor.h"
#include "CbmStsSensorDigiPar.h"
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
using std::pair;

// -----   Constructor   ---------------------------------------------------
CbmStsDigiScheme::CbmStsDigiScheme() {
  fStations = new TObjArray(10);
  fNSectors = fNSensors = fNChannels = 0;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsDigiScheme::~CbmStsDigiScheme() {
  if ( fStations) {
    fStations->Delete();
    delete fStations;
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
  CbmStsStationDigiPar*  statPar = NULL;
  CbmStsSectorDigiPar* sectorPar = NULL;
  CbmStsSensorDigiPar* sensorPar = NULL;
  TObjArray* passNodes = geoPar->GetGeoPassiveNodes();
  TObjArray* sensNodes = geoPar->GetGeoSensitiveNodes();
  for (Int_t iStation=0; iStation<nStations; iStation++) {

    // Get digitisation parameters of station
    statPar = (CbmStsStationDigiPar*) digiPar->GetStation(iStation);
    Int_t    statNr   = statPar->GetStationNr();
    Double_t statRot  = statPar->GetRotation();
    Int_t    nSectors = statPar->GetNSectors();
    Double_t statZPos = -666.;// = statPar->GetZPosition();

    CbmStsStation* station;
    TString stationName = Form("stat%02d",iStation+1);

    TString statVolName = Form("stsstation%02ikeepvol",statNr);
    CbmGeoNode* statKeepVol = (CbmGeoNode*) (passNodes->FindObject(statVolName));
    if ( statKeepVol ) {
      CbmGeoTransform* transform = statKeepVol->getLabTransform();
      CbmGeoVector translat = transform->getTranslation();
      CbmGeoTransform center = statKeepVol->getCenterPosition();
      CbmGeoVector centerV = center.getTranslation();

      statZPos = translat.Z() + centerV.Z();
    }

    Int_t sensorByIndex = 0;

    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      sectorPar = (CbmStsSectorDigiPar*) statPar->GetSector(iSector);
      Int_t sectorNr = iSector+1;
      Int_t sectorDetId  = 2 << 24 | statNr << 16 | sectorNr << 4;

      sectorPar = (CbmStsSectorDigiPar*) statPar->GetSector(iSector);
      Int_t nSensors = sectorPar->GetNSensors();

      CbmStsSector* sector;
      TString sectorName = Form("stat%02dsect%d",iStation+1,iSector+1);

      for (Int_t iSensor=0; iSensor<nSensors; iSensor++) {
	Int_t sensorNr = iSensor+1;
	Int_t    detId  = 2 << 24 | statNr << 16 | sectorNr << 4 | sensorNr << 1;
	  
	CbmGeoNode* geoSensor;
	if ( sensorByIndex ) geoSensor = (CbmGeoNode*) (sensNodes->At(sensorByIndex++));
	else {
	  Int_t nofNodes = sensNodes->GetEntries();
	  TString tempLookName = Form("stsstation%02isensor1#1",statNr);
	  for (Int_t it=0; it<nofNodes; it++) {
	    geoSensor = (CbmGeoNode*) (sensNodes->At(it));
	    TString tempNodeName = geoSensor->getName();
	    if ( tempNodeName.Contains(tempLookName.Data()) ) {
	      sensorByIndex = it+1;
	      break;
	    }
	  }
	}
	fDetIdByName.insert(pair<TString, Int_t> (geoSensor->GetName(), detId));

	sensorPar = (CbmStsSensorDigiPar*) sectorPar->GetSensor(iSensor);

	Int_t    sensorType   = sensorPar->GetType();
	Double_t sensorX      = sensorPar->GetX0();
	Double_t sensorY      = sensorPar->GetY0();
	Double_t sensorZ      = sensorPar->GetZ0();
	Double_t sensorRot    = sensorPar->GetRotation();
	Double_t sensorXDim   = sensorPar->GetLx();
	Double_t sensorYDim   = sensorPar->GetLy();
	Double_t sensorZDim   = sensorPar->GetD();
	Double_t sensorDX     = sensorPar->GetDx();
	Double_t sensorDY     = sensorPar->GetDy();
	Double_t sensorStereoF = sensorPar->GetStereoF();
	Double_t sensorStereoB = sensorPar->GetStereoB();

	TString sensorName = geoSensor->GetName();
	sensorName.ReplaceAll("#","_");

	CbmStsSensor* sensor = new CbmStsSensor(sensorName.Data(), detId, 
						sensorType, 
						sensorX, sensorY, sensorZ,
						sensorRot, 
						sensorXDim, sensorYDim, sensorZDim, 
						sensorDX, sensorDY, sensorStereoF, sensorStereoB);
	

	// create sector that will keep the sensor
	if ( iSensor == 0 ) {
	  sector  = new CbmStsSector(sectorName.Data(), sectorDetId);

	  // create station that will keep the sector
	  if ( iSector == 0 ) {
	    CbmGeoMedium* material = geoSensor->getMedium();
	    Double_t sensorRL    = material->getRadiationLength();

	    station = new CbmStsStation(stationName.Data(), statNr, statZPos,
					sensorZDim, sensorRL, 0., 
					100., statRot);
	    fStations->Add(station);
	    fStationMap[statNr] = station;
	  }	
	  station->AddSector(sector);
	}
	sector->AddSensor(sensor);

	// put sensor into name/sensor map
	map < TString, CbmStsSensor*>::iterator p;
   	p=fSensorByName.find(sensorName);
	if(p!=fSensorByName.end()){
	  cout << " -E- Sensor \"" << sensorName.Data() << "\" is already inserted " << endl;
	}else{
	  fSensorByName.insert(pair<TString, CbmStsSensor*> (sensorName, sensor));
	}

      } // Loop over sensors

      fNSensors  += sector->GetNSensors();
    } // Loop over sectors
    
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
  fDetIdByName.clear();
  fNSectors = fNSensors = fNChannels = 0;
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


// -----   Public method GetSector   ---------------------------------------
CbmStsSector* CbmStsDigiScheme::GetSector(Int_t stationNr, Int_t sectorNr) {
  return ( GetStationByNr(stationNr)->GetSectorByNr(sectorNr) );
}
// -------------------------------------------------------------------------

// -----   Public method GetSensor   ---------------------------------------
CbmStsSensor* CbmStsDigiScheme::GetSensor(Int_t stationNr, Int_t sectorNr, Int_t sensorNr) {
  return ( GetStationByNr(stationNr)->GetSectorByNr(sectorNr)->GetSensorByNr(sensorNr) );
}
// -------------------------------------------------------------------------

// -----   Public method GetDetectorIdByName  ------------------------------
Int_t CbmStsDigiScheme::GetDetectorIdByName(TString sensorName)
{
  map < TString, Int_t>::iterator p;
  p=fDetIdByName.find(sensorName);

  if(p!=fDetIdByName.end()){
    return p->second;
  }else{
    cout << " -E- StsDigiScheme::GetDetectorIdByName \"" << sensorName.Data() << "\" not found " << endl;
    return -1;
  }
}
// -------------------------------------------------------------------------

// -----   Public method GetSensorIdByName  --------------------------------
CbmStsSensor* CbmStsDigiScheme::GetSensorByName(TString sensorName)
{
  map < TString, CbmStsSensor*>::iterator p;
  p=fSensorByName.find(sensorName);

  if(p!=fSensorByName.end()){
    return p->second;
  }else{
    cout << " -E- StsDigiScheme::GetDetectorIdByName \"" << sensorName.Data() << "\" not found " << endl;
    return 0;
  }
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
