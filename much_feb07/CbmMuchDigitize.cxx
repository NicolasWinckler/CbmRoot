/** CbmMuchDigitize.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 19.03.07
 *@version 1.0
 **
 ** CBM task class for digitizing MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchDigi out of CbmMuchPoint.
 **/


// Includes from MUCH
#include "CbmGeoMuchPar.h"
#include "CbmMuchDigi.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchDigiPar.h"
#include "CbmMuchDigiScheme.h"
#include "CbmMuchDigitize.h"
#include "CbmMuchPoint.h"
#include "CbmMuchSector.h"
#include "CbmMuchStation.h"
#include "CbmMuchPad.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "CbmMCTrack.h"
#include "FairMCPoint.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TDatabasePDG.h"
#include "TFile.h"

#include <iostream>
#include <iomanip>
#include <cassert>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::fixed;
using std::left;
using std::right;
using std::setprecision;
using std::map;
using std::pair;

// -----   Default constructor   ------------------------------------------
CbmMuchDigitize::CbmMuchDigitize() : FairTask("MuchDigitize", 1) {
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fDTime = 8e-2;
  fQMax = 0;
  fQThreshold = 0;
  fUseAvalanche = 0;
  SetDetectorType(kMICROMEGAS);
  fMeanGasGain = 1e4;
  fDigiScheme  = CbmMuchDigiScheme::Instance();
  fRnd = new TRandom3();
  fLandauRnd = new TRandom();

  Reset();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchDigitize::CbmMuchDigitize(Int_t iVerbose) 
  : FairTask("MuchDigitize", iVerbose) { 
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fDTime = 8e-2;
  fQMax = 0;
  fQThreshold = 0;
  fUseAvalanche = 0;
  SetDetectorType(kMICROMEGAS);
  fMeanGasGain = 1e4;
  fDigiScheme  = CbmMuchDigiScheme::Instance();
  fRnd = new TRandom3();
  fLandauRnd = new TRandom();

  Reset();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmMuchDigitize::CbmMuchDigitize(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) { 
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fDTime = 8e-2;
  fQMax = 0;
  fQThreshold = 0;
  fUseAvalanche = 0;
  SetDetectorType(kMICROMEGAS);
  fMeanGasGain = 1e4;
  fDigiScheme  = CbmMuchDigiScheme::Instance();
  fRnd = new TRandom3();
  fLandauRnd = new TRandom();

  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchDigitize::~CbmMuchDigitize() { 
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
  if ( fDigiScheme ) fDigiScheme->FreeInstance();
  fUseAvalanche = 0;
  SetDetectorType(kMICROMEGAS);

  Reset();
}
// -------------------------------------------------------------------------

// ------- Private method ExecSimple ---------------------------------------
Bool_t CbmMuchDigitize::ExecSimple(CbmMuchStation* station, CbmMuchPoint* point,
				   Int_t iPoint, Int_t& inc){

  Double_t xIn = point->GetXIn();
  Double_t yIn = point->GetYIn();
  Double_t xOut = point->GetXOut();
  Double_t yOut = point->GetYOut();
  Double_t sinRot = station->GetSinRot();
  Double_t cosRot = station->GetCosRot();
  
  // Get avalanche center position
  Double_t x0  = (xIn+xOut)/2;
  Double_t y0  = (yIn+yOut)/2;

  // Find fired sector/pad (fast algorithm)
  Double_t gridDx = station->GetGridDx();
  Double_t gridDy = station->GetGridDy();
  Double_t gridLx = station->GetGridLx();
  Double_t gridLy = station->GetGridLy();
  // Transform to rotated system (for grid purpose)
  Double_t x0_rot = x0*cosRot + y0*sinRot;
  Double_t y0_rot = y0*cosRot - x0*sinRot;
  Int_t iGridColumn = (Int_t)((x0_rot + gridLx/2.)/gridDx);
  Int_t iGridRow    = (Int_t)((y0_rot + gridLy/2.)/gridDy);

  if(iGridRow > station->GetGridRows() - 1 ||
     iGridColumn > station->GetGridCols() - 1) return kFALSE;
  
  CbmMuchSector* sector = station->GetSector(iGridRow, iGridColumn);
  Int_t iChannel = -1;
  if(sector) {
    iChannel = sector->GetChannel(x0,y0);
    // Ask neighbour sectors if needed
    if(iChannel < 0){
      map<Int_t, CbmMuchSector*> neighbours = sector->GetNeighbours();
      for(map<Int_t, CbmMuchSector*>::iterator it=neighbours.begin(); it!=neighbours.end(); it++){
    	   sector = (*it).second;
	      if(!sector) continue;
    	   iChannel = sector->GetChannel(x0,y0);
	      if(iChannel >=0) break;
      }
    }
  }
  if ( iChannel < 0 ){
    fNOutside++;
    //if(sector) delete sector;
    return kFALSE;
  }

  Int_t iDetectorId  = sector->GetDetectorId();
  Int_t stationNr = station->GetStationNr();
  Int_t sectorNr = sector->GetSectorNr();

  pair<Int_t, Int_t> aa(iDetectorId, iChannel);
  Int_t iDigi = -1;
  if ( fChannelMap.find(aa) == fChannelMap.end() ) {
    // Channel not yet active. Create new Digi and Match.
    iDigi = fNDigis;
    Double_t time = point->GetTime() + gRandom->Gaus(0,fDTime);
    new ((*fDigis)[fNDigis]) CbmMuchDigi(stationNr, sectorNr, iChannel, time, fDTime);
    new ((*fDigiMatches)[fNDigis]) CbmMuchDigiMatch(iPoint);
    // Match pair (iDetectorId, iChannel) to index of the Digi.
    fChannelMap[aa] = iDigi;
    fNDigis++;
  }
  else {
    // Channel already active. 
    iDigi = fChannelMap[aa];
    CbmMuchDigi* digi = dynamic_cast<CbmMuchDigi*>(fDigis->At(iDigi));
    Double_t time = point->GetTime() + gRandom->Gaus(0,fDTime);
    digi->AddTime(time); // add time info
    CbmMuchDigiMatch* match = dynamic_cast<CbmMuchDigiMatch*>(fDigiMatches->At(iDigi));
    if ( match ) {
      match->AddPoint(iPoint);
      fNMulti++;
    }
  }

  return kTRUE;
}
// -------------------------------------------------------------------------

// ------- Private method ExecAdvanced -------------------------------------
Bool_t CbmMuchDigitize::ExecAdvanced(CbmMuchStation* station, CbmMuchPoint* point, 
				     Int_t iPoint){
  Int_t stationNr = station->GetStationNr();
  Double_t sinRot = station->GetSinRot();
  Double_t cosRot = station->GetCosRot();
  Double_t gridDx = station->GetGridDx();
  Double_t gridDy = station->GetGridDy();
  Double_t gridLx = station->GetGridLx();
  Double_t gridLy = station->GetGridLy();
 
  //**************  Simulate avalanche (begin) *********************************//
 
  // Get track length within the station
  Double_t xIn    = point->GetXIn();
  Double_t yIn    = point->GetYIn();
  Double_t xOut   = point->GetXOut();
  Double_t yOut   = point->GetYOut();
  Double_t zIn    = point->GetZIn();
  Double_t zOut   = point->GetZOut();
  Double_t deltaX = xOut - xIn;
  Double_t deltaY = yOut - yIn;
  Double_t deltaZ = zOut - zIn;
  Double_t lTrack = sqrt(deltaX*deltaX+
			 deltaY*deltaY + deltaZ*deltaZ); // track length

  //********** Primary electrons from the track (begin) ************************//
  // Get particle's characteristics
  Int_t trackID = point->GetTrackID();
  if(trackID < 0) {return kFALSE;}
  CbmMCTrack* mcTrack = (CbmMCTrack*)fMCTracks->At(trackID);
  if(NULL == mcTrack) {return kFALSE;}
  Int_t pdgCode = mcTrack->GetPdgCode(); 

  // Reject funny particles
  if(pdgCode >  10000000) return kFALSE;
 
  TParticlePDG *particle = TDatabasePDG::Instance()->GetParticle(pdgCode);
  if(!particle){
     cout << "ERROR!!! Pdg code is " << pdgCode << endl;
     return kFALSE;
  }
  if(TMath::Abs(particle->Charge()) < 0.1) return kFALSE;
  TVector3 momentum;                                    // 3-momentum of the particle
  point->Momentum(momentum); 
  Double_t mom = momentum.Mag()*1e3;                    // absolute momentum value [MeV/c]
  Double_t mom2 = mom*mom;                              // squared momentum of the particle
  Double_t mass = particle->Mass()*1e3;                 // mass of the particle [MeV/c^2]
  Double_t mass2 = mass*mass;                           // squared mass of the particle
  Double_t Tkin = sqrt(mom2 + mass2) - mass;            // kinetic energy of the particle
  Double_t sigma = CbmMuchDigitize::Sigma_n_e(Tkin, mass);                   // sigma for Landau distribution
  Double_t mpv = CbmMuchDigitize::MPV_n_e(Tkin, mass);                       // most probable value for Landau distr.
  UInt_t nElectrons = (UInt_t) fLandauRnd->Landau(mpv, sigma); // number of prim. electrons per 0.3 cm gap
  // Number of electrons for current track length
  if(mass < 100.) nElectrons = (UInt_t)(nElectrons*lTrack/0.47);
  else nElectrons = (UInt_t)(nElectrons*lTrack/0.36);
  //********** Primary electrons from the track (end)   ***********************//  

  Double_t hypotenuse = sqrt(deltaX*deltaX + deltaY*deltaY);
  Double_t cosphi_tr = deltaX/hypotenuse;     // cos of track azim. angle
  Double_t sinphi_tr = deltaY/hypotenuse;     // sin of track azim. angle

  map<pair<Int_t, Int_t>, CbmMuchDigi*> chargedPads;
  Double_t time = point->GetTime();
  UInt_t nTrackCharge = 0; // Total charge left by a track
  for (Int_t iElectron = 0; iElectron < nElectrons ; iElectron++){
     Double_t aL = fRnd->Rndm()*hypotenuse;
     Double_t xe = xIn + aL*cosphi_tr;
     Double_t ye = yIn + aL*sinphi_tr;

     // Calculate number of secondary electrons for each primary electron
     UInt_t nSecElectrons = GasGain();                    // number of secondary electrons
     Double_t spotL       = fSpotRadius*2.;               // diameter of the spot
     Double_t spotArea    = spotL*spotL;                  // spot area

     // Find intersection of spot with sectors
     TPolyLine spotPolygon = GetPolygon(xe, ye, spotL, spotL, sinRot, cosRot);
     Double_t* xVertex = spotPolygon.GetX();
     Double_t* yVertex = spotPolygon.GetY();
     map<Int_t, CbmMuchSector*> firedSectors;
     for(Int_t iVertex = 0; iVertex < spotPolygon.GetN() - 1; iVertex++){
       // Transform to rotated system (for grid purpose)
       Double_t xeRot = xVertex[iVertex]*cosRot + yVertex[iVertex]*sinRot;
       Double_t yeRot = yVertex[iVertex]*cosRot - xVertex[iVertex]*sinRot;
       Int_t iGridColumn = (Int_t)((xeRot + gridLx/2.)/gridDx);
       Int_t iGridRow    = (Int_t)((yeRot + gridLy/2.)/gridDy);
       if(iGridRow > station->GetGridRows() - 1 ||
          iGridColumn > station->GetGridCols() - 1) continue;
       
       CbmMuchSector* sector = station->GetSector(iGridRow, iGridColumn);
       
       if(sector){
	 Int_t detID = sector->GetDetectorId();
	 if(firedSectors.find(detID) == firedSectors.end())
	   firedSectors[detID] = sector;
       }
     }

     // Fire pads in intersected sectors
     for(map<Int_t, CbmMuchSector*>::iterator it=firedSectors.begin(); it!=firedSectors.end(); it++){
       // Get sector and its parameters
       CbmMuchSector* sector = (*it).second;
       Int_t detectorId   = (*it).first;
       Int_t sectorNr     = sector->GetSectorNr();
       // Find fired pads
       for(Int_t iChannel = 0; iChannel < sector->GetNChannels(); iChannel++){
          Double_t padRad = sector->GetPadRadius();
          Double_t deltaX = xe - sector->GetPadX0(iChannel);
          Double_t deltaY = ye - sector->GetPadY0(iChannel);
          Double_t dist = TMath::Sqrt(deltaX*deltaX + deltaY*deltaY);
          if(dist > padRad + fSpotRadius) continue; // rough search

          TPolyLine padPolygon = sector->GetPadPolygon(iChannel);
          Double_t area;
          if(!PolygonsIntersect(sector, padPolygon, spotPolygon, area)) continue;  // detailed search
          UInt_t iCharge = (UInt_t) (nSecElectrons*area/spotArea);
          pair<Int_t, Int_t> channelId(detectorId, iChannel);
          if(chargedPads.find(channelId) == chargedPads.end()){
             chargedPads[channelId] = new CbmMuchDigi(stationNr, sectorNr, iChannel, time, fDTime);             
          }         
          chargedPads[channelId]->AddCharge(iCharge); // add charge to digi
       } // loop channels
     } // loop fired sectors
  } // loop primary electrons

  if(chargedPads.size() == 0){
     fNOutside++;
     return kFALSE;
  }

  for(map<pair<Int_t, Int_t>, CbmMuchDigi*>::iterator it = chargedPads.begin(); it != chargedPads.end(); it++){
     pair<Int_t, Int_t> channelId = (*it).first;
     CbmMuchDigi* digi = (*it).second;
     if(!digi) continue;
     Int_t iCharge = digi->GetCharge();
     if(iCharge < 0) iCharge = (Int_t)(TMath::Power(2, 31) - 2);
     if(fChargedPads.find(channelId) == fChargedPads.end()){
        fChargedPads[channelId] = new CbmMuchDigi(digi);
        fChargedMatches[channelId] = new CbmMuchDigiMatch(iPoint, iCharge);
     }
     else{
        fChargedPads[channelId]->AddTime(time);
        fChargedPads[channelId]->AddCharge(iCharge);
        fChargedMatches[channelId]->AddPoint(iPoint);
        fChargedMatches[channelId]->AddCharge(iCharge);
        fNMulti++;
     }

     // Clear memory
     delete digi;
  }

  chargedPads.clear();

  return kTRUE;
 //**************  Simulate avalanche (end) **********************************//
}

// -----   Public method Exec   --------------------------------------------
void CbmMuchDigitize::Exec(Option_t* opt) {
  // Reset all eventwise counters
  fTimer.Start();
  Reset();

  cout << endl;
  cout << "-I- " << fName << "   :   Event " << ++fEvent << endl;

  // Verbose screen output
  if ( fVerbose > 2 ) {
    cout << endl << "-I- " << fName << ": executing event" << endl;
    cout << "----------------------------------------------" << endl;
  }

  // Check for input arrays
 if( ! fMCTracks ) {
   cout << "-W- " << fName << "::Exec: No input array (MCTrack)" 
	 << endl;
   cout << "- " << fName << endl;
   return;
 }
  if ( ! fPoints ) {
    cerr << "-W- " << fName << "::Exec: No input array (MuchPoint) "
	 << endl;
    cout << "- " << fName << endl;
    return;
  }

  Int_t notUsable = 0; // DEBUG: counter for not usable points

  // Loop over all MuchPoints
  Int_t inc = 0;
  for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
    CbmMuchPoint* point = (CbmMuchPoint*) fPoints->At(iPoint);
    fNPoints++;

    // Get the station the point is in
    Int_t mcId = point->GetDetectorID();
    CbmMuchStation* station = fDigiScheme->GetStationByMcId(mcId);
    if ( ! station ) {
      if ( fVerbose > 2 ) cout << "MuchPoint " << iPoint 
			       << ", no digitization (MC volume "
			       << mcId << ")" << endl;
      //AZ - Check if this is a straw tube point
      if (point->GetDetectorID() < 200) {
	ExecStraws(station, point, iPoint, inc);
	continue;
      }
      //AZ
      fNFailed++;
      continue;
    }
    // Take only usable points
    if ( ! point->IsUsable() ) {
      notUsable++;
      continue;
    }

    // Produce Digis
    if(!fUseAvalanche){
      if(!ExecSimple(station, point, iPoint, inc)) continue;
    }
    else {
      if(!ExecAdvanced(station, point, iPoint)) continue; 
    }
  }  // MuchPoint loop
  if(fUseAvalanche) FirePads();

  // Screen output
  fTimer.Stop();


  if ( ! fVerbose ) cout << "+ ";
  else              cout << "-I- ";
  cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
       << fixed << right << fTimer.RealTime()
       << " s, points " << fNPoints << ", failed " << fNFailed
       << ", not usable " << notUsable
       << ", outside " << fNOutside << ", multihits " << fNMulti
       << ", digis " << fNDigis << endl;
}
// -------------------------------------------------------------------------

 
// -----   Private method SetParContainers   -------------------------------
void CbmMuchDigitize::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get MUCH geometry parameter container
  fGeoPar = (CbmGeoMuchPar*) db->getContainer("CbmGeoMuchPar");

  // Get MUCH digitisation parameter container
  fDigiPar = (CbmMuchDigiPar*) db->getContainer("CbmMuchDigiPar");
 
}
// -------------------------------------------------------------------------

// -----   Private method Finish   -----------------------------------------
void CbmMuchDigitize::Finish(){
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchDigitize::Init() {

  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");

  // Get input array of MuchPoints
  fPoints = (TClonesArray*) ioman->GetObject("MuchPoint");

  // Get input array of MC tracks
  fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");

  // Register output array MuchDigi
  fDigis = new TClonesArray("CbmMuchDigi",1000);
  ioman->Register("MuchDigi", "Digital response in MUCH", fDigis, kTRUE);

  // Register output array MuchDigiMatches
  fDigiMatches = new TClonesArray("CbmMuchDigiMatch",1000);
  ioman->Register("MuchDigiMatch", "Digi Match in MUCH", fDigiMatches, kTRUE);

  fEvent = 0;


  // Build digitization scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    fDigiScheme->InitGrid(); // Grid for fast search purpose
    if      (fVerbose >= 1) fDigiScheme->Print();
    cout << "-I- " << fName << "::Init: "
	 << "MUCH digitization scheme succesfully initialised" << endl;
    cout << "    Stations: " << fDigiScheme->GetNStations() 
	 << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
	 << fDigiScheme->GetNChannels() << endl;
    

    if(!fUseAvalanche)
      cout << "Using simple algorithm for MUCH digitization..." << endl;
    else
      cout << "Using advanced algorithm for MUCH digitization..." << endl;
    return kSUCCESS;
  }

  return kERROR;
}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchDigitize::ReInit() {
  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    fDigiScheme->InitGrid(); // Grid for fast search purpose
    if      (fVerbose >= 1) fDigiScheme->Print();
    cout << "-I- " << fName << "::Init: "
	 << "MUCH digitization scheme succesfully initialised" << endl;
    cout << "    Stations: " << fDigiScheme->GetNStations() 
	 << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
	 << fDigiScheme->GetNChannels() << endl;
    return kSUCCESS;
  }

  return kERROR;
}
// -------------------------------------------------------------------------

