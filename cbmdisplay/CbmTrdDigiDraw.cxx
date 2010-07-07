// -------------------------------------------------------------------------
// -----                        CbmTrdDigiDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "CbmTrdDigiDraw.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairEventManager.h"

#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdDigiPar.h"

//#include "TEveQuadSet.h"
#include "TEveManager.h"
#include "TEveBoxSet.h"
//#include "TEveTreeTools.h"

#include "TClonesArray.h"
//#include "TObjArray.h"
#include "TVector3.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::setprecision;

// -----   Default constructor   -------------------------------------------
CbmTrdDigiDraw::CbmTrdDigiDraw() 
  :fVerbose(0),
   fPointList(0),
   fEventManager(0),
   fq(0)
{
 }
// -------------------------------------------------------------------------

                                      

// -----   Standard constructor   ------------------------------------------
CbmTrdDigiDraw::CbmTrdDigiDraw(const char* name, Color_t color ,Style_t mstyle,Int_t iVerbose) 
  : FairTask(name, iVerbose),
    fVerbose(iVerbose),
    fPointList(0),
    fEventManager(0),
    fq(0),
    fColor(color),
    fStyle(mstyle)
{
  for(Int_t i=0; i<3; i++){
    for(Int_t j=0; j<4; j++){
      fActiveLayers[i][j]=kTRUE;
    } 

  } 
}
// ----  Initialisation  ----------------------------------------------
void CbmTrdDigiDraw::SetParContainers()
{
    cout<<" * CbmTrdDigiDraw * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)(rtdb->getContainer("CbmTrdDigiPar"));

}
// --------------------------------------------------------------------

