/** CbmMuchFindHits.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 22.03.07
 *@version 1.0
 **
 ** CBM task class for finding hits in the MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchHit out of CbmMuchDigi.
 **/

#include "CbmMuchFindHits.h"

#include "CbmGeoMuchPar.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchDigiPar.h"
#include "CbmMuchDigiScheme.h"
#include "CbmMuchHit.h"
#include "CbmMuchSector.h"
#include "CbmMuchStation.h"
#include "CbmMuchPad.h"
#include "CbmMuchCluster.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmGeoNode.h" //AZ
#include "CbmGeoRotation.h" //AZ

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h" //AZ

#include <iostream>
#include <iomanip>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::fixed;
using std::left;
using std::right;
using std::setprecision;
using std::set;
using std::map;
using std::vector;

// -----   Default constructor   ------------------------------------------
CbmMuchFindHits::CbmMuchFindHits() : CbmTask("MuchFindHits", 1) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fDigis   = NULL;
  fDigiMatches   = NULL;
  fHits    = NULL;
  fPrimaryClusters = NULL;
  fUseClustering = 0;
  fDigiScheme = CbmMuchDigiScheme::Instance();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchFindHits::CbmMuchFindHits(Int_t iVerbose) 
  : CbmTask("MuchFindHits", iVerbose) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fDigis   = NULL;
  fDigiMatches   = NULL;
  fHits    = NULL;
  fPrimaryClusters = NULL;
  fUseClustering = 0;
  fDigiScheme = CbmMuchDigiScheme::Instance();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmMuchFindHits::CbmMuchFindHits(const char* name, Int_t iVerbose) 
  : CbmTask(name, iVerbose) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fDigis   = NULL;
  fDigiMatches   = NULL;
  fHits    = NULL;
  fPrimaryClusters = NULL;
  fUseClustering = 0;
  fDigiScheme = CbmMuchDigiScheme::Instance();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchFindHits::~CbmMuchFindHits() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
  if ( fDigiScheme ) fDigiScheme->FreeInstance();
}
// -------------------------------------------------------------------------

void CbmMuchFindHits::ExecSimple(Bool_t &warn){
  // Find hits in sectors
  Int_t nDigis = 0;
  Int_t nHits   = 0;
  Int_t nStations = fDigiScheme->GetNStations();
  CbmMuchStation* station = NULL;
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    station = fDigiScheme->GetStation(iStation);
    Int_t nDigisInStation = 0;
    Int_t nHitsInStation   = 0;
    Int_t nSectors = station->GetNSectors();
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmMuchSector* sector = station->GetSector(iSector);
      set <Int_t> digiSet;
      if ( fDigiMap.find(sector) == fDigiMap.end() ) {
	      cout << "-E- " << fName << "::Exec: sector " 
	           << sector->GetSectorNr() << " of station " 
	           << station->GetStationNr() << "not found in front map!" 
	           << endl;
	      warn = kTRUE;
	      continue;
      }
      digiSet = fDigiMap[sector];

      Int_t nDigisInSector = digiSet.size();
      Int_t nHitsInSector   = FindHits(station, sector, digiSet);
      if ( fVerbose > 2 ) cout << "Sector " << sector->GetSectorNr() 
			       << ", Digis " << nDigisInSector 
			       << ", Hits " << nHitsInSector << endl;
      nHitsInStation   += nHitsInSector;
      nDigisInStation += nDigisInSector;
    }      // Sector loop

    if ( fVerbose > 1 ) cout << "Total for station " 
			     << station->GetStationNr() << ": Digis "
			     << nDigisInStation << ", hits "
			     << nHitsInStation << endl;
    nDigis += nDigisInStation;
    nHits   += nHitsInStation;
  }       // Station loop
}