// -----   Private method Reset   ------------------------------------------
void CbmMuchDigitize::Reset() {
  fNPoints = fNFailed = fNOutside = fNMulti = fNDigis = 0;
  fChannelMap.clear();
  fChargedPads.clear();
  fChargedMatches.clear();
  if ( fDigis ) fDigis->Clear();
  if ( fDigiMatches ) fDigiMatches->Delete();//Clear(); // Delete because of memory escape
}
// -------------------------------------------------------------------------

// -----   Private method FirePads   ---------------------------------------
void CbmMuchDigitize::FirePads(){
   for(map<pair<Int_t, Int_t>, CbmMuchDigi*>::iterator it = fChargedPads.begin(); it!=fChargedPads.end(); it++){
      pair<Int_t, Int_t> channelId = (*it).first;
      CbmMuchDigi* digi = (*it).second;
      CbmMuchDigiMatch* match = fChargedMatches[channelId];
      if(digi->GetCharge() > fQThreshold){
         Int_t iDigi = -1;
         if(fChannelMap.find(channelId) == fChannelMap.end()){
            iDigi = fNDigis;
            new ((*fDigis)[fNDigis]) CbmMuchDigi(digi);
            new ((*fDigiMatches)[fNDigis]) CbmMuchDigiMatch(match);
            fChannelMap[channelId] = iDigi;
            fNDigis++;
         }
      }
      delete digi;
      delete match;
   }
}
// -------------------------------------------------------------------------

