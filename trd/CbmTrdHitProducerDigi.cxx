#include "CbmTrdHitProducerDigi.h"

#include "CbmTrdDigiPar.h"
#include "CbmTrdRadiator.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdDigiMatch.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"

#include <iostream>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdHitProducerDigi::CbmTrdHitProducerDigi()
    :FairTask("TrdHitProducer")
	//:fRef(0)
{

}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdHitProducerDigi::CbmTrdHitProducerDigi(const char *name, const char *title)
	:FairTask(name)
{

}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdHitProducerDigi::~CbmTrdHitProducerDigi()
{
    FairRootManager *ioman =FairRootManager::Instance();
    ioman->Write();
    fHitCollection->Clear("C");
    delete fHitCollection;

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdHitProducerDigi::SetParContainers()
{
    cout<<" * HitProducer * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdHitProducerDigi::ReInit(){

    cout<<" * HitProducer * :: ReInit() "<<endl;


    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

    return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdHitProducerDigi::Init()
{

    cout<<" * HitProducer * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();

    fTrdDigi =(TClonesArray *)  ioman->ActivateBranch("TrdDigi");
    if ( ! fTrdDigi ) {
      cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigi array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }


    fTrdDigiMatch =(TClonesArray *)  ioman->ActivateBranch("TrdDigiMatch");
    if ( ! fTrdDigi ) {
      cout << "-W CbmTrdHitProducerDigi::Init: No TrdDigiMatch array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fMCStack = (TClonesArray*)ioman->ActivateBranch("MCTrack");

    fHitCollection = new TClonesArray("CbmTrdHit", 100);
    ioman->Register("TRDHit","TRD",fHitCollection,kTRUE);

    // Extract information about the number of TRD stations and
    // the number of layers per TRD station from the geomanager.
    // Store the information about the number of layers at the entrance
    // of subsequent stations in a vector. 
    // This allows to calculate the layer number starting with 1 for the
    // first layer of the first station at a later stage by only adding 
    // the layer number in the station to the number of layers in 
    // previous stations 
    TGeoVolume *fm=NULL;
    Int_t stationNr = 1;
    Int_t totalNrOfLayers = 0;
    fLayersBeforeStation.push_back(totalNrOfLayers);
    char volumeName[10];
    sprintf(volumeName, "trd%d", stationNr);
    fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
    if (fm){
      Int_t nrOfLayers = fm->GetNdaughters();
      totalNrOfLayers += nrOfLayers;
      fLayersBeforeStation.push_back(totalNrOfLayers);
      do {
	stationNr++;
        sprintf(volumeName, "trd%d", stationNr);
	fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
        if (fm) {
          nrOfLayers = fm->GetNdaughters();
	  totalNrOfLayers += nrOfLayers;
	  fLayersBeforeStation.push_back(totalNrOfLayers);
	}
      } while (fm);
    } else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducerSmearing        " <<endl;
      cout << " No TRD stations found in the geometry " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFATAL;
    }

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdHitProducerDigi::Exec(Option_t * option)
{

    fHitCollection->Clear();
    CbmTrdDigi *digi=NULL;
    CbmTrdDigiMatch *digimatch=NULL;

    Double_t xHit, yHit, zHit;
    Double_t xHitErr, yHitErr, zHitErr;

    Int_t nentries = fTrdDigi->GetEntries();
    cout<<" ** "<<nentries<<" Trd hits to be created in this event** "<<endl;

    for (int j=0; j < nentries; j++ ) {

      digi =  (CbmTrdDigi*) fTrdDigi->At(j);

      //      digi->Print();

      fCol = digi->GetCol();
      fRow = digi->GetRow();
      Double_t ELoss = digi-> GetCharge();
      fDetId = digi->GetDetId();

      /*
      cout <<"Col: "<<fCol<<endl;
      cout <<"Row: "<<fRow<<endl;
      cout <<"ELoss: "<<ELoss<<endl;
      cout <<"ID: "<<fDetId<<endl;
      */

      CalculateHitPosition();

	
      //      AddHit();

        Int_t Plane=fLayersBeforeStation[fStation-1]+fLayer;

      
        Double_t xHitErr = fpadsizex / TMath::Sqrt(12.);
        Double_t yHitErr = fpadsizey / TMath::Sqrt(12.);
	TVector3 posHit(fPosX, fPosY, fPosZ);
	TVector3 posHitErr(xHitErr,yHitErr, 0.);

	AddHit(posHit, posHitErr, 0, Plane, j , ELoss, 0., 0.);
      

    }

}
// --------------------------------------------------------------------

// ---- Add Hit to HitCollection --------------------------------------

void CbmTrdHitProducerDigi::AddHit(TVector3 &posHit, TVector3 &posHitErr,
			       Int_t index, Int_t Plane, Int_t ref,
			       Double_t ELoss, Double_t ELossTR,
			       Double_t ELossdEdX){

    TClonesArray& clref = *fHitCollection;
    Int_t size = clref.GetEntriesFast();

    
//    new((*fHitCollection)[size]) CbmTrdHit(fDetId, posHit, posHitErr, ref,
//					   Plane, 0., 0., ELoss);

    new((*fHitCollection)[size]) CbmTrdHit(fDetId, posHit, posHitErr, 0., 
                            ref, Plane, 0., 0., ELoss);
}

// --------------------------------------------------------------------

// ---- Finish --------------------------------------------------------
void CbmTrdHitProducerDigi::Finish()
{
}
// --------------------------------------------------------------------

// ---- Register ------------------------------------------------------
void CbmTrdHitProducerDigi::Register(){

  FairRootManager::Instance()->Register("TrdHit","Trd", fHitCollection, kTRUE);

}

// ---- CalculateHitPosition ------------------------------------------
void CbmTrdHitProducerDigi::CalculateHitPosition() {

  fModuleInfo = fDigiPar->GetModule(fDetId);
  Int_t detID = fModuleInfo->GetDetectorId();

  if (detID != fDetId ){
    cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
  }

  Int_t* bla = fTrdId.GetDetectorInfo(fDetId);
  fStation = bla[1];
  fLayer = bla[2];
  fModuleType = bla[3];
  fModuleCopy = bla[4];
 
 /*
  cout <<"##########################################"<<endl;
  cout <<"ID     : "<<fVolumeID<<endl;
  cout <<"System : "<<kTRD<<" , "<<bla[0]<<endl;
  cout <<"Station: "<<fstation<<" , "<<bla[1]<<endl;
  cout <<"Layer  : "<<flayer<<" , "<<bla[2]<<endl;
  cout <<"Type   : "<<fmodtype<<" , "<<bla[3]<<endl;
  cout <<"Copy   : "<<fmodnumber<<" , "<<bla[4]<<endl;


  fStation = (Int_t) (detID/100000);
  fLayer =  (Int_t) ((detID -(fStation*100000))/10000);
  fModuleType =  (Int_t) ((detID -(fStation*100000)-(fLayer*10000))/1000);
  fModuleCopy = (Int_t) (detID - (fStation*100000) - (fLayer*10000) - (fModuleType*1000)); 
  */
    
  fpadsizex = fModuleInfo->GetPadSizex();
  fpadsizey = fModuleInfo->GetPadSizey();
  fsizex    = fModuleInfo->GetSizex();
  fsizey    = fModuleInfo->GetSizey();

  if (fModuleInfo->IsRotated()){
    Double_t tempx = fsizex;
    Double_t tempy = fsizey;
    fsizey = tempx;
    fsizex = tempy;
  }


  Float_t local_point[2];
  local_point[0] = ((fCol+0.5) * fpadsizex) - fsizex + (fpadsizex/2);
  local_point[1] = ((fRow+0.5) * fpadsizey) - fsizey + (fpadsizey/2);

  /*
  cout<<"###################################"<<endl;
  cout<<"Col: "<< fCol <<endl;
  cout<<"Row: "<< fRow <<endl;
  cout<<"fPadX: "<< fpadsizex <<endl;
  cout<<"fPadY: "<< fpadsizey <<endl;
  cout<<"fsizex: "<< fsizex <<endl;
  cout<<"fsizey: "<< fsizey <<endl;
  cout<<"localx: "<<  local_point[0] <<endl;
  cout<<"localy: "<<  local_point[1] <<endl;

  fPosX=local_point[0]+fModuleInfo->GetX();
  fPosY=local_point[1]+fModuleInfo->GetY();
  fPosZ=fModuleInfo->GetZ();
  cout<<"fPosX: "<<  fPosX <<endl;
  cout<<"fPosY: "<<  fPosY <<endl;
  cout<<"###################################"<<endl;
  */

}


ClassImp(CbmTrdHitProducerDigi)