void CbmMuchFindHits::ExecClusters(){
   Int_t nHits = 0;
   // Find primary clusters
   FindClusters();
   
   Int_t nSecClusters = 0;
   // Find secondary clusters
   Int_t nClusters = fPrimaryClusters->GetEntriesFast();
   for(Int_t iCluster = 0; iCluster < nClusters; iCluster++){
      CbmMuchCluster* cluster = (CbmMuchCluster*)fPrimaryClusters->At(iCluster);
      vector<CbmMuchCluster*> secondaryClusters;
      if(cluster) ApplyThreshold(cluster, secondaryClusters);
      nSecClusters += secondaryClusters.size();

      // Produce hits from secondary clusters
      for(vector<CbmMuchCluster*>::iterator it = secondaryClusters.begin(); 
          it != secondaryClusters.end(); it++){
             CbmMuchCluster* secCluster = (*it);
             Double_t x, y, z;
             Double_t xq = 0.0;
             Double_t yq = 0.0;
             Double_t dx = 1e5;
             Double_t dy = 1e5;
             Int_t nClusterDigis = secCluster->GetNDigis();
             CbmMuchPad* pad = NULL;
             for(Int_t iDigi = 0; iDigi < nClusterDigis; iDigi++){
                Int_t digiIndex = secCluster->GetDigiIndex(iDigi);
                Int_t charge = 0;
                pad = GetPadByDigi(digiIndex, charge);
                if(!pad) continue;
                xq += pad->GetX0()*charge;
                yq += pad->GetY0()*charge;
                if(dx > pad->GetDx()) dx = pad->GetDx();
                if(dy > pad->GetDy()) dy = pad->GetDy();
             }

             dx = dx/TMath::Sqrt(12.);
             dy = dy/TMath::Sqrt(12.);

             // Calculate error matrix in sector system
             Double_t vX, vY, vXY;
             vX  = dx;
             vY  = dy;
             vXY = 0.;
    
             // Transform variances into global c.s.
             Int_t stationNr = pad->GetStationNr();
             CbmMuchStation* station = fDigiScheme->GetStationByNr(stationNr);
             if(!station) continue;
             Double_t sinrot = station->GetSinRot();
             Double_t cosrot = station->GetCosRot();
             Double_t wX  = vX * vX  * cosrot * cosrot 
                          - 2. * vXY * cosrot * sinrot
                          + vY * vY  * sinrot * sinrot;
             Double_t wY  = vX * vX  * sinrot * sinrot
                          + 2. * vXY * cosrot * sinrot
                          + vY * vY  * cosrot * cosrot; 
             Double_t wXY = (vX*vX - vY*vY) * cosrot * sinrot
                          + vXY * ( cosrot*cosrot - sinrot*sinrot );
             Double_t sigmaX = TMath::Sqrt(wX);
             Double_t sigmaY = TMath::Sqrt(wY);

             x = xq/secCluster->GetCharge();
             y = yq/secCluster->GetCharge();
             z = station->GetZ();

             TVector3 pos, dpos;
             pos.SetXYZ(x,y,z);
             dpos.SetXYZ(sigmaX,sigmaY,0.);
             new ((*fHits)[nHits++]) CbmMuchHit(station->GetDetectorId(), pos, dpos, wXY, iCluster);//, iDigi, digi->GetTimes(), digi->GetDTime());
             if(secCluster) delete secCluster;
      }
      secondaryClusters.clear();
   }
}