TPolyLine CbmMuchDigitize::GetPolygon(Double_t x0, Double_t y0, 
				      Double_t width, Double_t height, 
				      Double_t sinrot, Double_t cosrot){

  Double_t x[5], y[5];
  x[1]=-width/2; y[1]=+height/2;      x[2]=+width/2; y[2]=+height/2;
  x[0]=-width/2; y[0]=-height/2;      x[3]=+width/2; y[3]=-height/2;
  x[4]=x[0];  y[4]=y[0];
  for (Int_t i=0;i<5;i++) {
    Double_t xRot = x0+x[i]*cosrot-y[i]*sinrot;
    Double_t yRot = y0+y[i]*cosrot+x[i]*sinrot;
    x[i] = xRot;
    y[i] = yRot;
  }
  TPolyLine pline(5,x,y);
  return pline; 
}

TPolyLine CbmMuchDigitize::GetPolygon(Double_t x0, Double_t y0, 
				      Double_t width, Double_t height){

  Double_t x[5], y[5];
  x[1]=-width/2; y[1]=+height/2;      x[2]=+width/2; y[2]=+height/2;
  x[0]=-width/2; y[0]=-height/2;      x[3]=+width/2; y[3]=-height/2;
  x[4]=x[0];  y[4]=y[0];
  for(int i = 0; i<5; i++){
    x[i] += x0;
    y[i] += y0;
  }
  TPolyLine pline(5,x,y);
  return pline; 
}

