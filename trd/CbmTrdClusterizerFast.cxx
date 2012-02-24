#include "CbmTrdClusterizerFast.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdDigiMatch.h"
#include "CbmTrdModule.h"
#include "CbmTrdRadiator.h"

#include "CbmMCTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

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
#if ROOT_VERSION_CODE < ROOT_VERSION(5,20,0)
#include "CbmTrdStringToken.h"
#endif

//#include "sqlite/sqlite3.h" // used for the lookup table option

#include <iostream>
#include <cmath>
using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::fabs;
// ---- Default constructor -------------------------------------------
CbmTrdClusterizerFast::CbmTrdClusterizerFast()
  : FairTask("TrdCluster"),
    Digicounter(-1),
    fLayerZ(),
    fTime(-1.),
    fModuleType(-1),
    fModuleCopy(-1),
    fModuleID(-1),
    fMCindex(-1),
    fEfficiency(1.),
    fTrdPoints(NULL),
    fDigiCollection(new TClonesArray("CbmTrdDigi")),
    fDigiMatchCollection(NULL),
    fMCStacks(NULL),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fRadiators(NULL),
    fTrdId(),
    //fDigiMap(),
    //fDigiMapIt(),
    fModuleClusterMap(),
    fModuleClusterMapIt(),
    fdigi(NULL),
    fdigiMatch(NULL),
    fDebug(false)
{
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdClusterizerFast::CbmTrdClusterizerFast(const char *name, const char *title,
					     CbmTrdRadiator *radiator)
  :FairTask(name),
   Digicounter(-1),
   fLayerZ(),
   fTime(-1.),
   fModuleType(-1),
   fModuleCopy(-1),
   fModuleID(-1),
   fMCindex(-1),
   fEfficiency(1.),
   fTrdPoints(NULL),
   fDigiCollection(NULL),
   fDigiMatchCollection(NULL),
   fMCStacks(NULL),
   fDigiPar(NULL),
   fModuleInfo(NULL),
   fRadiators(radiator),
   fTrdId(),
   //fDigiMap(),
   //fDigiMapIt(),
   fModuleClusterMap(),
   fModuleClusterMapIt(),
   fdigi(NULL),
   fdigiMatch(NULL),
   fDebug(false)
{
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdClusterizerFast::~CbmTrdClusterizerFast()
{
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();
  fDigiCollection->Clear("C");
  fDigiCollection->Delete();
  delete fDigiCollection;
  fDigiMatchCollection->Clear("C");
  fDigiMatchCollection->Delete();
  delete fDigiMatchCollection;
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
  delete fdigi;
  delete fdigiMatch;
}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdClusterizerFast::SetParContainers()
{
    cout<<" * CbmTrdClusterizerFast * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdClusterizerFast::ReInit(){

  cout<<" * CbmTrdClusterizer * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdClusterizerFast::Init()
{

  cout<<" * CbmTrdClusterizerFast * :: Init() "<<endl;

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

  fDigiCollection = new TClonesArray("CbmTrdDigi", 100);
  ioman->Register("TrdDigi","TRD Digis",fDigiCollection,kTRUE);

  fDigiMatchCollection = new TClonesArray("CbmTrdDigiMatch", 100);
  ioman->Register("TrdDigiMatch","TRD Digis",fDigiMatchCollection,kTRUE);

  fRadiators->Init();

  return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdClusterizerFast::Exec(Option_t * option)
{
  TStopwatch timer;
  timer.Start();
  cout << "================CbmTrdClusterizerFast=====================" << endl;
  Digicounter = 0;
  CbmTrdPoint *pt = NULL;
  fDebug = false;//true;
  /*
  // Module infos are stored to ModuleClusterMap whenever a module is used for the first time in each event
  // Digis are buffered in the ModuleClusterMap until the end of event and than copied to die digicollection
  // Digis are only calculated for three central pads in x and y (start up version)
  // Afterwards can be extended to n-pad (worse, easier to implement) cluster for x and y or a maximum distance to cluster center (better, more flexible)
  // Dont forget to erase the ModuleClusterMap after each event!!!
  // First step: digis are estmated using prf for the position coresponding to center between entrance and exit point
  // Second step: path between entrance and exit is splitted into slithes
  */
  TVector3 mom;
  Double_t ELoss;
  Double_t ELossTR;     // TR energy loss for e- & e+
  Double_t ELossdEdX;   // ionization energy loss
  Int_t nCount = 0;
  Int_t nEntries = fTrdPoints->GetEntries();
  cout << " Found " << nEntries << " MC-Points in Collection of TRD" << endl;
  for (int j = 0; j < nEntries ; j++ ){
    //cout << " " << j << endl;
    pt = (CbmTrdPoint*) fTrdPoints->At(j);
    if(NULL == pt) {
      cout << " no point found " << endl;
    }
    if(NULL == pt) continue;

    pt->Momentum(mom);
    fMCindex=pt->GetTrackID();
    CbmMCTrack *p = (CbmMCTrack*) fMCStacks->At(fMCindex);
    if(NULL == p) {
      cout << " no point found " << endl;
    }

    if(NULL == p) continue;

    Int_t pdgCode = p->GetPdgCode();
    ELoss = pt->GetEnergyLoss();
    if(TMath::Abs(pdgCode) == 11){      
      ELoss +=  fRadiators->GetTR(mom);
    }
    // Calculate point in the middle of the detector. This is
    // for safety reasons, because the point at exit is slightly
    // outside of the active volume. If one does not use a point
    // in the detector one will get a wrong volume from the
    // geomanager. Navigate to the correct volume to get all
    // necessary information about this volume
    // --------------->[cm]<---------------
    Double_t x_in   = pt->GetXIn();
    Double_t x_out  = pt->GetXOut();
    Double_t y_in   = pt->GetYIn();
    Double_t y_out  = pt->GetYOut();
    Double_t z_in   = pt->GetZIn();
    Double_t z_out  = pt->GetZOut();
    Double_t x_mean = (x_in + x_out)/2.;
    Double_t y_mean = (y_in + y_out)/2.;
    Double_t z_mean = (z_in + z_out)/2.;
    // --------------->[mm]<---------------
 
    gGeoManager->FindNode(x_mean, y_mean, z_mean);

    // Get the local point in local MC coordinates from
    // the geomanager. This coordinate system is rotated
    // if the chamber is rotated. This is corrected in 
    // GetModuleInformationFromDigiPar(..) to have a
    // the same local coordinate system in all the chambers
    const Double_t *global_point = gGeoManager->GetCurrentPoint();
    Double_t local_point[3];
    Double_t local_meanLL[3];
    Double_t local_meanC[3];
    Double_t global_meanLL[3];//[cm]
    Double_t global_meanC[3];
    Double_t local_inLL[3];
    Double_t local_inC[3];
    Double_t global_inLL[3];//[cm]
    Double_t global_inC[3];
    Double_t local_outLL[3];
    Double_t local_outC[3];
    Double_t global_outLL[3];//[cm]
    Double_t global_outC[3];

    global_meanC[0]= x_mean;//[cm]      
    global_inC[0]  = x_in  ;//[cm]      
    global_outC[0] = x_out ;//[cm]

    global_meanC[1]= y_mean;//[cm]      
    global_inC[1]  = y_in  ;//[cm]      
    global_outC[1] = y_out ;//[cm]

    global_meanC[2]= z_mean;//[cm]      
    global_inC[2]  = z_in  ;//[cm]      
    global_outC[2] = z_out ;//[cm]

    gGeoManager->MasterToLocal(global_point, local_point);
    gGeoManager->MasterToLocal(global_meanC, local_meanC);
    gGeoManager->MasterToLocal(global_inC, local_inC);
    gGeoManager->MasterToLocal(global_outC, local_outC);

    for ( Int_t idim = 0; idim < 3; idim++)
      {
	local_meanC[idim]  *= 10; //[cm]->[mm]
	local_inC[idim]    *= 10; //[cm]->[mm]
	local_outC[idim]   *= 10; //[cm]->[mm]

	global_meanC[idim] *= 10; //[cm]->[mm]
	global_inC[idim]   *= 10; //[cm]->[mm]
	global_outC[idim]  *= 10; //[cm]->[mm]
      }

    fModuleID = pt->GetDetectorID();
    GetModuleInformationFromDigiPar(fModuleID);


    TransformC2LL(local_meanC, local_meanLL);
    TransformC2LL(local_inC,   local_inLL);
    TransformC2LL(local_outC,  local_outLL);

    Double_t padDisplacementLL[2] = {0, 0};
    Int_t PadMax[2] = {-1, -1};
    GetClusterDisplacement(local_meanLL, padDisplacementLL, PadMax);
    if(fDebug)
      printf ("(%6.2f,%6.2f)  (%6.2f,%6.2f)  (%3i,%3i)\n",local_meanLL[0],local_meanLL[1],padDisplacementLL[0],padDisplacementLL[1],PadMax[0],PadMax[1]);

    CalcDigisOnPadPlane(padDisplacementLL, PadMax, ELoss, j);

  }
  
  Int_t iDigi = 0;
  for (fModuleClusterMapIt = fModuleClusterMap.begin(); fModuleClusterMapIt != fModuleClusterMap.end(); fModuleClusterMapIt++) {
    for (Int_t xPad = 0; xPad < (*fModuleClusterMapIt).second->PadPlane.size(); xPad++) {
      for (Int_t yPad = 0; yPad < (*fModuleClusterMapIt).second->PadPlane[xPad].size(); yPad++) {
	if ((*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->Charge > 0.0) {
	  fdigi = new CbmTrdDigi(
				(*fModuleClusterMapIt).first, 
				xPad, 
				yPad, 
				(*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->Charge, 
				fTime, 
				(*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex[0]//iDigi // ?????????????????
				);
	  for (Int_t i = 1; i < (*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex.size(); i++)
	    fdigi->AddMCIndex((*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex[i]);

	  new ((*fDigiCollection)[iDigi]) CbmTrdDigi();
	  (*fDigiCollection)[iDigi] = fdigi;

	  fdigiMatch = new ((*fDigiMatchCollection)[iDigi]) CbmTrdDigiMatch(); 

	  for (Int_t i = 0; i < (*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex.size(); i++)
	    Int_t bla = fdigiMatch->AddPoint((*fModuleClusterMapIt).second->PadPlane[xPad][yPad]->MCIndex[i]);
	  iDigi++;

	}
      }
    }
  }
  //delete digi;
  //delete p;
  printf(" Added %d Digis to Collection of TRD\n\n",iDigi);
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");

  //fIntegralTest->Draw();
}
  // --------------------------------------------------------------------
void CbmTrdClusterizerFast::CalcDigisOnPadPlane(Double_t* clusterPosInPadLL, Int_t* PadMax, Double_t ELoss, Int_t pointId)
{
  if (PadMax[0] >= (Int_t)fModuleClusterMap[fModuleID]->PadPlane.size() || PadMax[1] >= (Int_t)fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]].size()) {
    printf("\n\n ERROR: CalcDigisOnPadPlane: PadMax out of module.\n Module size: (%3i,%3i)  PadMax(%3i,%3i)\n\n",(Int_t)fModuleClusterMap[fModuleID]->PadPlane.size(), (Int_t)fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]].size(),PadMax[0],PadMax[1]);
  }
  else
    {
      const Int_t nPadCluster = 3; // has to be odd
      Int_t xStart = PadMax[0] - nPadCluster / 2;
      Int_t xStop  = PadMax[0] + nPadCluster / 2 + 1;
      Int_t yStart = PadMax[1] - nPadCluster / 2;
      Int_t yStop  = PadMax[1] + nPadCluster / 2 + 1;
      if(fDebug)
	printf("PadPlane(%3i,%3i)  PadMax(%3i,%3i)     ",(Int_t)fModuleClusterMap[fModuleID]->PadPlane.size(), (Int_t)fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]].size(),PadMax[0],PadMax[1]);
      Double_t xPosC = -clusterPosInPadLL[0] + 0.5 * fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]][PadMax[1]]->SizeX;
      Double_t yPosC = -clusterPosInPadLL[1] + 0.5 * fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]][PadMax[1]]->SizeY;
      if(fDebug)
	printf ("x(%3i,%3i) y(%3i,%3i)      ",xStart,xStop,yStart,yStop);
      if (xStart < 0)
	xStart = 0;
      if (xStart > fModuleClusterMap[fModuleID]->nCol)
	xStart = fModuleClusterMap[fModuleID]->nCol;
      if (xStop < 0)
	xStop = 0;
      if (xStop > fModuleClusterMap[fModuleID]->nCol)
	xStop = fModuleClusterMap[fModuleID]->nCol;
      if (yStart < 0)
	yStart = 0;
      if (yStart > fModuleClusterMap[fModuleID]->nRow)
	yStart = fModuleClusterMap[fModuleID]->nRow;
      if (yStop < 0)
	yStop = 0;
      if (yStop > fModuleClusterMap[fModuleID]->nRow)
	yStop = fModuleClusterMap[fModuleID]->nRow;
      if(fDebug)
	printf ("x(%3i,%3i) y(%3i,%3i)\n",xStart,xStop,yStart,yStop);
 

      for (Int_t ix = xStart; ix < PadMax[0]; ix++)
	xPosC -= fModuleClusterMap[fModuleID]->PadPlane[ix][PadMax[1]]->SizeX;
      for (Int_t iy = yStart; iy < PadMax[1]; iy++)
	yPosC -= fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]][iy]->SizeY;

      Double_t xPosCStart = xPosC;
      Double_t yPosCStart = yPosC;

      for (Int_t y = yStart; y < yStop; y++) {
	//printf("%3i: ",y);
	for (Int_t x = xStart; x < xStop; x++) {
	  //printf("%3i, ",x);
	  fModuleClusterMap[fModuleID]->PadPlane[x][y]->Charge += 
	    CalcMathieson(xPosC, fModuleClusterMap[fModuleID]->PadPlane[x][y]->SizeX) * 
	    CalcMathieson(yPosC, fModuleClusterMap[fModuleID]->PadPlane[x][y]->SizeY) *
	    ELoss;
	  fModuleClusterMap[fModuleID]->PadPlane[x][y]->MCIndex.push_back(/*fMCindex*/pointId);
	  xPosC += fModuleClusterMap[fModuleID]->PadPlane[x][y]->SizeX;      
	}
	//printf("\n");
	xPosC = xPosCStart;
	yPosC += fModuleClusterMap[fModuleID]->PadPlane[PadMax[0]][y]->SizeY;
      }
    }
}

  // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::InitPadPlane(ClusterModule* mCluster)
  {
    Int_t iXSector = 0;
    Int_t iYSector = 0;
    Int_t secXPad = 0;
    Int_t secYPad = 0;

    //printf("    (%3i,%3i) %3i\n",mCluster->nCol,mCluster->nRow,(Int_t)mCluster->PadPlane.size());
    mCluster->PadPlane.resize(mCluster->nCol);
    for (Int_t xPads = 0; xPads < mCluster->nCol; xPads++){
      mCluster->PadPlane[xPads].resize(mCluster->nRow);
    
      if (xPads == mCluster->SecCol[iXSector]){
	iXSector++;
	secXPad = 0;
      }
    
      for (Int_t yPads = 0; yPads < mCluster->nRow; yPads++){
      
	if (yPads == mCluster->SecRow[iYSector]){
	  iYSector++;
	  secYPad = 0;
	}
      
	ClusterPad* pad = new ClusterPad;    
	mCluster->PadPlane[xPads][yPads] = pad;
      
	mCluster->PadPlane[xPads][yPads]->SizeX  = mCluster->PadSizeX[iXSector];
	mCluster->PadPlane[xPads][yPads]->SizeY  = mCluster->PadSizeY[iYSector];
	mCluster->PadPlane[xPads][yPads]->Charge = 0.0;
      
	secYPad++;
      }

      secXPad++;
    }
    //printf("    (%3i,%3i) (%3i,%3i)\n",mCluster->nCol,mCluster->nRow,(Int_t)mCluster->PadPlane.size(),(Int_t)mCluster->PadPlane[0].size());
  }
  // --------------------------------------------------------------------

  void CbmTrdClusterizerFast::GetModuleInformationFromDigiPar(Int_t VolumeID)
  {
    //cout << "GetModuleInformationFromDigiPar" << endl;
    // fPos is >0 for x and y and not rotated
    // origin of the local coordinate system in 
    // the lower left corner of the chamber, 
    // x to the right (small side of the pads), y up  

    //cout << "---ModuleID: " << VolumeID << endl;

    fModuleInfo = fDigiPar->GetModule(VolumeID);
    if (fModuleInfo != NULL)
      {
	Int_t detID = fModuleInfo->GetDetectorId();

	if (detID != VolumeID ){
	  cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
	}

	Int_t* detInfo = fTrdId.GetDetectorInfo(VolumeID); 
	fModuleType = detInfo[3];
	fModuleCopy = detInfo[4];
	//---------------------------------------------------------------------------------------------------
	std::map<Int_t, ClusterModule* >::iterator it = fModuleClusterMap.find(detID);
	if (it == fModuleClusterMap.end()) {
	  //cout << "------new ModuleID: " << VolumeID << endl;
	  ClusterModule *mCluster = new ClusterModule;
	  fModuleClusterMap[detID] = mCluster;
	  /*
	    add a matrix 0 -> nrow and ncol for pad width and height of each pad to get wride of the padsizematrix !!!! 
	  */
	  Float_t averagePadSizeX;
	  Float_t averagePadSizeY;
	  mCluster -> Station = detInfo[1];
	  mCluster -> Layer = detInfo[2];
	  mCluster -> moduleId = detID;//moduleId;
	
	  mCluster -> ModulePositionX = (Int_t)(10 * fModuleInfo->GetX());
	  mCluster -> ModulePositionY = (Int_t)(10 * fModuleInfo->GetY());
	  mCluster -> ModulePositionZ = (Int_t)(10 * fModuleInfo->GetZ());
	  mCluster -> ModuleSizeX = (fModuleInfo->GetSizex()) * 10. * 2;
	  mCluster -> ModuleSizeY = (fModuleInfo->GetSizey()) * 10. * 2;
	  mCluster -> nCol = fModuleInfo->GetnCol();
	  mCluster -> nRow = fModuleInfo->GetnRow();
	  mCluster -> NoSectors = fModuleInfo->GetNoSectors();

	  const Int_t NoSectors = fModuleInfo->GetNoSectors();
	  mCluster -> SectorSizeX.resize(NoSectors);
	  mCluster -> SectorSizeY.resize(NoSectors);
	  mCluster -> PadSizeX.resize(NoSectors);
	  mCluster -> PadSizeY.resize(NoSectors);
	  mCluster -> SecRow.resize(NoSectors);
	  mCluster -> SecCol.resize(NoSectors);      

	  for (Int_t i = 0; i < NoSectors; i++) {
	    mCluster -> SectorSizeX[i] = 10 * fModuleInfo->GetSectorSizex(i);
	    mCluster -> SectorSizeY[i] = 10 * fModuleInfo->GetSectorSizey(i);
	    mCluster -> PadSizeX[i]    = 10 * fModuleInfo->GetPadSizex(i);
	    mCluster -> PadSizeY[i]    = 10 * fModuleInfo->GetPadSizey(i);
	  }

	  for (Int_t i = 0; i < NoSectors; i++) {
	    mCluster -> SecRow[i]      = Int_t(mCluster->SectorSizeY[i] / mCluster->PadSizeY[i]);
	    mCluster -> SecCol[i]      = Int_t(mCluster->SectorSizeX[i] / mCluster->PadSizeX[i]);
	  }
	  averagePadSizeX /= NoSectors;
	  averagePadSizeY /= NoSectors;

	  InitPadPlane(mCluster);
	}
      }
    else
      {
	printf("fModuleInfo == NULL\n");
      }
  }
  // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::GetModuleInformation(){
    //cout << "GetModuleInformation" << endl;
    // Extract the information about station, layer, module type
    // and cpoy number of the module from the full path to the
    // node.
    // The full path is tokenized at the "/" which diide the different
    // levels of the geometry.
    // Knowing the nameing scheme of the volumes one gets the required
    // information with simple string manipulation.
    // This is probably not the fastes way, but the speed has to be checked.
    // The methode works only for versions of Root > 5.20.0, before the
    // class TStringTocken is not implemented

    TString path = gGeoManager->GetPath();
    cout<<"Path: "<<path<<endl;
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,20,0)
    TStringToken* bla = new TStringToken(path,"/");
