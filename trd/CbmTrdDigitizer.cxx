#include "CbmTrdDigitizer.h"

//#include "CbmTutorialDetDigiPar.h"
#include "CbmTrdPoint.h"

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
#include "TGeoMaterial.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TPRegexp.h"

#include <iostream>
using std::cout;
using std::endl;

// ---- Default constructor -------------------------------------------
CbmTrdDigitizer::CbmTrdDigitizer()
    :FairTask("TrdDigitizer")
	//:fRef(0)
{
  //    fDigiCollection = new TClonesArray("CbmTrdDigi");
}
// --------------------------------------------------------------------

// ---- Constructor ----------------------------------------------------
CbmTrdDigitizer::CbmTrdDigitizer(const char *name, const char *title)
	:FairTask(name)
{

}
// --------------------------------------------------------------------

// ---- Destructor ----------------------------------------------------
CbmTrdDigitizer::~CbmTrdDigitizer()
{
  //    FairRootManager *ioman =FairRootManager::Instance();
  //ioman->Write();
  //fDigiCollection->Clear("C");
  //delete fDigiCollection;

}
// --------------------------------------------------------------------

// ----  Initialisation  ----------------------------------------------
void CbmTrdDigitizer::SetParContainers()
{
    cout<<" * CbmTrdDigitizer * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    //    fDigiPar = (CbmTrdDigiPar*)
    //               (rtdb->getContainer("CbmTrdDigiPar"));

    //    fDigiPar->printparams();
}
// --------------------------------------------------------------------

// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdDigitizer::ReInit(){

  cout<<" * CbmTrdDigitizer * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  //  fDigiPar = (CbmTrdDigiPar*)
  //    (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdDigitizer::Init()
{

    cout<<" * CbmTrdDigitizer * :: Init() "<<endl;

    FairRootManager *ioman = FairRootManager::Instance();
    if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
    fTrdPoints=(TClonesArray *)  
      ioman->ActivateBranch("TRDPoint");
 
    if ( ! fTrdPoints ) {
      cout << "-W CbmTrdDigitizer::Init: No TrdPoints array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    //fListStack = (TClonesArray*)ioman->ActivateBranch("MCTrack");
    //fDigiCollection = new TClonesArray("CbmTrdDigi", 100);
    //ioman->Register("TRDDigi","TRD Digis",fDigiCollection,kTRUE);

    return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdDigitizer::Exec(Option_t * option)
{

  // Here we print something

  cout <<" I am in CbmTrdDigitizer::Exec" << endl;
    
  //  fNHits = 0;
  CbmTrdPoint *pt=NULL;
  
  Int_t nentries = fTrdPoints->GetEntriesFast();
  
  Double_t ELoss;       // total energy loss
  
  for (int j=0; j < nentries; j++ ) {
  
    pt = (CbmTrdPoint*) fTrdPoints->At(j);
  
    if(NULL == pt) continue;

    // Calculate point in the middle of the detector. This is
    // for safety reasons, because the point at exit is slightly
    // outside of the active volume. If one does not use a point
    // in the detector one will get a wrong volume from the
    // geomanager. Navigate to the correct volume to get all
    // necessary information about this volume

        Double_t x_mean = (pt->GetXIn()+pt->GetXOut())/2.;
        Double_t y_mean = (pt->GetYIn()+pt->GetYOut())/2.;
        Double_t z_mean = (pt->GetZIn()+pt->GetZOut())/2.;
        gGeoManager->FindNode(x_mean, y_mean, z_mean);

        GetModuleInformation();
      
	// Calculate the position in the chamber with the origin of
        // the coordinate system in the 
        // (upper left corner of the chamber)?????
        // TODO: define coordinate system for the chambers. Maybe better
        // to have the origin in the middle

        const Double_t *global_point = gGeoManager->GetCurrentPoint();
        Double_t local_point[3];  // global_point[3];
        gGeoManager->MasterToLocal(global_point, local_point);

        TGeoHMatrix *matrix = gGeoManager->GetCurrentMatrix(); 

	// Get the size of the active area
        TGeoNode *curnode = gGeoManager->GetCurrentNode();
        TGeoVolume *curvol = gGeoManager->GetCurrentVolume();
        TGeoBBox *shape = (TGeoBBox*)curvol->GetShape(); 
	//        Float_t sizez = shape->GetDZ();


        //Orientation of the detector layers
        //Layer 1 and 3 have resolution in x-direction (Orientation=1)
        //Layer 2 and 4 have resolution in y-direction (Orientation=0)
        Int_t Orientation=fLayer%2;

	cout<<"Station: "<<fStation<<endl;
	cout<<"Layer: "<<fLayer<<endl;
        cout<<"Orientation: "<<Orientation<<endl;
	cout<<"Module Type: "<<fModuleType<<endl;
	cout<<"Copy Nr.: "<<fModuleCopy<<endl;
        
	Float_t sizex;
	Float_t sizey;
	Float_t padsizex;
	Float_t padsizey;

        if( 1 == Orientation) {
          //check if the chamber is rotated
          if (matrix->IsRotation()){
            cout <<"Module is rotated" << endl;
            sizex = shape->GetDY();
            sizey = shape->GetDX();
	    padsizex = 0.5;
	    padsizey = 2;
   	  } else {
	    cout <<"Module is not rotated" << endl;
	    sizex = shape->GetDX();
	    sizey = shape->GetDY();
	    padsizex = 0.5;
	    padsizey = 2;
	  }
	}
        else if( 0 == Orientation) {
          //check if the chamber is rotated
          if (matrix->IsRotation()){
            cout <<"Module is rotated" << endl;
            sizex = shape->GetDY();
            sizey = shape->GetDX();
	    padsizex = 2.;
	    padsizey = 0.5;           
   	  } else {
	    cout <<"Module is not rotated" << endl;
	    sizex = shape->GetDX();
	    sizey = shape->GetDY();
	    padsizex = 2.;
	    padsizey = 0.5;           
	  }
	}
        else {
          Fatal("CbmTrdDigitizer","This case should never happen.");
	}
	
        cout<<"SizeX: "<<sizex<<endl;
        cout<<"SizeY: "<<sizey<<endl;
        cout<<"PadSizeX: "<<padsizex<<endl;
        cout<<"PadSizeY: "<<padsizey<<endl;


  }

}
// --------------------------------------------------------------------

// --------------------------------------------------------------------

// ---- Finish --------------------------------------------------------
void CbmTrdDigitizer::Finish()
{
}
// --------------------------------------------------------------------

void CbmTrdDigitizer::GetModuleInformation()
{

  // Extract the information about station, layer, module type
  // and cpoy number of the module from the full path to the
  // node.
  // The full path is tokenized at the "/" which diide the different
  // levels of the geometry.
  // Knowing the nameing scheme of the volumes one gets the required
  // information with simple string manipulation.
  // This is probably not the fastes way, but the speed has to be checked.

  TString path = gGeoManager->GetPath();
  cout<<"Path: "<<path<<endl;
  TStringToken* bla = new TStringToken(path,"/");

  while (bla->NextToken()) {
    if (bla->Contains("layer")) {
      TString bla3 = (TString) *bla;
      Ssiz_t pos = bla3.Last('_');
      Ssiz_t substringLength=bla3.Length()-pos-1;
      TString bla2 = bla3((bla3.Last('_')+1),substringLength);
      TString bla1 = bla3(3,1);
      fStation=bla1.Atoi();
      fLayer=bla2.Atoi();
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
// ---- Register ------------------------------------------------------
void CbmTrdDigitizer::Register(){

  //FairRootManager::Instance()->Register("TrdDigi","Trd Digi", fDigiCollection, kTRUE);

}
// --------------------------------------------------------------------

ClassImp(CbmTrdDigitizer)