Bool_t CbmMuchDigitize::ProjectionsIntersect(Double_t x11, Double_t x12, Double_t x21, Double_t x22, Double_t& length){
   if(x11 > x22 || x12 < x21) return kFALSE;
   if(x22 < x12){
      if(x21 > x11) length = x22 - x21;
      else length = x22 - x11;
   }
   else{
      if(x11 > x21) length = x12 - x11;
      else length = x12 - x21;
   }
   return kTRUE;
}

Bool_t CbmMuchDigitize::PolygonsIntersect(CbmMuchSector* sector, TPolyLine polygon1, TPolyLine polygon2, Double_t& area){
   Double_t sinRot = sector->GetSinRot();
   Double_t cosRot = sector->GetCosRot();
   Double_t length, width;
   Double_t* x1 = polygon1.GetX();
   Double_t* y1 = polygon1.GetY();
   Double_t* x2 = polygon2.GetX();
   Double_t* y2 = polygon2.GetY();

   Double_t x11 = x1[0]*cosRot + y1[0]*sinRot;
   Double_t x12 = x1[3]*cosRot + y1[3]*sinRot;
   Double_t x21 = x2[0]*cosRot + y2[0]*sinRot;
   Double_t x22 = x2[3]*cosRot + y2[3]*sinRot;

   Double_t y11 = y1[0]*cosRot - x1[0]*sinRot;
   Double_t y12 = y1[1]*cosRot - x1[1]*sinRot;
   Double_t y21 = y2[0]*cosRot - x2[0]*sinRot;
   Double_t y22 = y2[1]*cosRot - x2[1]*sinRot;

   if(!ProjectionsIntersect(x11, x12, x21, x22, width)) return kFALSE;
   if(!ProjectionsIntersect(y11, y12, y21, y22, length)) return kFALSE;
   area = width*length;
   return kTRUE;
}

