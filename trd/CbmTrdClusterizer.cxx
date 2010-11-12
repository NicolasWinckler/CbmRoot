#include "CbmTrdClusterizer.h"

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
CbmTrdClusterizer::CbmTrdClusterizer()
    :FairTask("TrdCluster")
	//:fRef(0)
{
   fDigiCollection = new TClonesArray("CbmTrdDigi");
  fEfficiency = 1;
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdClusterizer::CbmTrdClusterizer(const char *name, const char *title,
                 CbmTrdRadiator *radiator)
	:FairTask(name)
{
  fRadiators = radiator;
  fEfficiency = 1;
}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdClusterizer::~CbmTrdClusterizer()
{
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();
  fDigiCollection->Clear("C");
  delete fDigiCollection;
  fDigiMatchCollection->Clear("C");
  delete fDigiMatchCollection;

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdClusterizer::SetParContainers()
{
    cout<<" * CbmTrdClusterizer * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdClusterizer::ReInit(){

  cout<<" * CbmTrdClusterizer * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdClusterizer::Init()
{

  cout<<" * CbmTrdClusterizer * :: Init() "<<endl;

  FairRootManager *ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
  fTrdPoints=(TClonesArray *)ioman->ActivateBranch("TrdPoint"); 
  if ( ! fTrdPoints ) {
    cout << "-W CbmTrdCluster::Init: No TrdPoints array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }

  fMCStacks = (TClonesArray*)ioman->ActivateBranch("MCTrack");
  if ( ! fMCStacks ) {
    cout << "-W CbmTrdCluster::Init: No MCTrack array!" << endl;
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
void CbmTrdClusterizer::Exec(Option_t * option)
{
  Bool_t lookup = true; // use lookup table or calculate realtime
  Bool_t gaus = false;  // use mathieson or gaus function
  Bool_t fast = true;   // only for realtime!!! calculate over full area or circle area around mc-position
  cout << "================CbmTrdClusterizer=====================" << endl;
  Digicounter = 0;
  CbmTrdPoint *pt=NULL;
  MyPoint* point = new MyPoint;

  TVector3 mom;
  Double_t ELoss;
  Double_t ELossTR;     // TR energy loss for e- & e+
  Double_t ELossdEdX;   // ionization energy loss
  
  
  for (Int_t i = 0; i < 12; i++)
    {
      fLayerZ[i] = 1;
    }
  
  FillMathiesonVector();

  Int_t nEntries = fTrdPoints->GetEntries();
  //nEntries = 1;
  cout << " Found " << nEntries << " MC-Points in Collection of TRD" << endl;
  if (lookup) {
    cout << " Using Mathieson lookup-table" << endl;
  }
  else {
    if (gaus) {
      cout << " Using real time Gauss calculation" << endl;
    }
    else {
      cout << " Using real time Mathieson calculation" << endl;
    }
  }
  //nEntries = nEntries * 1 / 100;
  //nEntries = nEntries * 10 / 100;//5;
    
  for (int j = 0; j < nEntries ; j++ ) 
    {
      //cout << "=========================================================================================" << endl;
      //cout << j << "  ";
      /*
	if (int(j * 10 / float(nEntries)) - int((j-1) * 10 / float(nEntries)) == 1)
	{
	printf("   %3d \n",(int)(j * 100 / float(nEntries)));
	}
      */
      // if random value above fEfficency reject point
      if (gRandom->Rndm() > fEfficiency ) continue;
 
      pt = (CbmTrdPoint*) fTrdPoints->At(j);
      if(NULL == pt) {
	cout << " no point found " << endl;
      }
      if(NULL == pt) continue;

      pt->Momentum(mom);
      fMCindex=pt->GetTrackID();
      //cout << fMCindex << "  ";

      CbmMCTrack *p= (CbmMCTrack*) fMCStacks->At(fMCindex);
      if(NULL == p) {
	cout << " no point found " << endl;
      }

      if(NULL == p) continue;

      Int_t pdgCode = p->GetPdgCode();
  

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
      /*
	global_meanC= {x_mean, y_mean , z_mean};//[cm]      
	global_inC  = {x_in  , y_in   , z_in  };//[cm]      
	global_outC = {x_out , y_out  , z_out };//[cm]
      */
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

      //cout << fModuleID << endl;

      GetModuleInformationFromDigiPar(fModuleID);

      if (fModuleParaMap[fModuleID]->Layer % 2 == 0)
	{
	  Double_t temp = local_meanC[0];
	  local_meanC[0] = - local_meanC[1];
	  local_meanC[1] = temp;

	  temp = local_inC[0];
	  local_inC[0] = - local_inC[1];
	  local_inC[1] = temp;

	  temp = local_outC[0];
	  local_outC[0] = - local_outC[1];
	  local_outC[1] = temp;
	}

      if (fLayerZ[(fModuleParaMap[fModuleID]->Station-1) * 4 + (fModuleParaMap[fModuleID]->Layer-1)] < 2)
	{
	  fLayerZ[(fModuleParaMap[fModuleID]->Station-1) * 4 + (fModuleParaMap[fModuleID]->Layer-1)] = global_inC[2];
	}
      // cout << fStation << " " << fLayer << endl;
      TransformC2LL(local_meanC, local_meanLL, fModuleSize);
      TransformC2LL(local_inC,   local_inLL,   fModuleSize);
      TransformC2LL(local_outC,  local_outLL,  fModuleSize);

     
      //---------------------------------------------------------------------------------
      for (Int_t iDim = 0; iDim < 3; iDim++) {
	point->global_inC[iDim]  = global_inC[iDim];
	point->global_outC[iDim] = global_outC[iDim];
	point->local_inC[iDim]   = local_inC[iDim];
	point->local_outC[iDim]  = local_outC[iDim];
	point->clusterPosC[iDim] = 0;

	point->global_inLL[iDim]  = global_inLL[iDim];
	point->global_outLL[iDim] = global_outLL[iDim];
	point->local_inLL[iDim]   = local_inLL[iDim];
	point->local_outLL[iDim]  = local_outLL[iDim];
	point->clusterPosLL[iDim] = 0;
      }

      point->Sec_in = GetSector(local_inLL[1]);
      point->Row_in = GetRow(local_inLL[1]);
      point->Col_in = GetCol(local_inLL[0]);
      point->Sec_out = GetSector(local_outLL[1]);
      point->Row_out = GetRow(local_outLL[1]);
      point->Col_out = GetCol(local_outLL[0]);
  
      point->Sec_cluster = 0;
      point->Row_cluster = 0;
      point->Col_cluster = 0;  

      point->deltaR = sqrt(pow((local_inLL[0] - local_outLL[0]),2) + pow((local_inLL[1] - local_outLL[1]),2)/* + pow((fz_in - fz_out),2)*/);

      point->ELossTR = 0;
      if(TMath::Abs(pdgCode) == 11){ 
	point->ELossTR = fRadiators->GetTR(mom);
      }
      point->ELossdEdX = pt->GetEnergyLoss();;
      point->ELoss = point->ELossTR + point->ELossdEdX;

      point->alpha = TMath::ATan(fabs(local_inC[0] - local_outC[0]) / fabs(local_outC[2] - local_inC[2])) * (180. / TMath::Pi());
      point->beta  = TMath::ATan(fabs(local_inC[1] - local_outC[1]) / fabs(local_outC[2] - local_inC[2])) * (180. / TMath::Pi());
      point->gamma = TMath::ATan(fabs(sqrt(pow(local_inC[0],2) + pow(local_inC[1],2)) - sqrt(pow(local_outC[0],2) + pow(local_outC[1],2)))
				 / fabs(local_outC[2] - local_inC[2])) * (180. / TMath::Pi());

      //---------------------------------------------------------------------------------

      const Int_t nCol = fModuleParaMap[fModuleID]->nCol;//fnCol; 
      const Int_t nRow = fModuleParaMap[fModuleID]->nRow;//fnRow;
  
      Double_t PadChargeModule[nRow * nCol];     
      Double_t padW[nCol];
      Double_t padH[nRow];
      Int_t iSector = 0;

      Int_t secCounter = 0;
      Int_t tempRow = fModuleParaMap[fModuleID]->SecRow[secCounter];
     
      for (Int_t iRow = 0; iRow < fModuleParaMap[fModuleID]->nRow; iRow++)
	{
	  if (iRow == tempRow)
	    {
	      secCounter++;
	      tempRow += fModuleParaMap[fModuleID]->SecRow[secCounter];
	    }
	  padH[iRow] = GetPadHeight(secCounter);//fModuleParaMap[fModuleID]->PadSizeY[secCounter];
	  for (Int_t iCol = 0; iCol < fModuleParaMap[fModuleID]->nCol; iCol++)
	    {
	      PadChargeModule[iRow * fModuleParaMap[fModuleID]->nCol + iCol] = 0.0;	      
	      padW[iCol] = fModuleParaMap[fModuleID]->PadSizeX[0];
	    }
	} 
      /*  
      for (Int_t i = 0; i < 3 ; i++){
	cout << "H" << GetPadHeight(i) << endl;
      }
      cout << "nCol:" << nCol << " nRow:" << nRow << endl;
      */  
      SplitPathSlices(fast, lookup, gaus, j, point, PadChargeModule, j, padW, padH );
      
    }
  //cout << endl;
  printf(" Added %d TRD Digis to Collection\n  (Including multiple fired digis by differend particles in the same event)\n   %.2f Digis per MC-point in average\n",Digicounter,float(Digicounter/float(nEntries)));
 
  Int_t iDigi=0; 

  for ( fDigiMapIt=fDigiMap.begin() ; fDigiMapIt != fDigiMap.end(); fDigiMapIt++ ){
    if (fDigiMapIt->second->GetDetId() == 111003) {
      cout<<"Add ModID 111003 to TClonesArray"<<endl;
    }
    /*
      new ((*fDigiCollection)[iDigi]) CbmTrdDigi(fDigiMapIt->second->GetDetId(), fDigiMapIt->second->GetCol(), fDigiMapIt->second->GetRow(), fDigiMapIt->second->GetCharge(),fDigiMapIt->second->GetTime(), fDigiMapIt->second->GetFirstMCIndex());
      ;
    */
    new ((*fDigiCollection)[iDigi]) CbmTrdDigi();
    (*fDigiCollection)[iDigi] = fDigiMapIt->second;

    CbmTrdDigiMatch *p = new ((*fDigiMatchCollection)[iDigi]) CbmTrdDigiMatch(); 
    std::vector<int> arr=fDigiMapIt->second->GetMCIndex();
    std::vector<int>::iterator it;

    for (it=arr.begin() ; it <arr.end(); it++  ) {
      Int_t bla = p->AddPoint((Int_t)*it);
    }
    
    iDigi++;
  }
}
   // --------------------------------------------------------------------

    // ---- FinishTask-----------------------------------------------------
void CbmTrdClusterizer::FinishEvent()
{
  fDigiMap.clear();
  if ( fDigiCollection ) fDigiCollection->Clear();
  if ( fDigiMatchCollection ) fDigiMatchCollection->Clear();
}

  // --------------------------------------------------------------------
  // ---- CoordinateTransformation-----------------------------------------------------
void CbmTrdClusterizer::TransformLL2C(Double_t* LLCoordinate, Double_t* CCoordinate, Double_t* StrucDim)
{
  for (Int_t idim = 0; idim < 2; idim++)
    {
      CCoordinate[idim] = LLCoordinate[idim] - 0.5 * StrucDim[idim];
    }
}
  // --------------------------------------------------------------------
  // ---- CoordinateTransformation-----------------------------------------------------
void CbmTrdClusterizer::TransformC2LL(Double_t* CCoordinate, Double_t* LLCoordinate, Double_t* StrucDim)
{
  for (Int_t idim = 0; idim < 2; idim++)
    {
      LLCoordinate[idim] = CCoordinate[idim] + 0.5 * StrucDim[idim];
    }
}
 // --------------------------------------------------------------------
double CbmTrdClusterizer::GetPadHeight(Int_t Sector)
{
  return fModuleParaMap[fModuleID]->PadSizeY[Sector];
}
  // --------------------------------------------------------------------
  // ----GetModuleInformation ------------------------------------------
void CbmTrdClusterizer::GetModuleInformation()
{
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
      fModuleParaMap[fModuleID]->Station=bla1.Atoi();
      fModuleParaMap[fModuleID]->Layer=bla2.Atoi();
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
  // ----GetModuleInformationFromDigiPar ------------------------------------------
void CbmTrdClusterizer::GetModuleInformationFromDigiPar( Int_t VolumeID)
{
  //cout << "GetModuleInformationFromDigiPar" << endl;
  // fPos is >0 for x and y and not rotated
  // origin of the local coordinate system in 
  // the lower left corner of the chamber, 
  // x to the right (small side of the pads), y up  

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
      std::map<Int_t, ModulePara* >::iterator it = fModuleParaMap.find(detID);
      if (it == fModuleParaMap.end()) {
	ModulePara *mPara = new ModulePara;
	fModuleParaMap[detID] = mPara;
    
	mPara -> Station = detInfo[1];
	mPara -> Layer = detInfo[2];
	mPara -> moduleId = detID;//moduleId;

	mPara -> xPos = (Int_t)(10 * fModuleInfo->GetX());
	mPara -> yPos = (Int_t)(10 * fModuleInfo->GetY());
	mPara -> zPos = (Int_t)(10 * fModuleInfo->GetZ());
	mPara -> ModuleSizeX = (fModuleInfo->GetSizex()) * 10. * 2;
	mPara -> ModuleSizeY = (fModuleInfo->GetSizey()) * 10. * 2;
	mPara -> nCol = fModuleInfo->GetnCol();
	mPara -> nRow = fModuleInfo->GetnRow();
	mPara -> NoSectors = fModuleInfo->GetNoSectors();

	const Int_t NoSectors = fModuleInfo->GetNoSectors();
	mPara -> SectorSizeX.resize(NoSectors);
	mPara -> SectorSizeY.resize(NoSectors);
	mPara -> PadSizeX.resize(NoSectors);
	mPara -> PadSizeY.resize(NoSectors);
	mPara -> SecRow.resize(NoSectors);
	mPara -> SecCol.resize(NoSectors);      

	for (Int_t i = 0; i < NoSectors; i++) {
	  mPara -> SectorSizeX[i] = 10 * fModuleInfo->GetSectorSizex(i);
	  mPara -> SectorSizeY[i] = 10 * fModuleInfo->GetSectorSizey(i);
	  mPara -> PadSizeX[i]    = 10 * fModuleInfo->GetPadSizex(i);
	  mPara -> PadSizeY[i]    = 10 * fModuleInfo->GetPadSizey(i);
	}
	if (mPara -> Layer%2 == 0) //un-rotate
	  {
	    mPara -> ModuleSizeX = (fModuleInfo->GetSizey()) * 10. * 2;
	    mPara -> ModuleSizeY = (fModuleInfo->GetSizex()) * 10. * 2;
	    mPara -> nCol = fModuleInfo->GetnRow();
	    mPara -> nRow = fModuleInfo->GetnCol();

	    for (Int_t i = 0; i < NoSectors; i++) {
	      mPara -> SectorSizeX[i] = 10 * fModuleInfo->GetSectorSizey(i);
	      mPara -> SectorSizeY[i] = 10 * fModuleInfo->GetSectorSizex(i);
	      mPara -> PadSizeX[i]    = 10 * fModuleInfo->GetPadSizey(i);
	      mPara -> PadSizeY[i]    = 10 * fModuleInfo->GetPadSizex(i);
	    }
	  }
	for (Int_t i = 0; i < NoSectors; i++) {
	  mPara -> SecRow[i]      = Int_t(mPara->SectorSizeY[i] / mPara->PadSizeY[i]);
	  mPara -> SecCol[i]      = Int_t(mPara->SectorSizeX[i] / mPara->PadSizeX[i]);
	}
      }
      //---------------------------------------------------------------------------------------------------
  
      fModuleSize[0] = fModuleParaMap[detID] -> ModuleSizeX;
      fModuleSize[1] = fModuleParaMap[detID] -> ModuleSizeY;
      fModuleSize[2] = 0.;

    }
  else
    {
      printf("fModuleInfo == NULL\n");
    }
}

  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
float CbmTrdClusterizer::GetFloatPositionY(Double_t tempPosY)
{
  //cout << "GetFloatPositionY" << endl;
  Float_t floatRow = 0;
  for (Int_t iSector = 0; iSector < fModuleParaMap[fModuleID]->NoSectors; iSector++)
    {
      if (tempPosY > fModuleParaMap[fModuleID]->SectorSizeY[iSector])
	{
	  tempPosY -= fModuleParaMap[fModuleID]->SectorSizeY[iSector];
	  floatRow += fModuleParaMap[fModuleID]->SecRow[iSector];
	}
      else
	{
	  floatRow += tempPosY / Float_t(GetPadHeight(iSector));
	  break;
	}
    }
  return floatRow;
}
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
int CbmTrdClusterizer::GetCol(Double_t tempPosX)/*tempPosX has to be in LL module coordinate-systems in [mm]*/
{
  //cout << "GetCol" << endl;
  Int_t iCol   = (Int_t)((tempPosX) / fModuleParaMap[fModuleID]->PadSizeX[0]);
  return iCol;
}
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
int CbmTrdClusterizer::GetRow(Double_t tempPosY)/*tempPosY has to be in LL module coordinate-systems in [mm]*/
{
  //cout << "GetRow" << endl;
  Int_t iRow = 0;
  for (Int_t iSector = 0; iSector < fModuleParaMap[fModuleID]->NoSectors; iSector++)
    {
      if (tempPosY > fModuleParaMap[fModuleID]->SectorSizeY[iSector])
	{
	  tempPosY -= fModuleParaMap[fModuleID]->SectorSizeY[iSector];
	  iRow += fModuleParaMap[fModuleID]->SecRow[iSector];
	}
      else
	{
	  iRow += Int_t(tempPosY / Float_t(GetPadHeight(iSector)));
	  break;
	}
    }
  return iRow;
}
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
int CbmTrdClusterizer::GetSector(Double_t tempPosY)/*tempPosY has to be in LL module coordinate-systems in [mm]*/
{
  //cout << "GetSector" << endl;
  for (Int_t iSector = 0; iSector < fModuleParaMap[fModuleID]->NoSectors; iSector++)
    {
      if (tempPosY > fModuleParaMap[fModuleID]->SectorSizeY[iSector])
	{
	  tempPosY -= fModuleParaMap[fModuleID]->SectorSizeY[iSector];
	}
      else
	{
	  return iSector;
	  break;//?? nötig ??
	}
    }  
}
  // --------------------------------------------------------------------
  // ---- SplitPathSlices -------------------------------------------------
void CbmTrdClusterizer::SplitPathSlices(Bool_t fast, Bool_t lookup, Bool_t gaus, const Int_t pointID, MyPoint *point, Double_t* PadChargeModule, Int_t j, Double_t* padW, Double_t* padH)
{
  //cout << "SplitPathSlices" << endl;
  Float_t ClusterDistance = 0.2 * fModuleParaMap[fModuleID]->PadSizeX[0] - 0.1; //Ar 94 electron/cm    Xe 307 electrons/cm
  Int_t DrawTH = Int_t(15 * fModuleParaMap[fModuleID]->PadSizeX[0] / ClusterDistance)/* * 100*/;
  Int_t nPathSlice = Int_t(point->deltaR / ClusterDistance) + 1;                       

  Double_t W[fPadNrX];
  Double_t H[fPadNrY];

  Double_t delta[2];
  delta[0] = (point->local_outLL[0] - point->local_inLL[0]) / Double_t(nPathSlice); //[mm]
  delta[1] = (point->local_outLL[1] - point->local_inLL[1]) / Double_t(nPathSlice); //[mm]

  //Double_t deltaZ = (local_out[2] - local_in[2]) / double(nTimeSlice); //[mm]
  Double_t SliceELoss = point->ELossdEdX / Float_t(nPathSlice);
  if (nPathSlice == 1)
    {
      SliceELoss += point->ELossTR;
    }
 
  for ( Int_t iPathSlice = 0; iPathSlice < nPathSlice; iPathSlice++)
    {
      if (nPathSlice > 1 && iPathSlice == 0)
	{
	  SliceELoss += point->ELossTR;
	}
      if (nPathSlice > 1 && iPathSlice > 0)
	{
	  SliceELoss = point->ELossdEdX / Float_t(nPathSlice);
	}
      // Cluster position has to be known also in module cs to calc if cluster has traversed a pad boundery
      point->clusterPosLL[0] = point->local_inLL[0] + (0.5 * delta[0]) + (delta[0] * iPathSlice); 
      point->clusterPosLL[1] = point->local_inLL[1] + (0.5 * delta[1]) + (delta[1] * iPathSlice);
      point->Sec_cluster = GetSector(point->clusterPosLL[1]); 

      point->Row_cluster = GetRow(point->clusterPosLL[1]);
      point->Col_cluster = GetCol(point->clusterPosLL[0]);

      WireQuantisation(point);

      point->Row_cluster = GetRow(point->clusterPosLL[1]);
      point->Col_cluster = GetCol(point->clusterPosLL[0]);
      
      point->clusterPosC[0] = (point->clusterPosLL[0] - fModuleParaMap[fModuleID]->PadSizeX[0] * point->Col_cluster) - 0.5 * fModuleParaMap[fModuleID]->PadSizeX[0]; //[mm]
      
      Double_t tempClusterPosCy = point->clusterPosLL[1];
      Int_t tempSecRows = point->Row_cluster;
      Int_t iSector;
      tempy = 0;
      tempx = 0;
      for ( iSector = 0; iSector < fModuleParaMap[fModuleID]->NoSectors; iSector++)
	{
	  if (tempClusterPosCy > fModuleParaMap[fModuleID]->SectorSizeY[iSector])
	    {
	      tempClusterPosCy -= fModuleParaMap[fModuleID]->SectorSizeY[iSector];
	      tempSecRows -= fModuleParaMap[fModuleID]->SecRow[iSector];
	    }
	  else
	    {
	      point->clusterPosC[1] = tempClusterPosCy - tempSecRows * fModuleParaMap[fModuleID]->PadSizeY[iSector] - 0.5 * fModuleParaMap[fModuleID]->PadSizeY[iSector];
	      tempy = point->clusterPosLL[1] - (point->clusterPosC[1] + 0.5 * fModuleParaMap[fModuleID]->PadSizeY[iSector]);
	      break;
	    }
	}

      tempx = point->clusterPosLL[0] - (point->clusterPosC[0] + 0.5 * fModuleParaMap[fModuleID]->PadSizeX[0]);
      
      GetPadSizeMatrix(point, H, W, padH, padW);
      if (lookup) {
	LookupMathiesonVector(point->clusterPosC[0], point->clusterPosC[1], SliceELoss, W, H);
      }
      else {
	if (gaus) {
	  CalcGaus(fast, point->clusterPosC[0], point->clusterPosC[1], SliceELoss, W ,H);
	}
	else {
	  CalcMathieson(fast, point->clusterPosC[0], point->clusterPosC[1], SliceELoss, W ,H);
	}
      }
      ChargeConservation(point);

      ClusterMapping(point, PadChargeModule);

    }

  for (Int_t iRow = 0; iRow < fModuleParaMap[fModuleID]->nRow; iRow++)
    {
      for (Int_t iCol = 0; iCol < fModuleParaMap[fModuleID]->nCol; iCol++)
	{
	  if (PadChargeModule[iRow * fModuleParaMap[fModuleID]->nCol + iCol] > 0)
	    {	      
	      Digicounter++;
	      AddDigi(j/*Digicounter*/, iCol, iRow, fModuleParaMap[fModuleID]->nCol, fModuleParaMap[fModuleID]->nRow, Double_t(PadChargeModule[iRow * fModuleParaMap[fModuleID]->nCol + iCol]));

	    }
	}
    }
}
  // --------------------------------------------------------------------
void CbmTrdClusterizer::WireQuantisation(MyPoint *point)
{
  //cout << "WireQuantisation" << endl;
  Float_t wireSpacing = 3; //[mm] anode wire spacing
  Double_t tempPos = point->clusterPosLL[1];//ClusterMLL[1];
  Int_t iSec = point->Sec_cluster;//GetSector(ClusterMLL[1]);
  Int_t iRow = point->Row_cluster;//GetRow(ClusterMLL[1]);
  Int_t NoWires = Int_t(fModuleParaMap[fModuleID]->PadSizeY[point->Sec_cluster] / wireSpacing);
  for (Int_t i = 0; i < iSec; i++)
    {
      tempPos -= fModuleParaMap[fModuleID]->SectorSizeY[i];
    }
  tempPos -= fModuleParaMap[fModuleID]->PadSizeY[point->Sec_cluster] * (tempPos / Int_t(fModuleParaMap[fModuleID]->PadSizeY[point->Sec_cluster]));
  Int_t nextWire;
  Int_t wireBelow = Int_t(tempPos / wireSpacing);
  Float_t delta = (tempPos / wireSpacing) - wireBelow;
  if (delta >= 0.5) {
    nextWire = wireBelow + 1;
  }
  else {
    nextWire = wireBelow;
  }
 
  Float_t nextWirePos = point->clusterPosLL[1] + (nextWire - (tempPos / wireSpacing)) * wireSpacing;
  point->clusterPosLL[1] = nextWirePos;
}
    // --------------------------------------------------------------------

  void CbmTrdClusterizer::SlowIntegration(Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H)
  {

  }
    // --------------------------------------------------------------------
  void CbmTrdClusterizer::FastIntegration(Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H)
  {
  Int_t rMax = 0;
  Int_t rMin = 0;
  Double_t Q = 0;
  Float_t r = 0.0;         // local pad cylindrical coordinates in anode wire direction; r = sqrt(x^2+y^2) [mm]

  for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++) {
    for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++) {
      fPadCharge[iPadRow][iPadCol] = Q;
    }
  }

  Int_t xStep = 0;
 Int_t xSign[4] = { 1, 1,-1,-1};
  Int_t ySign[4] = { 1,-1, 1,-1};
  Float_t xPos = x_mean;
  Float_t yPos = y_mean;
  Int_t iPadCol = Int_t(fPadNrX/2);
  Int_t iPadRow = Int_t(fPadNrY/2);
  Float_t xNextPad =  0.5 * W[iPadCol];
  Float_t yNextPad =  0.5 * H[iPadRow];

  Float_t xDeltaGridPos = DeltaGrid(x_mean, 0.5 * W[iPadCol]);
  Float_t yDeltaGridPos = DeltaGrid(y_mean, 0.5 * H[iPadRow]);
 
  for (Int_t direction = 0; direction < 4; direction++){
    iPadRow = Int_t(fPadNrY/2);
    yNextPad = ySign[direction] * 0.5 * H[iPadRow];
    for (Int_t yStep = 0; yStep < endOfMathiesonArray; yStep++) { //y
      xStep = 0;
      iPadCol = Int_t(fPadNrX/2);
      xNextPad = xSign[direction] * 0.5 * W[iPadCol];
      yPos = y_mean + yDeltaGridPos + ySign[direction] * yStep;
      if (direction == 0 || direction == 2) {
	if (yPos > yNextPad) {
	  iPadRow++;
	  yNextPad += H[iPadRow];
	}
      }
      else {
	if (yPos < yNextPad) {
	  iPadRow--;
	  yNextPad -= H[iPadRow];
	}
      }
      r = sqrt(pow(xSign[direction] * xStep + xDeltaGridPos ,2) + pow(ySign[direction] * yStep + yDeltaGridPos ,2));
      
      while (Int_t(r * Accuracy) + 2 < endOfMathiesonArray * Accuracy 
	     && iPadCol < fPadNrX
	     && iPadCol >= 0
	     ) { //x
	xPos = x_mean + xDeltaGridPos + xSign[direction] * xStep;
	if (direction == 0 || direction == 1) {
	  if (xPos > xNextPad) {
	    iPadCol++;
	    xNextPad += W[iPadCol];
	  }
	}
	else {
	  if (xPos < xNextPad) {
	    iPadCol--;
	    xNextPad -= W[iPadCol];
	  }
	}
	if (Int_t(r * Accuracy) + 2 < endOfMathiesonArray * Accuracy) {
	  rMin = Int_t(r * Accuracy);
	  rMax = rMin+1;
	  
	  Float_t m = ((fMathieson[rMax] - fMathieson[rMin]) / (Float_t(Accuracy)));
	  Float_t b = fMathieson[rMax] - m * (rMax/Float_t(Accuracy));
	  Q = m * r + b;
	  
	  //Q = 0.1;
	  if (xStep > 0 && yStep > 0) {
	    fPadCharge[iPadRow][iPadCol] += Q;
	  }
	  else {
	    if ((xStep == 0 && direction == 0) || (xStep == 0 && direction == 3)) {
	      fPadCharge[iPadRow][iPadCol] += Q;
	    } 
	    if ((yStep == 0 && direction == 1) || (yStep == 0 && direction == 2)) {
	      fPadCharge[iPadRow][iPadCol] += Q;
	    } 
	  }
	}
	xStep++;
	r = sqrt(pow(xSign[direction] * xStep + xDeltaGridPos ,2) + pow(ySign[direction] * yStep + yDeltaGridPos ,2));
      }   
    }
  }

  for (Int_t iPRow = 0; iPRow < fPadNrY; iPRow++) {
    for (Int_t iPCol = 0; iPCol < fPadNrX; iPCol++) {
      fPadCharge[iPRow][iPCol] *= SliceELoss;
    }
  }
  }
    // --------------------------------------------------------------------
Double_t CbmTrdClusterizer::DeltaGrid(Double_t doubleV, Double_t offset) 
{
  doubleV += offset;       // center origin to lower left corner
  //doubleV -= 0.5;          // grid border offset 0.5 mm
  Int_t intV = doubleV / 1;
  
  Double_t delta = doubleV - intV; 
  
  if(delta >= 0.5) {
    delta = doubleV - (intV + 1);
  }
  return delta + 0.5;
}
// --------------------------------------------------------------------
void CbmTrdClusterizer::LookupMathiesonVector(Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H)
{
  //CalcMathieson(x_mean, y_mean, SliceELoss, W ,H);
  //cout << "LookupMathiesonVector" << endl;
  Int_t rMax = 0;
  Int_t rMin = 0;
  Double_t Q = 0;
  Float_t r = 0.0;         // local pad cylindrical coordinates in anode wire direction; r = sqrt(x^2+y^2) [mm]

  for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++) {
    for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++) {
      fPadCharge[iPadRow][iPadCol] = Q;
    }
  }

  Int_t xStep = 0;

  /*
    Float_t xPosP = x_mean + 0.5;
    Float_t xPosN = x_mean - 0.5;
    Float_t yPosP = y_mean + 0.5;
    Float_t yPosN = y_mean - 0.5;
    Int_t iPadColP = Int_t(fPadNrX/2);
    Int_t iPadColN = Int_t(fPadNrX/2);
    Int_t iPadRowP = Int_t(fPadNrY/2);
    Int_t iPadRowN = Int_t(fPadNrY/2);
    Float_t xNextPadP =  0.5 * W[iPadColP];
    Float_t xNextPadN = -0.5 * W[iPadColN];
    Float_t yNextPadP =  0.5 * H[iPadRowP];
    Float_t yNextPadN = -0.5 * H[iPadRowN];

    Float_t xDeltaGridPos = DeltaGrid(x_mean, 0.5 * W[iPadColP]);
    Float_t yDeltaGridPos = DeltaGrid(y_mean, 0.5 * H[iPadRowP]);
  */
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Int_t xSign[4] = { 1, 1,-1,-1};
  Int_t ySign[4] = { 1,-1, 1,-1};
  Float_t xPos = x_mean;
  Float_t yPos = y_mean;
  Int_t iPadCol = Int_t(fPadNrX/2);
  Int_t iPadRow = Int_t(fPadNrY/2);
  Float_t xNextPad =  0.5 * W[iPadCol];
  Float_t yNextPad =  0.5 * H[iPadRow];

  Float_t xDeltaGridPos = DeltaGrid(x_mean, 0.5 * W[iPadCol]);
  Float_t yDeltaGridPos = DeltaGrid(y_mean, 0.5 * H[iPadRow]);
 
  //cout << x_mean << " - " << xDeltaGridPos << " = " << x_mean-xDeltaGridPos << endl;
  for (Int_t direction = 0; direction < 4; direction++){
    //cout << "for direction" << endl;
    //cout << endl << endl << direction << xSign[direction] << ySign[direction] << endl;
    iPadRow = Int_t(fPadNrY/2);
    yNextPad = ySign[direction] * 0.5 * H[iPadRow];
    //cout << " " << direction;
    for (Int_t yStep = 0; yStep < endOfMathiesonArray; yStep++) { //y
      //cout << " for yStep" << endl;
      //cout << endl;;
      xStep = 0;
      iPadCol = Int_t(fPadNrX/2);
      xNextPad = xSign[direction] * 0.5 * W[iPadCol];
      yPos = y_mean + yDeltaGridPos + ySign[direction] * yStep;
      if (direction == 0 || direction == 2) {
	if (yPos > yNextPad) {
	  iPadRow++;
	  yNextPad += H[iPadRow];
	  //cout << iPadRow << "--------------------- "<< yNextPad << " H"<< H[iPadRow] << endl;
	}
      }
      else {
	if (yPos < yNextPad) {
	  iPadRow--;
	  yNextPad -= H[iPadRow];
	  //cout << iPadRow << "--------------------- "<< yNextPad << " H"<< H[iPadRow] << endl;
	}
      }
      r = sqrt(pow(xSign[direction] * xStep + xDeltaGridPos ,2) + pow(ySign[direction] * yStep + yDeltaGridPos ,2));
      
      while (Int_t(r * Accuracy) + 2 < endOfMathiesonArray/*21*/ * Accuracy 
	     && iPadCol < fPadNrX
	     && iPadCol >= 0
	     ) { //x
	//cout << "x";
	//cout << "  while xStep" << endl;
	xPos = x_mean + xDeltaGridPos + xSign[direction] * xStep;
	if (direction == 0 || direction == 1) {
	  if (xPos > xNextPad) {
	    iPadCol++;
	    xNextPad += W[iPadCol];
	    //cout << "|";
	  }
	}
	else {
	  if (xPos < xNextPad) {
	    iPadCol--;
	    xNextPad -= W[iPadCol];
	    // cout << "|";
	  }
	}
	//r = sqrt(pow(xSign[direction] * xStep + xDeltaGridPos ,2) + pow(ySign[direction] * yStep + yDeltaGridPos ,2));
	
	if (Int_t(r * Accuracy) + 2 < endOfMathiesonArray * Accuracy) {
	  rMin = Int_t(r * Accuracy);
	  rMax = rMin+1;
	  
	  Float_t m = ((fMathieson[rMax] - fMathieson[rMin]) / (Float_t(Accuracy)));
	  Float_t b = fMathieson[rMax] - m * (rMax/Float_t(Accuracy));
	  Q = m * r + b;
	  
	  //Q = 0.1;
	  if (xStep > 0 && yStep > 0) {
	    fPadCharge[iPadRow][iPadCol] += Q;
	  }
	  else {
	    if ((xStep == 0 && direction == 0) || (xStep == 0 && direction == 3)) {
	      fPadCharge[iPadRow][iPadCol] += Q;
	    } 
	    if ((yStep == 0 && direction == 1) || (yStep == 0 && direction == 2)) {
	      fPadCharge[iPadRow][iPadCol] += Q;
	    } 
	  }
	}
	else {
	  cout << "    " << xSign[direction] * xStep + xDeltaGridPos << " " << ySign[direction] * yStep + yDeltaGridPos << " " << r << endl;
	}
	xStep++;
	r = sqrt(pow(xSign[direction] * xStep + xDeltaGridPos ,2) + pow(ySign[direction] * yStep + yDeltaGridPos ,2));
      }  
      //cout << "  while xStep" << endl;   
    }
    //cout << " for yStep" << endl;
  }
  //cout << "for direction" << endl;
  //cout << endl;
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /*
    for (Int_t yStep = 0; yStep < endOfMathiesonArray; yStep++) { //y
    xStep = 0;
    iPadColN = Int_t(fPadNrX/2);
    iPadColP = Int_t(fPadNrX/2);

    xNextPadP =  0.5 * W[iPadColP];
    xNextPadN = -0.5 * W[iPadColN];

    yPosP = y_mean + 0.5 + yStep;
    yPosN = y_mean - 0.5 - yStep;


    if (yPosP > yNextPadP) {
    iPadRowP++;
    yNextPadP += H[iPadRowP];
    }
    if (yPosN < yNextPadN) {
    iPadRowN--;
    yNextPadN -= H[iPadRowN];
    }

    r = sqrt(pow(xStep + 0.5 ,2) + pow(yStep + 0.5 ,2));
 
    while (Int_t(r * Accuracy) < endOfMathiesonArray * Accuracy + 2
    ) { //x
    xPosP = x_mean + 0.5 + xStep;
    xPosN = x_mean - 0.5 - xStep;
    if (xPosP > xNextPadP) {
    iPadColP++;
    xNextPadP += W[iPadColP];
    }
    if (xPosN < xNextPadN) {
    iPadColN--;
    xNextPadN -= W[iPadColN];
    }

    r = sqrt(pow(xStep + 0.5 ,2) + pow(yStep + 0.5 ,2));
    if (Int_t(r * Accuracy) + 2 < endOfMathiesonArray * Accuracy) {
    rMin = Int_t(r * Accuracy);
    rMax = rMin+1;
    Float_t m = ((fMathieson[rMax] - fMathieson[rMin]) / (Float_t(Accuracy)));
    Float_t b = fMathieson[rMax] - m * (rMax/Float_t(Accuracy));
    Q = m * r + b;
    if (iPadRowP < fPadNrY && iPadColP < fPadNrX) {
    fPadCharge[iPadRowP][iPadColP] += Q;
    }
    if (yPosP != yPosN) {
    if (xPosP != xPosN) {
    if (iPadRowP < fPadNrY && iPadColN >= 0) {
    fPadCharge[iPadRowP][iPadColN] += Q;
    }
    if (iPadRowN >= 0 && iPadColP < fPadNrX) {
    fPadCharge[iPadRowN][iPadColP] += Q;
    }
    if (iPadRowN >= 0 && iPadColN >= 0) {
    fPadCharge[iPadRowN][iPadColN] += Q;
    }
    }
    }
    }
    xStep++;
    }      
    }
  */
  for (Int_t iPRow = 0; iPRow < fPadNrY; iPRow++) {
    for (Int_t iPCol = 0; iPCol < fPadNrX; iPCol++) {
      fPadCharge[iPRow][iPCol] *= SliceELoss;
    }
  }
}
  // --------------------------------------------------------------------
  void CbmTrdClusterizer::FillMathiesonVector()
  {
    //cout << "FillMathiesonVector" << endl;
    Double_t Q = 0;
    Float_t h = 3;           //anode-cathode gap [mm]
    Float_t s = 3;           //anode wire spacing [mm]
    Float_t ra = 12.5E-3/2.; //anode wire radius [mm]
    Float_t qa = 1700;       //anode neto charge [??] ???
    Float_t par = 1.0;       // normalization factor
    //Float_t r = 0.0;         // local pad cylindrical coordinates in anode wire direction; r = sqrt(x^2+y^2) [mm]
    Float_t value = 0.0;
    Float_t argument = 0.0;
    Float_t taylor = 0.0;
    Double_t rho = 0.0;     //charge at position x
    Float_t K3 = 0.525;     //Mathieson parameter for 2nd MuBu prototype -> Parametrisation for chamber parameter
    //Float_t K3 = -0.24 * (h / s) + 0.7 + (-0.75 * log(ra / s) - 3.64);// aproximation of 'E. Mathieson 'Cathode Charge Distributions in Multiwire Chambers' Nuclear Instruments and Methods in Physics Research A270,1988
    Float_t K2 = 3.14159265 / 2.* ( 1. - sqrt(K3)/2.);
    Float_t K1 = (K2 * sqrt(K3)) / (4. * atan(sqrt(K3)));
  
    //cout <<  Int_t( sqrt(pow(fPadNrX * 5,2) + pow(fPadNrY * 200,2))) * Accuracy << endl;
    //for (Int_t r = endOfMathiesonArray; r >= 0; r--) // values to be checked !!!!!
    for (Int_t r = 0; r < endOfMathiesonArray * Accuracy; r++) // values to be checked !!!!!
      {
	value = pow(tanh(K2 * (r/Float_t(Accuracy)) / h),2);
	rho = (qa * K1 * (1. - value) /
	       (1. + K3 * value)) / float((accuracy * accuracy)); // accuracy or Accuracy ???
   
	fMathieson[r] = rho;   
      }
  }

  // --------------------------------------------------------------------
void CbmTrdClusterizer::GetPadSizeMatrix(MyPoint *point, Double_t* H, Double_t* W, Double_t* padH, Double_t* padW)
{  
  //cout << "GetPadSizeMatrix" << endl;
  
  Int_t rowPos = point->Row_cluster;
  Int_t colPos = point->Col_cluster;
  for (Int_t iRow = 0; iRow <= fPadNrY / 2; iRow++)
    {
      if (iRow == 0)
	{
	  H[(fPadNrY / 2)] = padH[rowPos];
	}
      else
	{
	  if (rowPos + iRow < fModuleParaMap[fModuleID]->nRow)
	    {
	      H[(fPadNrY / 2) + iRow] = padH[rowPos + iRow];
	    }
	  else
	    {
	      H[(fPadNrY / 2) + iRow] = H[(fPadNrY / 2) + iRow - 1];
	    }
	  if (rowPos - iRow > 0)
	    {
	      H[(fPadNrY / 2) - iRow] = padH[rowPos - iRow];
	    }
	  else
	    {
	      H[(fPadNrY / 2) - iRow] = H[(fPadNrY / 2) - iRow + 1];
	    }
	}
    }
  for (Int_t iCol = 0; iCol <= fPadNrX / 2; iCol++)
    {	   
      if (iCol == 0)
	{
	  W[(fPadNrX / 2)] = padW[colPos];
	}
      else
	{
	  if (colPos + iCol < fModuleParaMap[fModuleID]->nCol)
	    {   
	      W[(fPadNrX / 2) + iCol] = padW[colPos + iCol];
	    }
	  else
	    {
	      W[(fPadNrX / 2) + iCol] = W[(fPadNrX / 2) +iCol - 1];
	    }
	  if (colPos - iCol > 0)
	    {
	      W[(fPadNrX / 2) - iCol] = padW[colPos - iCol];
	    }
	  else
	    {
	      W[(fPadNrX / 2) - iCol] = W[(fPadNrX / 2) - iCol + 1];
	    }
	}
    }
 }
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
  void CbmTrdClusterizer::ChargeConservation(MyPoint *point)
  {
    //cout << "ChargeConservation" << endl;
    /*
      the integration area of CalcMathieson() is not necessary coverd by the module size. Therefore the charge induced outside of the module borders is added to the charge on the first pads inside of the module ranges.
    */
    for (Int_t iRow = 0; iRow <= fPadNrY / 2; iRow++)
      {
	for (Int_t iCol = 0; iCol <= fPadNrX / 2; iCol++)
	  {	      
	    if (point->Row_cluster - (fPadNrY / 2) + iRow < 0)
	      {
		fPadCharge[iRow + 1][iCol] += fPadCharge[iRow][iCol];
		fPadCharge[iRow    ][iCol]  = 0.0;

		fPadCharge[iRow + 1][(fPadNrX-1) - iCol] += fPadCharge[iRow][(fPadNrX-1) - iCol];
		fPadCharge[iRow    ][(fPadNrX-1) - iCol]  = 0.0;
	      }
	      
	    if (point->Row_cluster + (fPadNrY / 2)  - iRow > fModuleParaMap[fModuleID]->nRow)
	      {
		fPadCharge[(fPadNrY-1) - iRow - 1][iCol] += fPadCharge[(fPadNrY-1) - iRow][iCol];
		fPadCharge[(fPadNrY-1) - iRow    ][iCol]  = 0.0;

		fPadCharge[(fPadNrY-1) - iRow - 1][(fPadNrX-1) - iCol] += fPadCharge[(fPadNrY) - iRow][(fPadNrX-1) - iCol];
		fPadCharge[(fPadNrY-1) - iRow    ][(fPadNrX-1) - iCol]  = 0.0;
	      }
	      
	    if (point->Col_cluster - (fPadNrX / 2) + iCol < 0)
	      { 
		fPadCharge[iRow][iCol + 1] += fPadCharge[iRow][iCol];
		fPadCharge[iRow][iCol    ]  = 0.0;
	      }
	      
	    if (point->Col_cluster + (fPadNrX / 2) - iCol > fModuleParaMap[fModuleID]->nCol)
	      {
		fPadCharge[iRow][(fPadNrX-1) - iCol - 1] += fPadCharge[iRow][(fPadNrX-1) - iCol];
		fPadCharge[iRow][(fPadNrX-1) - iCol    ]  = 0.0;
	      }	      	     
	  }
      }  
    //cout << "ChargeConservation ende" << endl;
  }
  // --------------------------------------------------------------------
  // --------------------------------------------------------------------
  void CbmTrdClusterizer::ClusterMapping(MyPoint *point, Double_t* PadChargeModule)
  {
    //cout << "ClusterMapping" << endl;
    /*
      Associates the integration are used within CalcMathieson() to the pad area of the module
    */  
    for (Int_t iRow = 0; iRow < fModuleParaMap[fModuleID]->nRow; iRow++)
      {
	for (Int_t iCol = 0; iCol < fModuleParaMap[fModuleID]->nCol; iCol++)
	  {	
	    if (iCol >= point->Col_cluster - (fPadNrX / 2) && iCol <= point->Col_cluster + (fPadNrX / 2))
	      { 		  
		if (iRow >= point->Row_cluster - (fPadNrY / 2) && iRow <= point->Row_cluster + (fPadNrY / 2))
		  {
		    PadChargeModule[iRow * fModuleParaMap[fModuleID]->nCol + iCol] += fPadCharge[iRow - (point->Row_cluster - (fPadNrY / 2))][iCol - (point->Col_cluster - (fPadNrX / 2))];
		  }
	      }
	  }
      }  
  }

  // --------------------------------------------------------------------
void CbmTrdClusterizer::CalcGaus(Bool_t fast, Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H)
{
  Float_t sigma = 2.28388e+00;//8.08257e-01; // [mm]
  Float_t amplitude = 2.96216e-01;//2.96136e-01;
  Float_t rho = 0;
  Float_t Q = 0;
  Float_t r;
  for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++) {
    for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++) {
      fPadCharge[iPadRow][iPadCol] = Q;
    }
  }
  if (fast) {
    Int_t xStep = 0;
    Float_t xPosP = x_mean + 0.5;
    Float_t xPosN = x_mean - 0.5;
    Float_t yPosP = y_mean + 0.5;
    Float_t yPosN = y_mean - 0.5;
    Int_t iPadColP = Int_t(fPadNrX/2);
    Int_t iPadColN = Int_t(fPadNrX/2);
    Int_t iPadRowP = Int_t(fPadNrY/2);
    Int_t iPadRowN = Int_t(fPadNrY/2);
    Float_t xNextPadP =  0.5 * W[iPadColP];
    Float_t xNextPadN = -0.5 * W[iPadColN];
    Float_t yNextPadP =  0.5 * H[iPadRowP];
    Float_t yNextPadN = -0.5 * H[iPadRowN];

    for (Int_t yStep = 0; yStep < endOfMathiesonArray; yStep++) { //y
      xStep = 0;
      iPadColN = Int_t(fPadNrX/2);
      iPadColP = Int_t(fPadNrX/2);

      xNextPadP =  0.5 * W[iPadColP];
      xNextPadN = -0.5 * W[iPadColN];

      yPosP = y_mean + 0.5 + yStep;
      yPosN = y_mean - 0.5 - yStep;


      if (yPosP > yNextPadP) {
	iPadRowP++;
	yNextPadP += H[iPadRowP];
      }
      if (yPosN < yNextPadN) {
	iPadRowN--;
	yNextPadN -= H[iPadRowN];
      }

      r = sqrt(pow(xStep + 0.5 ,2) + pow(yStep + 0.5 ,2));
 
      while (Int_t(r * Accuracy) < endOfMathiesonArray * Accuracy + 2
	     /*
	       && iPadColP < fPadNrX 
	       && iPadColN >= 0
	     */ ) { //x
	xPosP = x_mean + 0.5 + xStep;
	xPosN = x_mean - 0.5 - xStep;
	if (xPosP > xNextPadP) {
	  iPadColP++;
	  xNextPadP += W[iPadColP];
	}
	if (xPosN < xNextPadN) {
	  iPadColN--;
	  xNextPadN -= W[iPadColN];
	}

	r = sqrt(pow(xStep + 0.5 ,2) + pow(yStep + 0.5 ,2));
	rho = amplitude * exp(-0.5 * pow((r/sigma),2));

	if (iPadRowP < fPadNrY && iPadColP < fPadNrX) {
	  fPadCharge[iPadRowP][iPadColP] += rho;
	}
      

	if (yPosP != yPosN) {
	  if (xPosP != xPosN) {
	    if (iPadRowP < fPadNrY && iPadColN >= 0) {
	      fPadCharge[iPadRowP][iPadColN] += rho;
	    }
	    if (iPadRowN >= 0 && iPadColP < fPadNrX) {
	      fPadCharge[iPadRowN][iPadColP] += rho;
	    }
	    if (iPadRowN >= 0 && iPadColN >= 0) {
	      fPadCharge[iPadRowN][iPadColN] +=rho;
	    }
	  }      
	}
	xStep++;
      }
    }
  }
  else {
    for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++) { 
      for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++) { 
	for (Int_t yi = 0; yi < H[iPadRow] * accuracy; yi++) {
	  for (Int_t xi = 0; xi < W[iPadCol] * accuracy; xi++) {
			  
	    //Mathieson coordinate system ist centered in the center of the hit pad 
	    r = sqrt(
		     pow(((iPadCol - int(fPadNrX/2)) * W[iPadCol] + (xi + 0.5) / float(accuracy) - 0.5 * W[iPadCol]) - x_mean,2) + 
		     pow(((iPadRow - int(fPadNrY/2)) * H[iPadRow] + (yi + 0.5) / float(accuracy) - 0.5 * H[iPadRow]) - y_mean,2)
			 
		     );
	    rho = amplitude * exp(-0.5 * pow((r/sigma),2));
	    fPadCharge[iPadRow][iPadCol] += rho;
	  }
	}
      }
    }
  }

  for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++) {
    for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++) {
      fPadCharge[iPadRow][iPadCol] *= SliceELoss;
    }
  }

}

    // ---- CalcMathieson -------------------------------------------------