// -----   Public method Exec   --------------------------------------------
void CbmMuchFindHits::Exec(Option_t* opt) {

  Bool_t warn = kFALSE;

  fTimer.Start();

  // Check for digi scheme
  if ( ! fDigiScheme ) {
    cerr << "-E- " << fName << "::Exec: No digi scheme!" << endl;
    return;
  }

  // Clear output array
  if(fHits) fHits->Clear();
  if(fPrimaryClusters) fPrimaryClusters->Delete();//Clear(); // Delete because of memory escape

  // Sort MUCH digis with respect to sectors (fills fDigiMap)
  SortDigis();

  if(fUseClustering){
     ExecClusters();
  }
  else{
     ExecSimple(warn);
  }
  //AZ Hit producer for straw tubes
  Int_t statNr = fDigiScheme->GetNStations() + 1;
  TObjArray* sensNodes = fGeoPar->GetGeoSensitiveNodes();
  TString statName = Form("muchstation%02iL#1",statNr); 
  CbmGeoNode* geoStat = (CbmGeoNode*) (sensNodes->FindObject(statName));
  if (geoStat) ExecStraws(statNr); // straws
  //AZ

  fTimer.Stop();
  if ( fVerbose ) {
    cout << "-I- " << fName << ":Event summary" << endl;
    cout << "    Active channels           : " << fDigis->GetEntriesFast() << endl;
    cout << "    Hits created              : " << fHits->GetEntriesFast()   << endl;
    cout << "    Real time                 : " << fTimer.RealTime() << endl;
    if(fUseClustering)
       cout << "    Clusters                  : " << fPrimaryClusters->GetEntriesFast() << endl;
  }
  else {
    if ( warn ) cout << "- ";
    else        cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) 
	 << fixed << right << fTimer.RealTime() 
	 << " s, digis " << fDigis->GetEntriesFast() << ", hits: " 
	 << fHits->GetEntriesFast() << endl;
  }
}
// -------------------------------------------------------------------------

    


// -----   Private method SetParContainers   -------------------------------
void CbmMuchFindHits::SetParContainers() {

  // Get run and runtime database
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get MUCH geometry parameter container
  fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");

  // Get MUCH digitisation parameter container
  fDigiPar = (CbmMuchDigiPar*) db->getContainer("CbmMuchDigiPar");

}
// -------------------------------------------------------------------------




// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchFindHits::Init() {

  // Get input array
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No CbmRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("MuchDigi");
  fDigiMatches = (TClonesArray*) ioman->GetObject("MuchDigiMatch");

  // Register output array
  fHits = new TClonesArray("CbmMuchHit", 1000);
  ioman->Register("MuchHit", "Hit in MUCH", fHits, kTRUE);

  fPrimaryClusters = new TClonesArray("CbmMuchCluster", 1000);
  ioman->Register("MuchCluster", "Cluster in MUCH", fPrimaryClusters, kTRUE);

  // Build digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;
  if(fUseClustering) fDigiScheme->InitGrid();

  // Create sectorwise digi sets
  MakeSets();

  // Control output
  if      (fVerbose >= 1) fDigiScheme->Print();
  cout << "-I- " << fName << "::Init: "
       << "MUCH digitization scheme succesfully initialised" << endl;
  cout << "    Stations: " << fDigiScheme->GetNStations()
       << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: "
       << fDigiScheme->GetNChannels() << endl;

  if(!fUseClustering)
    cout << "Using simple algorithm for MUCH hit producer..." << endl;
  else
    cout << "Using clustering algorithm for MUCH hit producer..." << endl;

  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchFindHits::ReInit() {

  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;
  if(fUseClustering) fDigiScheme->InitGrid();

  // Create sectorwise digi sets
  MakeSets();

  // Control output
  if      (fVerbose >= 1) fDigiScheme->Print();
  cout << "-I- " << fName << "::Init: "
       << "MUCH digitization scheme succesfully reinitialised" << endl;
  cout << "    Stations: " << fDigiScheme->GetNStations() 
       << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
       << fDigiScheme->GetNChannels() << endl;
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method MakeSets   ---------------------------------------
void CbmMuchFindHits::MakeSets() {
  fDigiMap.clear();
  Int_t nStations = fDigiScheme->GetNStations();
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    CbmMuchStation* station = fDigiScheme->GetStation(iStation);
    Int_t nSectors = station->GetNSectors();
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmMuchSector* sector = station->GetSector(iSector);
      set<Int_t> a;
      fDigiMap[sector] = a;
    }
  }
}
// -------------------------------------------------------------------------