Int_t CbmMuchDigitize::GasGain(){
//  const Double_t q_mean  = 1.e4;  // mean gas gain, arbitrary value
  Double_t gasGain = -fMeanGasGain*TMath::Log(1-fRnd->Rndm());
  if(gasGain < 0.) gasGain = 1e6;
  return (Int_t)gasGain;
}


Double_t CbmMuchDigitize::Sigma_n_e(Double_t Tkin, Double_t mass){
  Double_t logT;
  if(mass < 100){
     logT = log(Tkin*0.511/mass);
     return CbmMuchDigitize::e_sigma_n_e(logT);
  }
  else if (mass >= 100 && mass < 200){
     logT = log(Tkin*105.658/mass);
     return CbmMuchDigitize::mu_sigma_n_e(logT);
  }
  else {
     logT = log(Tkin*938.272/mass);
     return CbmMuchDigitize::p_sigma_n_e(logT);
  }
}

Double_t CbmMuchDigitize::MPV_n_e(Double_t Tkin, Double_t mass){
  Double_t logT;
  if(mass < 100.){
     logT = log(Tkin*0.511/mass);
     return CbmMuchDigitize::e_MPV_n_e(logT);
  }
  else if (mass >= 100. && mass < 200.){
     logT = log(Tkin*105.658/mass);
     return CbmMuchDigitize::mu_MPV_n_e(logT);
  }
  else {
     logT = log(Tkin*938.272/mass);
     return CbmMuchDigitize::p_MPV_n_e(logT);
  }
}