#else
    CbmTrdStringToken* bla = new CbmTrdStringToken(path,"/");
#endif

    while (bla->NextToken()) {
      if (bla->Contains("layer")) {
	TString bla3 = (TString) *bla;
	Ssiz_t pos = bla3.Last('_');
	Ssiz_t substringLength=bla3.Length()-pos-1;
	TString bla2 = bla3((bla3.Last('_')+1),substringLength);
	TString bla1 = bla3(3,1);
	fModuleClusterMap[fModuleID]->Station=bla1.Atoi();
	fModuleClusterMap[fModuleID]->Layer=bla2.Atoi();
      }
      if (bla->Contains("mod")){
	TString bla3 = (TString) *bla;
	Ssiz_t pos = bla3.Last('_');
	Ssiz_t substringLength=bla3.Length()-pos-1;
	TString bla2 = bla3(pos+1,substringLength);
	substringLength=pos-7;
	TString bla1 = bla3(7,substringLength);     
	fModuleType = bla1.Atoi();
	fModuleCopy = bla2.Atoi();
	break;
      } 
    }
  }
  // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::AddDigi(const Int_t pointID, Int_t iCol, Int_t iRow, Int_t nCol, Int_t nRow, Double_t iCharge)
  {

  }
  // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::SplitPathSlices()
  {

  }
  // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::WireQuantisation()
  {

  }
  // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::ChargeConservation()
  {

  }
  // --------------------------------------------------------------------
  Double_t CbmTrdClusterizerFast::CalcMathieson(Double_t x, Double_t W)
  {
    Float_t K3 = 0.525;  //Mathieson parameter for 2nd MuBu prototype -> Parametrisation for chamber parameter
    //Float_t K3 = (-0.7/1.6 * (h/s) + 0.7) + ((exp(-9.74350e+02 * ra/s) * 5.64791e-01 + 3.32737e-01));// aproximation of 'E. Mathieson 'Cathode Charge Distributions in Multiwire Chambers' Nuclear Instruments and Methods in Physics Research A270,1988
    //K3 = 5.25407e-01; // MS 336 fit
    //K3 = 6.14949e-01; // MS 444 fit

    //Float_t K2 = 3.14159265 / 2.* ( 1. - sqrt(K3)/2.);
    //Float_t K1 = (K2 * sqrt(K3)) / (4. * atan(sqrt(K3)));
    //Float_t W = 5;
    //Float_t par = 1;
    Float_t h = 3;
    Double_t SqrtK3 = sqrt(K3);
    /*
      Char_t formula[500];
      sprintf(formula," -1. / (2. * atan(sqrt(%f))) * (atan(sqrt(%f) *tanh(3.14159265 * (-2. + sqrt(%f) ) * (%f + 2.* x * %f) / (8.* %f) )) +  atan(sqrt(%f) *  tanh(3.14159265 * (-2. + sqrt(%f) ) * (%f - 2.* x * %f) / (8.* %f) )) )",K3,K3,K3,W,par,h,K3,K3,W,par,h);
      TF1* mathiesonPRF = new TF1("mathieson",formula, -15, 15);
    */
    Double_t mathiesonPRF = fabs(
				 -1. / (2. * atan(SqrtK3)) * (
							      atan(SqrtK3 * tanh(TMath::Pi() * (-2. + SqrtK3 ) * (W + 2.* x) / (8.* h) )) +  
							      atan(SqrtK3 * tanh(TMath::Pi() * (-2. + SqrtK3 ) * (W - 2.* x) / (8.* h) )) 
							      )
				 );
    /*
      -1. / (2. * atan(sqrt(K3))) * (
      atan(sqrt(K3) * tanh(3.14159265 * (-2. + sqrt(K3) ) * (W + 2.* x) / (8.* h) )) +
      atan(sqrt(K3) * tanh(3.14159265 * (-2. + sqrt(K3) ) * (W - 2.* x) / (8.* h) )) 
      );
    */
    return mathiesonPRF;
  }
  // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::TransformLL2C(Double_t* LLCoordinate, Double_t* CCoordinate)
  {
    //Transform coordinates from module coordinate system with origin in the Lower Left (LL) corner to the
    //coordinate system with origin in the Center (C) of the module
 
    CCoordinate[0] = LLCoordinate[0] - 0.5 * fModuleClusterMap[fModuleID]->ModuleSizeX;
    CCoordinate[1] = LLCoordinate[1] - 0.5 * fModuleClusterMap[fModuleID]->ModuleSizeY;
  }
  // --------------------------------------------------------------------
  void CbmTrdClusterizerFast:: TransformC2LL(Double_t* CCoordinate, Double_t* LLCoordinate)
  {
    //Transform the coordinates from coordinate system with origin in the Center (C) of the module 
    // to the coordinate system with origin in the Lower Left (LL) corner

    LLCoordinate[0] = CCoordinate[0] + 0.5 * fModuleClusterMap[fModuleID]->ModuleSizeX;
    LLCoordinate[1] = CCoordinate[1] + 0.5 * fModuleClusterMap[fModuleID]->ModuleSizeY;
    
  }
  // --------------------------------------------------------------------
  void CbmTrdClusterizerFast::GetClusterDisplacement(Double_t* clusterPosInModuleLL, Double_t* clusterPosInPadLL, Int_t* PadMax)
  {
    PadMax[0] = 0; // Fired pad in x-direction
    PadMax[1] = 0; // "      "   " y-direction
    Double_t tempPosX = clusterPosInModuleLL[0];
    Double_t tempPosY = clusterPosInModuleLL[1];
   
    for (Int_t iSector = 0; iSector < fModuleClusterMap[fModuleID]->NoSectors; iSector++)
      {
	if (tempPosX > fModuleClusterMap[fModuleID]->SectorSizeX[iSector])
	  {
	    tempPosX -= fModuleClusterMap[fModuleID]->SectorSizeX[iSector];
	    PadMax[0] += fModuleClusterMap[fModuleID]->SecCol[iSector];
	  }
	else
	  {
	    PadMax[0] += Int_t(tempPosX / Float_t(fModuleClusterMap[fModuleID]->PadSizeX[iSector]));
	    clusterPosInPadLL[0] = tempPosX - Int_t(tempPosX / Float_t(fModuleClusterMap[fModuleID]->PadSizeX[iSector])) * fModuleClusterMap[fModuleID]->PadSizeX[iSector];
	    break;
	  }
      }  
    for (Int_t iSector = 0; iSector < fModuleClusterMap[fModuleID]->NoSectors; iSector++)
      {
	if (tempPosY > fModuleClusterMap[fModuleID]->SectorSizeY[iSector])
	  {
	    tempPosY -= fModuleClusterMap[fModuleID]->SectorSizeY[iSector];
	    PadMax[1] += fModuleClusterMap[fModuleID]->SecRow[iSector];
	  }
	else
	  {
	    PadMax[1] += Int_t(tempPosY / Float_t(fModuleClusterMap[fModuleID]->PadSizeY[iSector]));
	    clusterPosInPadLL[1] = tempPosY - Int_t(tempPosY / Float_t(fModuleClusterMap[fModuleID]->PadSizeY[iSector])) * fModuleClusterMap[fModuleID]->PadSizeY[iSector];
	    break;
	  }
      }
  }
  // --------------------------------------------------------------------
  Int_t CbmTrdClusterizerFast::GetCol(Double_t tempPosX)/*tempPosX has to be in LL module coordinate-systems in [mm]*/
  {
    Int_t iCol = 0;
    for (Int_t iSector = 0; iSector < fModuleClusterMap[fModuleID]->NoSectors; iSector++)
      {
	if (tempPosX > fModuleClusterMap[fModuleID]->SectorSizeX[iSector])
	  {
	    tempPosX -= fModuleClusterMap[fModuleID]->SectorSizeX[iSector];
	    iCol += fModuleClusterMap[fModuleID]->SecCol[iSector];
	  }
	else
	  {
	    iCol += Int_t(tempPosX / Float_t(fModuleClusterMap[fModuleID]->PadSizeX[iSector]));
	    break;
	  }
      }
    return iCol;
  }

  // --------------------------------------------------------------------
  Int_t CbmTrdClusterizerFast::GetRow(Double_t tempPosY)/*tempPosY has to be in LL module coordinate-systems in [mm]*/
  {
    //cout << "GetRow" << endl;
    Int_t iRow = 0;
    for (Int_t iSector = 0; iSector < fModuleClusterMap[fModuleID]->NoSectors; iSector++)
      {
	if (tempPosY > fModuleClusterMap[fModuleID]->SectorSizeY[iSector])
	  {
	    tempPosY -= fModuleClusterMap[fModuleID]->SectorSizeY[iSector];
	    iRow += fModuleClusterMap[fModuleID]->SecRow[iSector];
	  }
	else
	  {
	    iRow += Int_t(tempPosY / Float_t(fModuleClusterMap[fModuleID]->PadSizeY[iSector]));
	    break;
	  }
      }
    return iRow;
  }

  // ---- FinishTask-----------------------------------------------------
  void CbmTrdClusterizerFast::FinishEvent()
  {
 
    for (fModuleClusterMapIt = fModuleClusterMap.begin();
	 fModuleClusterMapIt != fModuleClusterMap.end(); ++fModuleClusterMapIt) {
      fModuleClusterMapIt->second->SectorSizeX.clear();
      fModuleClusterMapIt->second->SectorSizeX.clear();
      fModuleClusterMapIt->second->SectorSizeY.clear();
      fModuleClusterMapIt->second->PadSizeX.clear();
      fModuleClusterMapIt->second->PadSizeY.clear();
      fModuleClusterMapIt->second->SecCol.clear();
      fModuleClusterMapIt->second->SecRow.clear();
      for (Int_t x = 0; x < fModuleClusterMapIt->second->PadPlane.size(); x++) {
	for(Int_t y = 0; y < fModuleClusterMapIt->second->PadPlane[x].size(); y++) {
	  delete fModuleClusterMapIt->second->PadPlane[x][y];
	}
	fModuleClusterMapIt->second->PadPlane[x].clear();
      }
      fModuleClusterMapIt->second->PadPlane.clear();
      delete fModuleClusterMapIt->second;
    }
    fModuleClusterMap.clear();

    if ( fDigiCollection ) fDigiCollection->Clear();
    if ( fDigiMatchCollection ) fDigiMatchCollection->Clear();
  }

  // ---- Register ------------------------------------------------------
  void CbmTrdClusterizerFast::Register()
  {
    FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
    FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
  }
  // --------------------------------------------------------------------

  ClassImp(CbmTrdClusterizerFast)