// -----   Private method SortDigis   --------------------------------------
void CbmMuchFindHits::SortDigis() {

  // Check input array
  if ( ! fDigis ) {
    cout << "-E- " << fName << "::SortDigis: No input array!" << endl;
    return;
  }

  // Clear sector digi sets
  map<CbmMuchSector*, set<Int_t> >::iterator mapIt;
  for (mapIt=fDigiMap.begin(); mapIt!=fDigiMap.end(); mapIt++)
    ((*mapIt).second).clear();

  // Fill digis into sets
  CbmMuchDigi* digi = NULL;
  CbmMuchSector* sector = NULL;
  Int_t stationNr = -1;
  Int_t sectorNr  = -1;
  Int_t nDigis = fDigis->GetEntriesFast();
  for (Int_t iDigi=0; iDigi<nDigis; iDigi++) {
    digi = (CbmMuchDigi*) fDigis->At(iDigi);
    stationNr = digi->GetStationNr();
    sectorNr  = digi->GetSectorNr();
    sector = fDigiScheme->GetSector(stationNr, sectorNr);
    if (sector == 0x0) continue; //AZ - to allow straws
    if ( fDigiMap.find(sector) == fDigiMap.end() ) {
      cerr << "-E- " << fName << "::SortDigis:: sector " << sectorNr
	   << " of station " << stationNr 
	   << " not found in digi scheme (F)!" << endl;
      Fatal("STOP", "STOP");
      continue;
    }
    fDigiMap[sector].insert(iDigi);
  }
}
// -------------------------------------------------------------------------


// -----   Private method FindChannelDigiMap  ------------------------------
void CbmMuchFindHits::FillChannelDigiMap(){
   fChannelDigiMap.clear();
   Int_t nDigis = fDigis->GetEntriesFast();
   for(Int_t iDigi = 0; iDigi < nDigis; iDigi++){
      CbmMuchDigi* digi = (CbmMuchDigi*)fDigis->At(iDigi);
      if(!digi) continue;
      Int_t detectorId = digi->GetDetectorId();
      Int_t channelNr = digi->GetChannelNr();
      pair<Int_t, Int_t> channelId(detectorId, channelNr);
      if(fChannelDigiMap.find(channelId) == fChannelDigiMap.end())
         fChannelDigiMap[channelId] = iDigi;
   }

}
// -------------------------------------------------------------------------

// -----   Private method FindClusters  ------------------------------------
void CbmMuchFindHits::FindClusters(){
   FillChannelDigiMap();

   fSelectedDigis.clear();
   Int_t nClusters = 0;
   Int_t nDigis = fDigis->GetEntriesFast();
   Int_t dummyCharge = 0;
   for(Int_t iDigi=0; iDigi<nDigis; iDigi++){
      // Selection
      if(fSelectedDigis.find(iDigi) != fSelectedDigis.end()) continue;

      vector<Int_t> digiIndices;
      Int_t qMax = 0;
      Int_t sumCharge = 0;
      CreateCluster(iDigi, digiIndices, sumCharge, qMax);
      new ((*fPrimaryClusters)[nClusters]) CbmMuchCluster(digiIndices, sumCharge, qMax);
      // Match digis to the parent cluster
      SetDigiClusterMatch(digiIndices, nClusters);
      nClusters++;
   }
}
// -------------------------------------------------------------------------

// -----   Private method SetDigiClusterMatch  -----------------------------
void CbmMuchFindHits::SetDigiClusterMatch(vector<Int_t> digiIndices, Int_t clusterIndex){
   for(vector<Int_t>::iterator it = digiIndices.begin(); it!=digiIndices.end(); it++){
      Int_t idx = *it;
      CbmMuchDigiMatch* digiMatch = (CbmMuchDigiMatch*)fDigiMatches->At(idx);
      if(!digiMatch) continue;
      digiMatch->SetCluster(clusterIndex);
   }
}
// -------------------------------------------------------------------------