Double_t CbmMuchDigitize::mu_sigma_n_e(Double_t &logT){
    if (logT<-0.916291) logT=-0.916291; 
    if (logT>9.21034) logT=9.21034; 
    const int n = 7;
    double    val=0;
    double    arg=1;
    double    p[n]={74.5272
       ,   -49.7648
       ,   14.4886
       ,   -2.23059
       ,   0.188254
       ,   -0.00792744
       ,   0.00011976
    };
    for (int i=0; i<n; i++){
      val=val+arg*p[i];
      arg = arg*logT;
    }
    return val;
}

Double_t CbmMuchDigitize::p_sigma_n_e(Double_t &logT){
    if (logT<1.09861) logT=1.09861; 
    if (logT>9.21034) logT=9.21034; 
    const int n = 7;
    double    val=0;
    double    arg=1;
    double    p[n]={175.879
       ,   -15.016
       ,   -34.6513
       ,   13.346
       ,   -2.08732
       ,   0.153678
       ,   -0.00440115
    };
    for (int i=0; i<n; i++){
      val=val+arg*p[i];
      arg = arg*logT;
    }
    return val;
}

Double_t CbmMuchDigitize::e_sigma_n_e(Double_t &logT){
    if (logT<-3.21888) logT=-3.21888; 
    if (logT>9.21034) logT=9.21034; 
    const int n = 7;
    double    val=0;
    double    arg=1;
    double    p[n]={4.06815
       ,   -0.225699
       ,   0.464502
       ,   -0.141208
       ,   0.0226821
       ,   -0.00195697
       ,   6.87497e-05
    };
    for (int i=0; i<n; i++){
      val=val+arg*p[i];
      arg = arg*logT;
    }
    return val;
}

