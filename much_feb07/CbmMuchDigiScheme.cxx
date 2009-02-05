/** CbmMuchDigiScheme.cxx
 **@author  Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 **@since   15.03.07
 **@version 1.0
 **
 ** Auxiliary class for MuCh digitization and hit finding.
 ** Controls the digitisation scheme of the stations and sectors.
 **/


#include "TArrayD.h"

#include "CbmGeoMedium.h"
#include "CbmGeoNode.h"
#include "CbmGeoTransform.h"
#include "CbmGeoVector.h"
#include "CbmRootManager.h"

#include "CbmGeoMuchPar.h"
#include "CbmMuchDigiPar.h"
#include "CbmMuchDigiScheme.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"
#include "CbmMuchDigi.h"
#include "CbmMuchSectorDigiPar.h"
#include "CbmMuchStation.h"
#include "CbmMuchStationDigiPar.h"

using std::cout;
using std::endl;
using std::flush;

CbmMuchDigiScheme* CbmMuchDigiScheme::fInstance = NULL;
Bool_t CbmMuchDigiScheme::fInitialized = kFALSE;
Bool_t CbmMuchDigiScheme::fGridInitialized = kFALSE;
Int_t CbmMuchDigiScheme::fRefcount = 0;

// -----   Constructor   ---------------------------------------------------
CbmMuchDigiScheme::CbmMuchDigiScheme() {
  fStations = new TObjArray(10);
  fNSectors = fNChannels = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchDigiScheme::~CbmMuchDigiScheme() {
  if ( fStations) {
    fStations->Delete();
    delete fStations;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Instance  -----------------------------------------
CbmMuchDigiScheme* CbmMuchDigiScheme::Instance(){
   if(!fInstance) fInstance = new CbmMuchDigiScheme();
   fRefcount++;
   return fInstance;
}
// -------------------------------------------------------------------------

// -----   Public method FreeInstance   ------------------------------------
void CbmMuchDigiScheme::FreeInstance(){
   fRefcount--;
   if(!fRefcount){
      delete this;
      fInstance = NULL;
   }
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
Bool_t CbmMuchDigiScheme::Init(CbmGeoMuchPar*  geoPar, 
			      CbmMuchDigiPar* digiPar) {

  if(!fInitialized){
     // Check availability of parameters
     if ( ! geoPar ) {
       cout << "-W- CbmMuchDigiScheme::Init: "
       << "No geometry parameters available!" << endl;
       return kFALSE;
     }
     if ( ! digiPar ) {
       cout << "-W-  CbmMuchDigiScheme::Init: "
       << "No digitization parameters available!" << endl;
       return kFALSE;
     }
     
     // Loop over stations in DigiPar
     Int_t nStations = digiPar->GetNStations();
     CbmMuchStationDigiPar* statPar = NULL;
     CbmMuchSectorDigiPar* sectorPar = NULL;
     for (Int_t iStation=0; iStation<nStations; iStation++) {

       // Get digitisation parameters of station
       statPar = (CbmMuchStationDigiPar*) digiPar->GetStation(iStation);
       Int_t    statNr   = statPar->GetStationNr();
       Double_t statRot  = statPar->GetRotation();
       Int_t    nSectors = statPar->GetNSectors();

       // Get geometry parameters of station
       TString statName = Form("muchstation%02i",statNr); 
       TObjArray* sensNodes = geoPar->GetGeoSensitiveNodes();
       CbmGeoNode* geoStat = (CbmGeoNode*) (sensNodes->FindObject(statName));
       if ( ! geoStat ) {
         cout << "-W- CbmMuchDigiScheme::Init: " << statName 
	   //<< " not found among sensitive nodes " << flush;
	      << " not found among sensitive nodes " << endl; //AZ
	 //AZ - check if this due to straw tube stations
	 statName = Form("muchstation%02iL#1",statNr);
	 if (sensNodes->FindObject(statName)) {
	   // Straw tubes - remove them from digi scheme
	   for (Int_t i = iStation; i < nStations; ++i) 
	     digiPar->GetStations()->RemoveAt(i);
	   break;
	 }
	 //AZ
         return kFALSE;
       }
       CbmGeoTransform* transform = geoStat->getLabTransform();
       CbmGeoVector translat = transform->getTranslation();
       CbmGeoTransform center = geoStat->getCenterPosition();
       CbmGeoVector centerV = center.getTranslation();
       TArrayD* params = geoStat->getParameters();
       CbmGeoMedium* material = geoStat->getMedium();
       material->calcRadiationLength();

       Double_t statRmin = params->At(7);
       Double_t statRmax = params->At(8);
       Double_t statD    = params->At(6) - params->At(3);
       Double_t statZ    = translat.Z() + centerV.Z() + params->At(3) + statD/2.;
       Double_t statRL   = material->getRadiationLength();
       Int_t    mcId     = geoStat->getMCid();

       // Create new station
       CbmMuchStation* station = new CbmMuchStation(statName, statNr, statZ,
                         statD, statRL, statRmin,
                         statRmax, statRot);
       fStations->Add(station);
       fStationMap[statNr] = station;
       fVolumeMap[mcId] = station;

       // Loop over sectors of station in DigiPar
       for (Int_t iSector=0; iSector<nSectors; iSector++) {
         sectorPar = statPar->GetSector(iSector);
         sectorPar->SetD(statD);
         sectorPar->SetZ0(statZ); 
         station->AddSector(sectorPar);
       }

       fNSectors  += station->GetNSectors();
       fNChannels += station->GetNChannels();

     } // Loop over stations

     fInitialized = kTRUE;
  }
  return kTRUE;

}
// -------------------------------------------------------------------------

// -----   Public method InitGrid   ----------------------------------------
void CbmMuchDigiScheme::InitGrid(){
   if(!fGridInitialized){
     CbmMuchStation* station = NULL;
     for (Int_t iStation=0; iStation < GetNStations(); iStation++) {
        station = (CbmMuchStation*)fStations->At(iStation);
        // Init grid for fast search within the station
        station->InitGrid();
        // Add neighbour sectors info
        station->InitNeighbourSectors();
        // Add neighbour pads info
        station->InitNeighbourPads();
     }
     fGridInitialized = kTRUE;
   }
}
// -------------------------------------------------------------------------

// -----   Public method Clear   -------------------------------------------
void CbmMuchDigiScheme::Clear() {
  fStations->Delete();

  fStationMap.clear();
  fVolumeMap.clear();

  fNSectors = fNChannels = 0;
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmMuchDigiScheme::Reset() {
  for (Int_t iStation=0; iStation<fStations->GetEntries(); iStation++) 
    GetStation(iStation)->Reset();
}
// -------------------------------------------------------------------------



// -----   Public method GetStation   --------------------------------------
CbmMuchStation* CbmMuchDigiScheme::GetStation(Int_t iStation) {
  if ( ! fStations ) return NULL;
  return dynamic_cast<CbmMuchStation*>(fStations->At(iStation));
}
// -------------------------------------------------------------------------



// -----   Public method GetStationByNr   ----------------------------------
CbmMuchStation* CbmMuchDigiScheme::GetStationByNr(Int_t stationNr) {
  if ( ! fStations ) return NULL;
  if ( fStationMap.find(stationNr) == fStationMap.end() ) return NULL;
  return fStationMap[stationNr];
}
// -------------------------------------------------------------------------



// -----   Public method GetStationByMcId   --------------------------------
CbmMuchStation* CbmMuchDigiScheme::GetStationByMcId(Int_t iVol) {
  if ( ! fStations ) return NULL;
  if ( fVolumeMap.find(iVol) == fStationMap.end() ) return NULL;
  return fVolumeMap[iVol];
}
// -------------------------------------------------------------------------

// -----   Public method GetSector  ----------------------------------------
CbmMuchSector* CbmMuchDigiScheme::GetSector(Int_t stationNr, Int_t sectorNr){
  CbmMuchStation* station = GetStationByNr(stationNr);
  return station == NULL? NULL : station->GetSectorByNr(sectorNr);
}
// -------------------------------------------------------------------------

// -----   Public method GetPad  -------------------------------------------
CbmMuchPad* CbmMuchDigiScheme::GetPad(CbmMuchDigi* digi){
   if(!digi) return NULL;
   Int_t stationNr = digi->GetStationNr();
   Int_t sectorNr  = digi->GetSectorNr();
   Int_t channelNr = digi->GetChannelNr();
   CbmMuchSector* sector = GetSector(stationNr, sectorNr);
   return sector->GetPad(channelNr);
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void CbmMuchDigiScheme::Print() {
  cout << endl;
  cout << "===================  MuCh digitization scheme   ================="
       << endl;
  for (Int_t iStat=0; iStat<GetNStations(); iStat++) {
    CbmMuchStation* station = GetStation(iStat);
    station->Print();
  }
  cout << "================================================================="
       << endl << endl;
}
// -------------------------------------------------------------------------

  


ClassImp(CbmMuchDigiScheme)
