// -------------------------------------------------------------------------
// -----                        CbmTrdDigiDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "CbmTrdDigiDraw.h"
//#include "FairHit.h"

#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairEventManager.h"

#include "CbmTrdDigi.h"
#include "CbmTrdModule.h"
#include "CbmTrdDigiPar.h"

#include "TEveQuadSet.h"
#include "TEveManager.h"
#include "TEveBoxSet.h"
#include "TEveTreeTools.h"

#include "TClonesArray.h"
#include "TObjArray.h"

#include <iostream>
using std::cout;
using std::endl;

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

   CbmTrdDigi *p=0;

   Int_t npoints=fPointList->GetEntriesFast();
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
   for (Int_t i=0; i<npoints; ++i) {   
     
     p=(CbmTrdDigi *)fPointList->At(i);
     
     if(p!=0) {
       
       Int_t      Col = p->GetCol();
       Int_t      Row = p->GetRow();
       Double_t ELoss = p->GetCharge();
       Int_t   fDetId = p->GetDetId();

       /*
       p->Print();
       cout<<"Col : "<<Col <<endl;
       cout<<"Row : "<<Row <<endl;
       cout<<"ELoss : "<<ELoss <<endl;
       cout<<"fDetID : "<<fDetId <<endl;
       */

       fModuleInfo = fDigiPar->GetModule(fDetId);
       Int_t detID = fModuleInfo->GetDetectorId();

       if (detID != fDetId ){
	 cout<<" -E- This is wrong!!!!!!!!!!!!!!!!!!!!!"<<endl;
       }
    
       Float_t padsizex = fModuleInfo->GetPadSizex();
       Float_t padsizey = fModuleInfo->GetPadSizey();
       Float_t sizex    = fModuleInfo->GetSizex();
       Float_t sizey    = fModuleInfo->GetSizey();

       if (fModuleInfo->IsRotated()){
	 Double_t tempx = sizex;
	 Double_t tempy = sizey;
	 sizey = tempx;
	 sizex = tempy;
       }

       Float_t local_point[2];
       local_point[0] = ((Col+0.5) * padsizex) - sizex;
       local_point[1] = ((Row+0.5) * padsizey) - sizey;

       Float_t X=local_point[0]+fModuleInfo->GetX();
       Float_t Y=local_point[1]+fModuleInfo->GetY();
       Float_t Z=fModuleInfo->GetZ();

       Int_t fStation = (Int_t) (detID/100000);
       Int_t fLayer =  (Int_t) ((detID -(fStation*100000))/10000);


       //   if ( fStation == 1 && fLayer == 1) {
       	 //cout <<"S,L: "<<fStation<<","<<fLayer<<endl;
	 q->AddBox(X, Y, Z, padsizex, padsizey, 0.);
	 //refCounter++;
	 //}
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


ClassImp(CbmTrdDigiDraw)