Double_t CbmMuchDigitize::mu_MPV_n_e(Double_t &logT){
    if (logT<-0.916291) logT=-0.916291; 
    if (logT>9.21034) logT=9.21034; 
    const int n = 7;
    double    val=0;
    double    arg=1;
    double    p[n]={660.746
       ,   -609.335
       ,   249.011
       ,   -55.6658
       ,   7.04607
       ,   -0.472135
       ,   0.0129834
    };
    for (int i=0; i<n; i++){
      val=val+arg*p[i];
      arg = arg*logT;
    }
    return val;
}

Double_t CbmMuchDigitize::p_MPV_n_e(Double_t &logT){
    if (logT<1.09861) logT=1.09861; 
    if (logT>9.21034) logT=9.21034; 
    const int n = 7;
    double    val=0;
    double    arg=1;
    double    p[n]={4152.73
       ,   -3123.98
       ,   1010.85
       ,   -178.092
       ,   17.8764
       ,   -0.963169
       ,   0.0216643
    };
    for (int i=0; i<n; i++){
      val=val+arg*p[i];
      arg = arg*logT;
    }
    return val;
}

Double_t CbmMuchDigitize::e_MPV_n_e(Double_t &logT){
    if (logT<-3.21888) logT=-3.21888; 
    if (logT>9.21034) logT=9.21034; 
    const int n = 7;
    double    val=0;
    double    arg=1;
    double    p[n]={14.654
       ,   -0.786582
       ,   2.32435
       ,   -0.875594
       ,   0.167237
       ,   -0.0162335
       ,   0.000616855
    };
    for (int i=0; i<n; i++){
      val=val+arg*p[i];
      arg = arg*logT;
    }
    return val;
}
void CbmMuchDigitize::SetDetectorType(DetectorType type){
   switch(type){
      case kGEM:
         fSpotRadius = 0.15;
         break;
      case kMICROMEGAS:
         fSpotRadius = 0.03;
         break;
   }
}