// -----   Private method CreateCluster  -----------------------------------
void CbmMuchFindHits::CreateCluster(Int_t iDigi, vector<Int_t> &digiIndices, Int_t &sumCharge, Int_t &qMax, Int_t qThreshold){
   digiIndices.push_back(iDigi);
   fSelectedDigis.insert(iDigi);
   Int_t charge = 0;
   CbmMuchPad* pad = GetPadByDigi(iDigi, charge);
   if(!pad) return;
   if(charge > qMax) qMax = charge;
   sumCharge += charge;
   map<pair<Int_t, Int_t>, CbmMuchPad*> neighbourPads = pad->GetNeighbours();
   for(map<pair<Int_t, Int_t>, CbmMuchPad*>::iterator it = neighbourPads.begin(); it != neighbourPads.end(); it++ ){
      pair<Int_t, Int_t> channelId = (*it).first;
      CbmMuchPad* neighbourPad = (*it).second;
      if(fChannelDigiMap.find(channelId) == fChannelDigiMap.end()) continue;
      
      Int_t digiIndex = fChannelDigiMap[channelId];
      CbmMuchDigi* neighbourDigi = (CbmMuchDigi*)fDigis->At(digiIndex);
      if(!neighbourDigi) continue;
      Int_t digiCharge = neighbourDigi->GetCharge();
      if(digiCharge <= qThreshold) continue;

      // Selection
      if(fSelectedDigis.find(digiIndex)!=fSelectedDigis.end()) continue;

      CreateCluster(digiIndex, digiIndices, sumCharge, qMax, qThreshold);
   }
}
// -------------------------------------------------------------------------

// -----   Private method ApplyThreshold  ----------------------------------
void CbmMuchFindHits::ApplyThreshold(CbmMuchCluster* cluster, vector<CbmMuchCluster*> &newClusters){
   fSelectedDigis.clear();
   Int_t qThreshold = cluster->GetMaxCharge()/10;
   Int_t dummy = 0;
   for(Int_t iDigi = 0; iDigi<cluster->GetNDigis(); iDigi++){
      Int_t digiIndex = cluster->GetDigiIndex(iDigi);
      Int_t charge = 0;
      CbmMuchPad* pad = GetPadByDigi(digiIndex, charge);
      if(!pad) continue;
      if(charge <= qThreshold) continue;

      // Selection
      if(fSelectedDigis.find(digiIndex)!=fSelectedDigis.end()) continue;
      //if(pad->GetSecondarySelected()) continue;

      vector<Int_t> digiIndices;
      Int_t sumCharge = 0;
      CreateCluster(digiIndex, digiIndices, sumCharge, dummy, qThreshold);
      newClusters.push_back(new CbmMuchCluster(digiIndices, sumCharge));
   }
}
// -------------------------------------------------------------------------

// -----   Private method GetPadByDigi  ------------------------------------
CbmMuchPad* CbmMuchFindHits::GetPadByDigi(Int_t digiIndex, Int_t &charge){
   CbmMuchDigi* digi = (CbmMuchDigi*)fDigis->At(digiIndex);
   if(digi){
      charge = digi->GetCharge();
      Int_t stationNr = digi->GetStationNr();
      Int_t sectorNr = digi->GetSectorNr();
      Int_t channelNr = digi->GetChannelNr();
      CbmMuchSector* sector = fDigiScheme->GetSector(stationNr, sectorNr);
      if(sector) return sector->GetPad(channelNr);
   }
   return NULL;
}
// -------------------------------------------------------------------------