// -------------------------------------------------------------------------
InitStatus CbmTrdDigiDraw::Init()
{
  if(fVerbose>1) cout<<  "CbmTrdDigiDraw::Init()" << endl;
   FairRootManager* fManager = FairRootManager::Instance();
   fPointList = (TClonesArray *)fManager->GetObject(GetName());
   if(fPointList==0){
      cout << "CbmTrdDigiDraw::Init()  branch " << GetName() << " Not found! Task will be deactivated "<< endl; 
      SetActive(kFALSE);
    }
   if(fVerbose>2)  cout<<  "CbmTrdDigiDraw::Init() get track list" <<  fPointList<< endl;
   fEventManager =FairEventManager::Instance();
   if(fVerbose>2) cout<<  "CbmTrdDigiDraw::Init() get instance of FairEventManager " << endl;
   fq =0;
      
  // gEve->AddElement(fq, fEventManager ); 
   return kSUCCESS;
}
// -------------------------------------------------------------------------  
void CbmTrdDigiDraw::Exec(Option_t* option)
{
   if (IsActive()){

   CbmTrdDigi *digi=0;

   Int_t npoints=fPointList->GetEntriesFast();
   Reset();

   /*
   Int_t digiCounter = 0;

   for (Int_t i=0; i<npoints; ++i) {   
     p=(CbmTrdDigi *)fPointList->At(i);
     
     if(p!=0) {       
       Int_t   fDetId = p->GetDetId();
       Int_t fStation = (Int_t) (fDetId/100000);
       Int_t fLayer =  (Int_t) ((fDetId -(fStation*100000))/10000);
       if ( fStation == 1 && fLayer == 1) digiCounter++;
     }

   }
   */

   TEveBoxSet *q = new TEveBoxSet(GetName(),"");
   q->Reset(TEveBoxSet::kBT_AABox, kTRUE, npoints);
   //q->Reset(TEveBoxSet::kBT_AABox, kTRUE, digiCounter);
   //   q->SetMainAlpha(1.);
   q->SetMainColor(kBlue);
   //   q->SetMarkerSize(1.5);
   //   q->SetMarkerStyle(fStyle);

   //   q->SetDefDepth();
   //   q->SetDefHeight();
   //   q->SetDefDepth();

   Int_t refCounter=0;
   Int_t Col, Row, DetId;
   Double_t ELoss;
   Int_t *bla;
   Int_t Station, Layer, ModuleType, ModuleCopy;
   Int_t Sector, moduleId;
   TVector3 posHit, padSize;
   Double_t padsizex, padsizy;
   Double_t X, Y;

   for (Int_t i=0; i<npoints; ++i) {   
     
     digi =  (CbmTrdDigi*) fPointList->At(i);
     

     if(digi!=0) {
       
    
       Col = digi->GetCol();
       Row = digi->GetRow();
       ELoss = digi-> GetCharge();
       DetId = digi->GetDetId();

       // The digi contains the information of the detector Id + the
       // sector number. The digitization parameters are stored per
       // module with arrays holding the information about the sectors.
       // So we have to extract the information about the module Id and
       // the sector from the detector Id.
       bla = fTrdId.GetDetectorInfo(DetId);
       Station = bla[1];
       Layer = bla[2];
       ModuleType = bla[3];
       ModuleCopy = bla[4];
       Sector = bla[5];
       moduleId= fTrdId.GetModuleId(DetId);

       /*
	 cout <<"##########################################"<<endl;
	 cout <<"ID        : "<<DetId<<endl;
	 cout <<"Module ID : "<<moduleId<<endl;
	 cout <<"Sector    : "<<Sector<<endl;
       */


       fModuleInfo = fDigiPar->GetModule(moduleId);
       fModuleInfo->GetPosition(Col, Row, moduleId, Sector, posHit, padSize);



       // The given point is used as the edge of the box but it is
       // the middle point of the pad. So we have to do a transformation
     
      
       X=posHit.X()-(padSize.X()/2);
       Y=posHit.Y()-(padSize.Y()/2);


       if(fVerbose>1){
         cout<<"*** CbmTrdHitProducerDigi::CalculateHitPosition ***"<<endl;
         cout<<"Col: "<< Col <<endl;
         cout<<"Row: "<< Row <<endl;
         cout<<setprecision(5)<<"fPadX: "<< padSize.X() <<endl;
         cout<<setprecision(5)<<"fPadY: "<< padSize.Y() <<endl;
         cout<<setprecision(5)<<"fPosX: "<< posHit.X() <<endl;
         cout<<setprecision(5)<<"fPosY: "<< posHit.Y() <<endl;
         cout<<setprecision(5)<<"fPosZ: "<< posHit.Z() <<endl;
     }

       if ( fActiveLayers[Station-1][Layer-1]) {
       	 //cout <<"S,L: "<<fStation<<","<<fLayer<<endl;
	 q->AddBox(X,Y,posHit.Z(), 
                   padSize.X(), padSize.Y(), 0.);
	 refCounter++;
       }
     }
     
   }
   gEve->AddElement(q);
   gEve->Redraw3D(kFALSE); 
   fq=q;
   //   cout<<"digiCounter: "<<digiCounter<<endl;
   //   cout<<"refCounter : "<<refCounter<<endl;
   }
   
}
// -----   Destructor   ----------------------------------------------------
CbmTrdDigiDraw::~CbmTrdDigiDraw() 
{ 
}
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
  /** Action after each event**/
void CbmTrdDigiDraw::Finish()
{
}
// -------------------------------------------------------------------------
void CbmTrdDigiDraw::Reset()
{
     if(fq!=0){
	   fq->Reset();
       gEve->RemoveElement(fq, fEventManager );
	 }
}

void  CbmTrdDigiDraw::SetLayerStation1(Bool_t Layer1, Bool_t Layer2,
                                       Bool_t Layer3, Bool_t Layer4)
{
  fActiveLayers[0][0]=Layer1;
  fActiveLayers[0][1]=Layer2;
  fActiveLayers[0][2]=Layer3;
  fActiveLayers[0][3]=Layer4;
}
void  CbmTrdDigiDraw::SetLayerStation2(Bool_t Layer1, Bool_t Layer2,
                                       Bool_t Layer3, Bool_t Layer4)
{
  fActiveLayers[1][0]=Layer1;
  fActiveLayers[1][1]=Layer2;
  fActiveLayers[1][2]=Layer3;
  fActiveLayers[1][3]=Layer4;
}
void  CbmTrdDigiDraw::SetLayerStation3(Bool_t Layer1, Bool_t Layer2,
                                       Bool_t Layer3, Bool_t Layer4)
{
  fActiveLayers[2][0]=Layer1;
  fActiveLayers[2][1]=Layer2;
  fActiveLayers[2][2]=Layer3;
  fActiveLayers[2][3]=Layer4;
}


ClassImp(CbmTrdDigiDraw)