//AZ ------- Private method ExecStraws ---------------------------------------
Bool_t CbmMuchDigitize::ExecStraws(CbmMuchStation* station, CbmMuchPoint* point,
				   Int_t iPoint, Int_t& inc)
{
  // Digitize straw tube MC point

  //return kTRUE;
  Int_t detID = point->GetDetectorID();
  Int_t layer = detID & 1;
  Int_t stationNr = detID >> 2;
  //new ((*fDigis)[fNDigis]) CbmMuchDigi(stationNr, sectorNr, iChannel, time, fDTime);
  CbmMuchDigi *digi = new ((*fDigis)[fNDigis]) CbmMuchDigi(stationNr, layer, 0, -1, 0);
  new ((*fDigiMatches)[fNDigis++]) CbmMuchDigiMatch(iPoint);
  //cout << detID << " " << stationNr << " " << layer << " " << digi->GetDetectorId() << endl;
  Double_t coord[3];
  coord[0] = (point->GetXIn() + point->GetXOut()) / 2.;
  coord[1] = (point->GetYIn() + point->GetYOut()) / 2.;
  coord[2] = (point->GetZIn() + point->GetZOut()) / 2.;
  // Pass abs values of coordinates and their sign in some stupid manner
  Int_t signs = 0;
  for (Int_t i = 0; i < 3; ++i) {
    digi->AddTime(TMath::Abs(coord[i]));
    if (coord[i] < 0) signs |= (1 << i);
  }
  digi->SetUniqueID(signs);
  //cout << " Straw digi: " << coord[0] << " " << coord[1] << " " << coord[2] << " " << signs << endl;
  return kTRUE;
}

ClassImp(CbmMuchDigitize)
ClassImp(Point)