// -----   Private method FindHits   ---------------------------------------
Int_t CbmMuchFindHits::FindHits(CbmMuchStation* station,
				CbmMuchSector* sector, 
				set<Int_t>& digiSet) {

  // Counter
  Int_t nNew = 0;

  // Get sector parameters
  Int_t    detId  = sector->GetDetectorId();
  Int_t    iType  = sector->GetType();
  Double_t xc     = sector->GetX0();
  Double_t yc     = sector->GetY0();
  Double_t lx     = sector->GetLx();
  Double_t ly     = sector->GetLy();
  Int_t nColumns  = sector->GetNCols();
  Int_t nRows     = sector->GetNRows();
  Double_t dx     = lx/nColumns;
  Double_t dy     = ly/nRows;
  Double_t z      = station->GetZ();
  Int_t stationNr = station->GetStationNr();
  Int_t sectorNr  = sector->GetSectorNr();

  // Some auxiliary values
  Double_t sinrot = sector->GetSinRot();
  Double_t cosrot = sector->GetCosRot();

  // Calculate error matrix in sector system
  Double_t vX, vY, vXY;
  if ( iType == 1 || iType == 2) {
    vX  = dx / TMath::Sqrt(12.);
    vY  = dy / TMath::Sqrt(12.);
    vXY = 0.;
  }
  else {
    cerr << "-E- " << fName << "::FindHits: Illegal sector type "
	 << iType << endl;
    return 0;
  }
    
  // Transform variances into global c.s.
  Double_t wX  = vX * vX  * cosrot * cosrot 
    - 2. * vXY * cosrot * sinrot
    + vY * vY  * sinrot * sinrot;
  Double_t wY  = vX * vX  * sinrot * sinrot
    + 2. * vXY * cosrot * sinrot
    + vY * vY  * cosrot * cosrot; 
  Double_t wXY = (vX*vX - vY*vY) * cosrot * sinrot
    + vXY * ( cosrot*cosrot - sinrot*sinrot );
  Double_t sigmaX = TMath::Sqrt(wX);
  Double_t sigmaY = TMath::Sqrt(wY);

  // Now perform the loop over active channels
  set<Int_t>::iterator iter;

  // ----- Types 1,2 : Pad-type sector   ---------------------------------------
  Int_t iDigi = -1;
  Int_t iChan = -1;
  Int_t iCol  = -1;
  Int_t iRow  = -1;
  Int_t nHits = fHits->GetEntriesFast();
  Double_t xint, yint;
  Double_t x, y;
  TVector3 pos, dpos;
  CbmMuchDigi* digi = NULL;
  for (iter=digiSet.begin(); iter!=digiSet.end(); iter++) {
    iDigi = (*iter);
    digi  = (CbmMuchDigi*) fDigis->At(iDigi);
    if ( ! digi ) {
      cout << "-W- " << GetName() << "::FindHits: Invalid digi index "
	   << iDigi << " in the sector "
	   << sector->GetSectorNr() << ", station "
	   << station->GetStationNr() << endl;
      continue;
    }
    iChan = digi->GetChannelNr();
    iRow  = Int_t( iChan / nColumns );
    iCol  = iChan - iRow * nColumns;
    xint = ( Double_t(iCol) + 0.5 ) * dx;
    yint = ( Double_t(iRow) + 0.5 ) * dy;
      
    // Translation to centre of sector
    xint = xint - lx/2.;
    yint = yint - ly/2.;

    // Rotation around sector centre
    x = xint * cosrot - yint * sinrot;
    y = xint * sinrot + yint * cosrot;

    // Translation into global c.s.
    x = x + xc;
    y = y + yc;

    // Make new hit
    pos.SetXYZ(x, y, z);
    dpos.SetXYZ(sigmaX, sigmaY, 0.);
    new ((*fHits)[nHits++]) CbmMuchHit(detId, pos, dpos, wXY, iDigi, digi->GetTimes(), digi->GetDTime());
//    CbmMuchHit* muchHit = dynamic_cast<CbmMuchHit*>(fHits->At(nHits-1));
//    Int_t sysId = muchHit->GetSystemId();
//    cout << "systemId = " << sysId << endl;
    nNew++;
    if ( fVerbose > 3 ) cout << "New MuchHit at (" << x << ", " << y 
			     << ", " << z << "), station " << stationNr
			     << ", sector " << sectorNr << ", channel "
			     << iChan << endl;
  }    // Loop over digi set

  // ---------------------------------------------------------------------

  return nNew;
}
// -------------------------------------------------------------------------