void CbmTrdClusterizer::CalcMathieson(Bool_t fast, Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H)
{
  //Bool_t fast = false;
  //cout << "CalcMathieson" << endl;
  /*
    Calculates the induced charge on tha area defind by 'fPadNrX' aand 'fPadNrY'  by using the mathieson formula for each cluster
  */
  //printf("CalcMathieson...\n");
  //Int_t accuracy = 1;    // '1/accuracy' integration step width [mm]
  Float_t h = 3;           //anode-cathode gap [mm]
  Float_t s = 3;           //anode wire spacing [mm]
  Float_t ra = 12.5E-3/2.; //anode wire radius [mm]
  Float_t qa = 1700;       //anode neto charge [??] ???
  Float_t par = 1.0;       // normalization factor
  Float_t value = 0.0;
  Float_t argument = 0.0;
  Float_t taylor = 0.0;
  Double_t rho = 0.0;      //charge at position x
  Float_t K3 = 0.525;      //Mathieson parameter for 2nd MuBu prototype -> Parametrisation for chamber parameter
  //Float_t K3 = -0.24 * (h / s) + 0.7 + (-0.75 * log(ra / s) - 3.64);// aproximation of 'E. Mathieson 'Cathode Charge Distributions in Multiwire Chambers' Nuclear Instruments and Methods in Physics Research A270,1988
  Float_t K2 = 3.14159265 / 2.* ( 1. - sqrt(K3)/2.);
  Float_t K1 = (K2 * sqrt(K3)) / (4. * atan(sqrt(K3)));
  TH2F* Test = NULL;
  TH2F* TestPos = NULL;
  TH2F* Test2 = NULL;
  Double_t Q = 0;
  Float_t r = 0.0;         // local pad cylindrical coordinates in anode wire direction; r = sqrt(x^2+y^2) [mm]
  
  for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++) {
    for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++) {
      fPadCharge[iPadRow][iPadCol] = Q;
    }
  }
  if (fast) {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Int_t xStep = 0;
  
    Float_t xPosP = x_mean + 0.5;
    Float_t xPosN = x_mean - 0.5;
    Float_t yPosP = y_mean + 0.5;
    Float_t yPosN = y_mean - 0.5;
    Int_t iPadColP = Int_t(fPadNrX/2);
    Int_t iPadColN = Int_t(fPadNrX/2);
    Int_t iPadRowP = Int_t(fPadNrY/2);
    Int_t iPadRowN = Int_t(fPadNrY/2);
    Float_t xNextPadP =  0.5 * W[iPadColP];
    Float_t xNextPadN = -0.5 * W[iPadColN];
    Float_t yNextPadP =  0.5 * H[iPadRowP];
    Float_t yNextPadN = -0.5 * H[iPadRowN];

    Float_t xDeltaGridPos = DeltaGrid(x_mean, 0.5 * W[iPadColP]);
    Float_t yDeltaGridPos = DeltaGrid(y_mean, 0.5 * H[iPadRowP]);

    for (Int_t yStep = 0; yStep < endOfMathiesonArray; yStep++) { //y
      xStep = 0;
      iPadColN = Int_t(fPadNrX/2);
      iPadColP = Int_t(fPadNrX/2);

      xNextPadP =  0.5 * W[iPadColP];
      xNextPadN = -0.5 * W[iPadColN];

      yPosP = y_mean + 0.5 + yStep;
      yPosN = y_mean - 0.5 - yStep;


      if (yPosP > yNextPadP) {
	iPadRowP++;
	yNextPadP += H[iPadRowP];
      }
      if (yPosN < yNextPadN) {
	iPadRowN--;
	yNextPadN -= H[iPadRowN];
      }

      r = sqrt(pow(xStep + 0.5 ,2) + pow(yStep + 0.5 ,2));
 
      while (Int_t(r * Accuracy) + 2 < endOfMathiesonArray * Accuracy 
	     /*
	       && iPadColP < fPadNrX 
	       && iPadColN >= 0
	     */ ) { //x
	xPosP = x_mean + 0.5 + xStep;
	xPosN = x_mean - 0.5 - xStep;
	if (xPosP > xNextPadP) {
	  iPadColP++;
	  xNextPadP += W[iPadColP];
	}
	if (xPosN < xNextPadN) {
	  iPadColN--;
	  xNextPadN -= W[iPadColN];
	}
	/*
	  cout << "xStep:" << xStep << " yStep:" << yStep << endl;
	  cout << "    xPosP:" << xPosP << "   nW:" << xNextPadP << "   W:" << W[iPadColP]  << "     ColP:" << iPadColP << endl;
	  cout << "    xPosN:" << xPosN << "   nW:" << xNextPadN << "   W:" << W[iPadColN]  << "     ColN:" << iPadColN << endl;
	  cout << "    yPosP:" << yPosP << "   nH:" << yNextPadP << "   H:" << H[iPadRowP]  << "     RowP:" << iPadRowP << endl;
	  cout << "    yPosN:" << yPosN << "   nH:" << yNextPadN << "   H:" << H[iPadRowN]  << "     RowN:" << iPadRowN << endl << endl;
	*/
	r = sqrt(pow(xStep + 0.5 ,2) + pow(yStep + 0.5 ,2));
	value = pow(tanh(K2 * r / h),2);
	rho = (qa * K1 * (1. - value) /
	       (1. + K3 * value)) / float((accuracy * accuracy));

	if (iPadRowP < fPadNrY && iPadColP < fPadNrX) {
	  fPadCharge[iPadRowP][iPadColP] += rho;
	}
      

	if (yPosP != yPosN) {
	  if (xPosP != xPosN) {
	    if (iPadRowP < fPadNrY && iPadColN >= 0) {
	      fPadCharge[iPadRowP][iPadColN] += rho;
	    }
	    if (iPadRowN >= 0 && iPadColP < fPadNrX) {
	      fPadCharge[iPadRowN][iPadColP] += rho;
	    }
	    if (iPadRowN >= 0 && iPadColN >= 0) {
	      fPadCharge[iPadRowN][iPadColN] +=rho;
	    }
	  }      
	}
	xStep++;
      }    
    }
    /*
      for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++) {
      for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++) {
      fPadCharge[iPadRow][iPadCol] *= SliceELoss;
      }
      }
    */
  
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  }
  else {
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
    /**
     * Basic difference between the previous (this) and the activ algorithm is 
     * that the activ on uses 1 integration step per mm distance from the mc-point.
     * This leads to symmetric integration values around the mc-point, but does not 
     * ensure same number of integration steps per pad area for all pads since this 
     * number depends now on the relative mc-coordinates. Therefor the results of this 
     * new faster version differs a little bit from the previous one. The previous algorithm
     * used integration step positions defined by the pad sizes which ensured same number of 
     * integration steps per pad area.
     **/    
 
    //************************************************************************
    Float_t testMinMax, testMaxMin, testMinMin, testMaxMax;
    Float_t termA, termB;
    for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++) { 
      //termB = H[iPadRow] / Float_t(accuracy) - 0.5 * H[iPadRow];
      for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++) { 
	//termA = W[iPadCol] / Float_t(accuracy) - 0.5 * W[iPadCol];
	/* 
	   testMinMin = sqrt(
	   pow(((iPadCol     - Int_t(fPadNrX/2)) * termA) - x_mean,2) + 
	   pow(((iPadRow     - Int_t(fPadNrY/2)) * termB) - y_mean,2)			 
	   );
	   testMaxMax = sqrt(
	   pow((((iPadCol+1) - Int_t(fPadNrX/2)) * termA) - x_mean,2) + 
	   pow((((iPadRow+1) - Int_t(fPadNrY/2)) * termB) - y_mean,2)			 
	   );
	   testMinMax = sqrt(
	   pow(((iPadCol     - Int_t(fPadNrX/2)) * termA) - x_mean,2) + 
	   pow((((iPadRow+1) - Int_t(fPadNrY/2)) * termB) - y_mean,2)			 
	   );
	   testMaxMin = sqrt(
	   pow((((iPadCol+1) - Int_t(fPadNrX/2)) * termA) - x_mean,2) + 
	   pow(((iPadRow     - Int_t(fPadNrY/2)) * termB) - y_mean,2)			 
	   );
	   if ( (Int_t(testMinMin * Accuracy)+2 > endOfMathiesonArray * Accuracy) && (Int_t(testMaxMax * Accuracy)+2 > endOfMathiesonArray * Accuracy) &&
	   (Int_t(testMinMax * Accuracy)+2 > endOfMathiesonArray * Accuracy) && (Int_t(testMaxMin * Accuracy)+2 > endOfMathiesonArray * Accuracy)) {
	    
	   //continue;
	   }
	*/
	//*********************************************************************************
	for (Int_t yi = 0; yi < H[iPadRow] * accuracy; yi++) {
	  for (Int_t xi = 0; xi < W[iPadCol] * accuracy; xi++) {
			  
	    //Mathieson coordinate system ist centered in the center of the hit pad 
	    r = sqrt(
		     pow(((iPadCol - int(fPadNrX/2)) * W[iPadCol] + (xi + 0.5) / float(accuracy) - 0.5 * W[iPadCol]) - x_mean,2) + 
		     pow(((iPadRow - int(fPadNrY/2)) * H[iPadRow] + (yi + 0.5) / float(accuracy) - 0.5 * H[iPadRow]) - y_mean,2)
			 
		     );
	    value = pow(tanh(K2 * r / h),2);
		    
	    // argument = K2 * r / h;
	    // taylor = argument - pow(argument,3) / 3. + 2 * pow(argument,5) / 15. - 17 * pow(argument,7) / 315. + 62 * pow(argument,9) / 2835;
	    // value = pow(taylor,2);
		    
	    //rho = r;
	    //rho = (iPadRow * fPadNrX + iPadCol) / (H[iPadRow] * accuracy * W[iPadCol] * accuracy);
	    //rho = (1.) / (H[iPadRow] * accuracy * W[iPadCol] * accuracy);
		   
	    //rho = (qa * K1 * (1. - tanh(K2 * r / h) * tanh(K2 * r / h)) /
	    //(1. + K3 * tanh(K2 * r / h) * tanh(K2 * r / h))) / float((accuracy * accuracy));  
		     
	    rho = (qa * K1 * (1. - value) /
		   (1. + K3 * value)) / float((accuracy * accuracy));         
		 
	    if (rho > 0.00) {
	      fPadCharge[iPadRow][iPadCol] += rho;// * SliceELoss;
	    }
	  } //xi
	} //yi
      } //iPadCol
    } //iPadRow    
    //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  } //else

  for (Int_t iPadRow = 0; iPadRow < fPadNrY; iPadRow++) {
    for (Int_t iPadCol = 0; iPadCol < fPadNrX; iPadCol++) {
      fPadCharge[iPadRow][iPadCol] *= SliceELoss;
    }
  }

}

    // -------------------------------------------------------------------
    // ------AddDigi--------------------------------------------------------------

    //void CbmTrdClusterizer::AddDigi() 
    void CbmTrdClusterizer::AddDigi(const Int_t pointID, Int_t iCol, Int_t iRow, Int_t nCol, Int_t nRow, Double_t iCharge) 
    {
      //cout << "AddDigi" << endl;
      // Add digi for pixel(x,y) in module to map for intermediate storage
      // In case the pixel for this pixel/module combination does not exists
      // it is added to the map.
      // In case it exists already the information about another hit in this
      // pixel is added. Also the additional energy loss is added to the pixel.

  
      // Look for pixel in charge map
      //pair<Int_t, Int_t> a(fCol_mean, fRow_mean);
      Int_t temp;
      pair<Int_t, Int_t> a;
      if (fModuleParaMap[fModuleID]->Layer % 2 == 0)
	{
	  //      pair<Int_t, Int_t> a(iRow, iCol);
	  /*
	    a.first=iRow;
	    a.second=iCol;
	  */
	  temp = iCol;
	  iCol = fModuleParaMap[fModuleID]->nRow - (fModuleParaMap[fModuleID]->nRow - (iRow + 1) + 1);
	  iRow = fModuleParaMap[fModuleID]->nCol - (temp + 1);
	}
      else
	{
	  //      pair<Int_t, Int_t> a(iCol, iRow);
	  /*
	    a.first=iCol;
	    a.second=iRow;
	  */
	}
      a.first =iCol;
      a.second=iRow;
      pair<Int_t, pair<Int_t,Int_t> > b(fModuleID, a);
      fDigiMapIt = fDigiMap.find(b);

      //    cout<<"DetID: "<<fModuleID<<endl;

      // Pixel not yet in map -> Add new pixel
      // cout << pointID << endl;
      if ( fDigiMapIt == fDigiMap.end() ) {
	//  CbmTrdDigi* digi = new CbmTrdDigi(fModuleID, fCol_mean, fRow_mean, fELoss, fMCindex);
	fTime = 0.0;
	CbmTrdDigi* digi = new CbmTrdDigi(fModuleID, iCol, iRow, iCharge, fTime, pointID);
	//TODO:  hier muessen noch die digis mit zu niedriger ladung abgefangen werden (-> pointID = 1E6) damit sich die Hits eindeutiger den MCs zuordnen lassen ??
	fDigiMap[b] = digi;
      }

      // Pixel already in map -> Add charge
      else {
	CbmTrdDigi* digi = (CbmTrdDigi*)fDigiMapIt->second;
	if ( ! digi ) Fatal("AddChargeToPixel", "Zero pointer in digi map!");
	digi->AddCharge(iCharge);
	if (digi->GetMCIndex().back() != pointID) { // avoid double pointIDs by SplitPathSlice()
	  digi->AddMCIndex(pointID);
	}
	//if( fTime > (digi->GetTime()) ) digi->SetTime(fTime);
      }
  
    }

    // ---- Register ------------------------------------------------------
    void CbmTrdClusterizer::Register()
    {
      FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);
      FairRootManager::Instance()->Register("TrdDigiMatch","Trd Digi Match", fDigiMatchCollection, kTRUE);
    }
    // --------------------------------------------------------------------

    ClassImp(CbmTrdClusterizer)
