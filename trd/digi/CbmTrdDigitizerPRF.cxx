#include "CbmTrdDigitizerPRF.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdGeoHandler.h"

#include "CbmMCTrack.h"

#include "CbmMatch.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TF1.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TImage.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TStopwatch.h"

#include "TPRegexp.h"

//#include "sqlite/sqlite3.h" // used for the lookup table option

#include <iostream>
#include <cmath>
#include <iomanip>
using std::cout;
using std::endl;
using std::pair;
using std::make_pair;
using std::map;
using std::vector;
using std::max;
using std::fabs;
// ---- Default constructor -------------------------------------------
CbmTrdDigitizerPRF::CbmTrdDigitizerPRF()
  : FairTask("TrdCluster"),
    fDebug(false),
    fTime(-1.),
    fModuleType(-1),
    fModuleCopy(-1),
    fModuleAddress(-1),
    fMCPointId(-1),
    fnRow(-1),
    fnCol(-1),
    fnSec(-1),
    fModuleId(-1),
    fLayerId(-1),
    fTrdPoints(NULL),
    fDigis(new TClonesArray("CbmTrdDigi")),
    fDigiMatchs(NULL),
    fMCStacks(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiators(NULL),
    fDigiMap(),
    fGeoHandler(new CbmTrdGeoHandler())
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdDigitizerPRF::CbmTrdDigitizerPRF(const char *name, const char *title,
					     CbmTrdRadiator *radiator)
  :FairTask(name),
   fTime(-1.),
   fDebug(false),
   fModuleType(-1),
   fModuleCopy(-1),
   fModuleAddress(-1),
   fMCPointId(-1),
    fnRow(-1),
    fnCol(-1),
    fnSec(-1),
    fModuleId(-1),
    fLayerId(-1),
   fTrdPoints(NULL),
   fDigis(NULL),
   fDigiMatchs(NULL),
   fMCStacks(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fRadiators(radiator),
   fDigiMap(),
   fGeoHandler(new CbmTrdGeoHandler())
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdDigitizerPRF::~CbmTrdDigitizerPRF()
{
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();
  //fDigis->Clear("C");
  fDigis->Delete();
  delete fDigis;
  //fDigiMatchs->Clear("C");
  fDigiMatchs->Delete();
  delete fDigiMatchs;
  fTrdPoints->Clear("C");
  fTrdPoints->Delete();
  delete fTrdPoints;
  fMCStacks->Clear("C");
  fMCStacks->Delete();
  delete fMCStacks;
  if(fDigiPar)
    delete fDigiPar;
  if(fModuleInfo)
    delete fModuleInfo;
  //delete fdigi;
  //delete fdigiMatch;
}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdDigitizerPRF::SetParContainers()
{
    cout<<" * CbmTrdDigitizerPRF * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdDigitizerPRF::ReInit(){

  cout<<" * CbmTrdClusterizer * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdDigitizerPRF::Init()
{

  cout<<" * CbmTrdDigitizerPRF * :: Init() "<<endl;

  FairRootManager *ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
  fTrdPoints=(TClonesArray *)ioman->GetObject("TrdPoint"); 
  if ( ! fTrdPoints ) {
    cout << "-W CbmTrdClusterFast::Init: No TrdPoints array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }

  fMCStacks = (TClonesArray*)ioman->GetObject("MCTrack");
  if ( ! fMCStacks ) {
    cout << "-W CbmTrdClusterFast::Init: No MCTrack array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }

  fDigis = new TClonesArray("CbmTrdDigi", 100);
  ioman->Register("TrdDigi","TRD Digis",fDigis,kTRUE);

  fDigiMatchs = new TClonesArray("CbmMatch", 100);
  ioman->Register("TrdDigiMatch","TRD Digis",fDigiMatchs,kTRUE);

  fGeoHandler->Init();

  fRadiators->Init();

  return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdDigitizerPRF::Exec(Option_t * option)
{
  fDigis->Clear();
  fDigiMatchs->Clear();

  fDebug = false;
  TStopwatch timer;
  timer.Start();
  cout << "================CbmTrdDigitizerPRF=====================" << endl;
 
  CbmTrdPoint *point = NULL;
  CbmMCTrack *track = NULL;

  TVector3 mom;
  Double_t ELoss;
  Double_t ELossTR;     // TR energy loss for e- & e+
  Double_t ELossdEdX;   // ionization energy loss
  Double_t time;
  Int_t nCount = 0;
  Int_t nEntries = fTrdPoints->GetEntries();
  Int_t iLatticeHits(0), iElectrons(0), iBackwardTrack(0);
  if (fDebug) 
    nEntries = 1;
  cout << " Found " << nEntries << " MC-Points in Buffer of TRD" << endl;
  for (Int_t j = 0; j < nEntries ; j++ ){
    fMCPointId = j;
    ELoss = 0.0;
    ELossTR = 0.0;
    ELossdEdX = 0.0;
    //cout << " " << j << endl;
    point = (CbmTrdPoint*) fTrdPoints->At(j);
    if(NULL == point) {
      cout << " no point found " << endl;
    }
    if(NULL == point) continue;

    point->Momentum(mom);
    track = (CbmMCTrack*) fMCStacks->At(point->GetTrackID());
    if(NULL == track) {
      cout << " no point found " << endl;
    }

    if(NULL == track) continue;


    Double_t point_in[3];
    Double_t point_out[3];

    point_in[0] = point->GetXIn();
    point_in[1] = point->GetYIn();
    point_in[2] = point->GetZIn();

    point_out[0] = point->GetXOut();
    point_out[1] = point->GetYOut();
    point_out[2] = point->GetZOut();


    Double_t direction[3] = {point_out[0] - point_in[0],
			     point_out[1] - point_in[1],
			     point_out[2] - point_in[2]};

    if (direction[2] < 0){
      LOG(DEBUG2) << "CbmTrdDigitizerPRF::Exec: MC-track points towards target!" << FairLogger::endl;
      iBackwardTrack++;
      //TString ori[3] = {"x","y","z"};
      //for(Int_t i = 0; i < 3; i++)
      //printf("            %s:  in:%8.2f  out:%8.2f direction:%8.2f\n",ori[i].Data(),point_in[i],point_out[i],direction[i]);      
    }


    // Add TR energy loss for electrons ans positrons
    Int_t pdgCode = track->GetPdgCode();
  
    if(TMath::Abs(pdgCode) == 11){ 
      iElectrons++;
      if (fRadiators->LatticeHit(point)){  // electron has passed lattice grid (or frame material) befor reaching the gas volume -> TR-photons have been absorbed by the lattice grid
	iLatticeHits++;
	ELossTR = 0.0;
      } else if (direction[2] < 0){ //electron has not passed the radiator
	ELossTR = 0.0;
      } else {
	ELossTR = fRadiators->GetTR(mom);
      }
    }
    ELossdEdX = point->GetEnergyLoss();
    ELoss = ELossTR + ELossdEdX;

    fTime = point->GetTime();



    // Find node corresponding to the point in the center between entrance and exit MC-point coordinates
    gGeoManager->FindNode((point_out[0] + point_in[0]) / 2, 
			  (point_out[1] + point_in[1]) / 2, 
			  (point_out[2] + point_in[2]) / 2
			  );

    if (!TString(gGeoManager->GetPath()).Contains("gas")){
      LOG(ERROR) << "CbmTrdDigitizerPRF::Exec: MC-track not in TRD! Node:" << TString(gGeoManager->GetPath()).Data() << " gGeoManager->MasterToLocal() failed!" << FairLogger::endl;
      continue;
    }
    //const Int_t nCluster = 1;//TODO: as function of track length in active volume
    //cout << TString(gGeoManager->GetPath()).Data() << endl;
    //cout << "GetModuleAddress " << fGeoHandler->GetModuleAddress(TString(gGeoManager->GetPath())) << endl;
    //    fLayerId = CbmTrdAddress::GetLayerId(fGeoHandler->GetModuleAddress(TString(gGeoManager->GetPath())));
    //    cout << "GetLayerId   " << fLayerId << endl;
    //    fModuleId = CbmTrdAddress::GetModuleId(fGeoHandler->GetModuleAddress(TString(gGeoManager->GetPath())));
    //    cout << "GetModuleId  " << fModuleId << endl;
    //    fModuleId = point->GetDetectorID();
    //    cout << "ModuleAddress " << fModuleId << endl;

    fLayerId = CbmTrdAddress::GetLayerId(point->GetDetectorID());
    //cout << "GetLayerId   " << fLayerId << endl;
    fModuleId = CbmTrdAddress::GetModuleId(point->GetDetectorID());
    fModuleAddress = CbmTrdAddress::GetModuleAddress(point->GetDetectorID());
    //cout << "GetModuleId2 " << fModuleId << endl;

    const Double_t *global_point = gGeoManager->GetCurrentPoint();
    Double_t local_point[3];

    gGeoManager->MasterToLocal(global_point, local_point);


    //    fModuleId = CbmTrdAddress::GetModuleId(point->GetDetectorID());
    //    fLayerId  = CbmTrdAddress::GetLayerId(point->GetDetectorID());

    fModuleInfo = fDigiPar->GetModule(point->GetDetectorID());
    fnCol = fModuleInfo->GetNofColumns();
    fnRow = fModuleInfo->GetNofRows();
    // Form address which contains layerId, moduleId, sectorId, columnId and rowId.
    // Address in the MC point contains only information about layerId and moduleId.
    //fLayerId = CbmTrdAddress::GetLayerId(gGeoManager->GetModuleAddress(TString(gGeoManager->GetPath())));//gGeoManager->GetLayer(fModuleId);//CbmTrdAddress::GetLayerId(fModuleInfo->GetModuleAddress());   

    SplitTrackPath(point, ELoss);

  }
  // Fill data from internally used stl map into output TClonesArray
  Int_t iDigi = 0;
  map<Int_t, pair<CbmTrdDigi*, CbmMatch*> >::iterator it;
  for (it = fDigiMap.begin() ; it != fDigiMap.end(); it++) {
    CbmTrdDigi* digi = it->second.first;
    new ((*fDigis)[iDigi]) CbmTrdDigi(*digi);
    CbmMatch* digiMatch = it->second.second;
    new ((*fDigiMatchs)[iDigi]) CbmMatch(*digiMatch);
    delete digi;
    delete digiMatch;
    iDigi++;
  }
  fDigiMap.clear();
  printf("\n   %i tracks pointing towards target (%5.1f%%)\n",iBackwardTrack,100.*iBackwardTrack/nEntries);
  printf("\n   %i electron tracks through lattice material of %i electrons (%5.1f%%)\n",iLatticeHits,iElectrons,100*iLatticeHits/Double_t(iElectrons));
  printf("\n   Created %i TrdDigis  %7.3f Digis/MC-Point\n",iDigi, Double_t(iDigi/nEntries));
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");

  //fIntegralTest->Draw();
}
// --------------------------------------------------------------------
Double_t CbmTrdDigitizerPRF::CalcPRF(Double_t x, Double_t W, Double_t h)
{
  Float_t K3 = 0.525; 
  Double_t SqrtK3 = sqrt(K3);

  return fabs(
	      -1. / (2. * atan(SqrtK3)) * (
					   atan(SqrtK3 * tanh(TMath::Pi() * (-2. + SqrtK3 ) * (W + 2.* x) / (8.* h) )) + 
					   atan(SqrtK3 * tanh(TMath::Pi() * (-2. + SqrtK3 ) * (W - 2.* x) / (8.* h) ))
					   )
	      );
}
// --------------------------------------------------------------------
void CbmTrdDigitizerPRF::ScanPadPlane(const Double_t* local_point, Double_t clusterELoss)
{
  Int_t sectorId(-1), columnId(-1), rowId(-1);//, preSecRows(0);
  fModuleInfo->GetPadInfo( local_point, sectorId, columnId, rowId);
  if (sectorId < 0 && columnId < 0 && rowId < 0) {
    printf("    x:%7.3f  y:%7.3f z:%7.3f\n",local_point[0],local_point[1],local_point[2]);
    return;
  } else {
    for (Int_t i = 0; i < sectorId; i++)
      rowId += fModuleInfo->GetNofRowsInSector(i); // local -> global row

    Double_t displacement_x(0), displacement_y(0);//mm
    Double_t h = fModuleInfo->GetAnodeWireToPadPlaneDistance();
    Double_t W(fModuleInfo->GetPadSizeX(sectorId)), H(fModuleInfo->GetPadSizeY(sectorId));
    fModuleInfo->TransformToLocalPad(local_point, displacement_x, displacement_y);

    const Int_t maxCol(7), maxRow(3);

    //Estimate starting column and row and limits due to chamber dimensions
    Int_t startCol(columnId-maxCol/2), stopCol(columnId+maxCol/2), startRow(rowId-maxRow/2), stopRow(rowId+maxRow/2), startSec(0);
    Double_t sum = 0;
    Int_t secRow(-1), targCol(-1), targRow(-1), targSec(-1), address(-1);
    if (fDebug) printf("\nhit: (%7.3f,%7.3f)\n",displacement_x,displacement_y);
    for (Int_t iRow = startRow; iRow <= rowId+maxRow/2; iRow++){
      if (fDebug) printf("(%i): ",iRow);
      for (Int_t iCol = startCol; iCol <= columnId+maxCol/2; iCol++){
	if (((iCol >= 0) && (iCol <= fnCol-1)) && ((iRow >= 0) && (iRow <= fnRow-1))){// real adress	 
	  targSec = fModuleInfo->GetSector(iRow, secRow);
	  //printf("secId digi1 %i\n",targSec);
	  address = CbmTrdAddress::GetAddress(fLayerId, fModuleId, targSec, secRow, iCol); 
	  if (secRow > 11 && fModuleId == 5)
	    printf("address %i layer %i and modId %i modAddress %i  Sec%i Row:%i Col%i\n",address,fLayerId,fModuleId,fModuleAddress,targSec,secRow,iCol);
	} else {
          targRow = iRow;
          targCol = iCol;
          if (iCol < 0)
	    targCol = 0;
	  else if (iCol > fnCol-1)
	    targCol = fnCol-1;
	  if (iRow < 0)
	    targRow = 0;
	  else if (iRow > fnRow-1)	    
	    targRow = fnRow-1;

	  targSec = fModuleInfo->GetSector(targRow, secRow);
	  //printf("secId digi2 %i\n",targSec);
	  address = CbmTrdAddress::GetAddress(fLayerId, fModuleId, targSec, secRow, targCol);
	  if (secRow > 11 && fModuleId == 5)
	    printf("address %i layer %i and modId %i modAddress %i  Sec%i Row:%i Col%i\n",address,fLayerId,fModuleId,fModuleAddress,targSec,secRow,targCol);
	}
	Double_t chargeFraction = 0;
	if (rowId == iRow && columnId == iCol) // if pad in center of 7x3 arrayxs
	  chargeFraction = CalcPRF(displacement_x, W, h) * CalcPRF(displacement_y, H, h);
	else
	  chargeFraction = CalcPRF((iCol - columnId) * W - displacement_x, W, h) * CalcPRF((iRow - rowId) * H - displacement_y, H, h);

	sum += chargeFraction;
	//if (iCol >= stopCol)

	AddDigi(fMCPointId, address, Double_t(chargeFraction * clusterELoss), fTime);
	
	if (fDebug) {
	  if (rowId == iRow && columnId == iCol)
	    printf("(%3i:%5.3E) ",iCol,chargeFraction);
	  else
	    printf(" %3i:%5.3E  ",iCol,chargeFraction);
	  if (iCol == fnCol-1)
	    cout << "|";
	}
	
      } // for iCol
      
      if (fDebug) {
	if (iRow == fnRow-1)
	  cout << endl << "-------------------------------------" << endl;
	else
	  cout << endl;
      }
      
    } // for iRow
    if (sum < 0.99 || sum > 1.01){
      LOG(WARNING) << "CbmTrdDigitizerPRF::ScanPadPlane: Summed charge: " << std::setprecision(5) << sum << "  hit:(" << columnId << ", " << rowId <<")   max:(" << fnCol-1 << ", " << fnRow-1 << ")" << FairLogger::endl;
    }
  }  
}
// --------------------------------------------------------------------
void CbmTrdDigitizerPRF::SplitTrackPath(const CbmTrdPoint* point, Double_t ELoss)
{
  const Double_t nClusterPerCm = 1.0;
  Double_t point_in[3] = {
    point->GetXIn(),
    point->GetYIn(),
    point->GetZIn()
  };
  Double_t point_out[3] = {
    point->GetXOut(),
    point->GetYOut(),
    point->GetZOut()
  };
  Double_t local_point_out[3];// exit point coordinates in local module cs
  Double_t local_point_in[3]; // entrace point coordinates in local module cs
  gGeoManager->MasterToLocal(point_in,  local_point_in);
  gGeoManager->MasterToLocal(point_out, local_point_out);

  Double_t cluster_pos[3];   // cluster position in local module coordinate system
  Double_t cluster_delta[3]; // vector pointing in MC-track direction with length of one path slice within chamber volume to creat n cluster

  Double_t trackLength = 0;

  for (Int_t i = 0; i < 3; i++){
    cluster_delta[i] = (local_point_out[i] - local_point_in[i]);
    trackLength += cluster_delta[i] * cluster_delta[i];
  }
  trackLength = TMath::Sqrt(trackLength);
  const Int_t nCluster = trackLength / nClusterPerCm + 0.9;// Track length threshold of minimum 0.1cm track length in gas volume
  if (nCluster < 1){
    LOG(WARNING) << "CbmTrdDigitizerPRF::SplitTrackPath: nCluster: "<<nCluster<<"   track length: "<<std::setprecision(5)<<trackLength<<"cm  nCluster/cm: "<<std::setprecision(2)<<nClusterPerCm<<"  ELoss: "<<std::setprecision(5)<<ELoss*1e-6<<"keV " << FairLogger::endl;
    return;
  }
  for (Int_t i = 0; i < 3; i++){
    cluster_delta[i] /= Double_t(nCluster);
  }
  Double_t clusterELoss = ELoss / Double_t(nCluster);
 
  for (Int_t iCluster = 0; iCluster < nCluster; iCluster++){
    for (Int_t i = 0; i < 3; i++){
      cluster_pos[i] = local_point_in[i] + (0.5 + iCluster) * cluster_delta[i];// move start position 0.5 step widths into track direction    
    }

    if ( fModuleInfo->GetSizeX() < fabs(cluster_pos[0]) || fModuleInfo->GetSizeY() < fabs(cluster_pos[1])){
      printf("->    nC %i/%i x: %7.3f y: %7.3f \n",iCluster,nCluster-1,cluster_pos[0],cluster_pos[1]);
      for (Int_t i = 0; i < 3; i++)
	printf("  (%i) | in: %7.3f + delta: %7.3f * cluster: %i/%i = cluster_pos: %7.3f out: %7.3f g_in:%f g_out:%f\n",
	       i,local_point_in[i],cluster_delta[i],iCluster,(Int_t)nCluster,cluster_pos[i],local_point_out[i],point_in[i],point_out[i]);
    }

    fModuleInfo->ProjectPositionToNextAnodeWire(cluster_pos);

    ScanPadPlane(cluster_pos, clusterELoss);
  }
}
// --------------------------------------------------------------------
void CbmTrdDigitizerPRF::AddDigi(Int_t pointId, Int_t address, Double_t charge, Double_t time)
{
  const FairMCPoint* point = static_cast<const FairMCPoint*>(fTrdPoints->At(pointId));
  map<Int_t, pair<CbmTrdDigi*, CbmMatch*> >::iterator it = fDigiMap.find(address);
  if (it == fDigiMap.end()) { // Pixel not yet in map -> Add new pixel
     CbmMatch* digiMatch = new CbmMatch();
     digiMatch->AddReference(pointId, charge);
     fDigiMap[address] = make_pair(new CbmTrdDigi(address, charge, time), digiMatch);
  } else { // Pixel already in map -> Add charge
     CbmTrdDigi* digi = it->second.first;
     digi->AddCharge(charge);
     digi->SetTime(max(time, digi->GetTime()));
     CbmMatch* digiMatch = it->second.second;
     digiMatch->AddReference(pointId, charge);
  }
}

ClassImp(CbmTrdDigitizerPRF)