//AZ ---------   Private method ExecStraws   -------------------------------
void CbmMuchFindHits::ExecStraws(Int_t begStation) 
{
  // Make hits in straw tubes

  static Int_t first = 1;
  static CbmGeoRotation rotMatr[3];
  static Double_t radIn[4];
  Double_t diam[4] = {0.4, 0.4, 0.4, 0.4}; // tube diameters
  Double_t sigmaX = 0.02, sigmaY = 0.02; // 200um

  if (first) {
    // Some initialization - should go somewhere else
    cout << " Straws ... " << endl;
    first = 0;
    Double_t phi[3] = {0., 90., 45. }; // view rotation angles
    //Double_t phi[3] = {0., 10., -10. }; // view rotation angles
    for (Int_t i = 0; i < 3; ++i) {
      phi[i] *= TMath::DegToRad();
      rotMatr[i].setElement(TMath::Cos(phi[i]),0);
      rotMatr[i].setElement(TMath::Cos(phi[i]),4);
      rotMatr[i].setElement(TMath::Sin(phi[i]),1);
      rotMatr[i].setElement(-TMath::Sin(phi[i]),3);
      rotMatr[i].print();
      rotMatr[i].invert();
      rotMatr[i].print();
    }
    // Get inner radia 
    TObjArray* sensNodes = fGeoPar->GetGeoSensitiveNodes();
    for (Int_t i = 0; i < 3; ++i) {
      TString statName = Form("muchstation%02iL#1",begStation+3*i); 
      CbmGeoNode *node = (CbmGeoNode*) sensNodes->FindObject(statName);
      TArrayD *pars = node->getParameters();
      //cout << pars->At(4) << endl;
      radIn[i] = pars->At(4);
    }
  }

  Int_t nDigis = fDigis->GetEntriesFast(), nHits = fHits->GetEntriesFast();
  TVector3 pos, dpos;
  Double_t xyz[3] = {0};
  for (Int_t idig = 0; idig < nDigis; ++idig) {
    CbmMuchDigi *digi = (CbmMuchDigi*) fDigis->UncheckedAt(idig);
    if (digi->GetStationNr() < begStation) continue; // not straw
    Int_t signs = digi->GetUniqueID();
    for (Int_t i = 0; i < 3; ++i) {
      xyz[i] = digi->GetTimes()[i];
      if (signs & (1<<i)) xyz[i] = -xyz[i]; 
      //cout << xyz[i] << " ";
      //if (i == 2) cout << endl;
    }

    Int_t layer = digi->GetSectorNr();
    Int_t station = digi->GetStationNr() - begStation;
    Int_t rot = station % 3;
    Int_t station3 = station / 3; // triple station
    CbmGeoVector p(xyz[0], xyz[1], xyz[2]);
    CbmGeoVector ploc = rotMatr[rot] * p;
    Double_t xloc = ploc.getX();
    if (layer % 2 != 0) xloc += diam[station3] / 2.; // half-tube shift 
    Int_t itube = (Int_t) (xloc / diam[station3]);
    if (xloc < 0) itube--;
    cout << itube << " " << station3 << endl;
    Double_t *times = digi->GetTimes();

    if (TMath::Abs(ploc.getX()) < radIn[station3]) {
      if (ploc.getY() > 0) times[1] = 1;
      else times[1] = -1;
    } else times[1] = 0;

    // Global coordinates in rotated coordinate system
    Double_t errU = gRandom->Gaus(0,sigmaX);
    //hit->SetU(ploc.getX()+errU);
    times[0] = ploc.getX() + errU;
    Double_t wXY = rotMatr[rot](1);

    pos.SetXYZ(xyz[0], xyz[1], xyz[2]);
    dpos.SetXYZ(sigmaX, sigmaY, 0.);
    // Change station No.
    station = begStation + station * 2 + layer;
    Int_t detId = digi->GetDetectorId();
    detId = detId & ~(255 << 16); // reset station No.
    detId |= (station << 16);   // station No. on bits 16-23
    
    times[2] = -77777;
    CbmMuchHit *hit = 
      new ((*fHits)[nHits++]) CbmMuchHit(detId, pos, dpos, wXY, idig, 
					 times, digi->GetDTime());
    hit->SetCluster(itube); // tube number
    //new ((*fHits)[nHits++]) CbmMuchHit(detId, pos, dpos, wXY, iDigi, digi->GetTimes(), digi->GetDTime());
  } 
}

ClassImp(CbmMuchFindHits)
