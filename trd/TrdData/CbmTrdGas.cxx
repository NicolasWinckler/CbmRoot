#include "CbmTrdGas.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoBBox.h"
#include "TGeoMaterial.h"
#include "TObjArray.h"
#include "TMath.h"

#include <iostream>
using std::cout;
using std::endl;


CbmTrdGas* CbmTrdGas::fgInstance = 0;


CbmTrdGas::CbmTrdGas()
  : TObject(),
    fDetType(-1),
    fGasThick(0.),
    fPercentNobleGas(0.),
    fPercentCO2(0.),  
    fNobleGasType(-1),
    fFileNameLike(""),
    fFileNameANN("")
{
  if (fgInstance) {
    Fatal("CbmTrdGas", "Singleton instance already exists.");
    return;
  }

  fgInstance = this;
}

void CbmTrdGas::Init() {

    cout << "********** Initilization of TRD Gas ************ " << endl; 

    // Read MWPC gas properties from geometry. This enforce that gas 
    // mixture simulation and reconstruction is the same. This is important
    // because dE/dx is calculated in the simulation and TR is calculated
    // in the reconstruction

    // get pointer to gas volume
    TGeoVolume *fm=NULL;
    fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject("trd1gas");
    if (!fm) fm = (TGeoVolume *)gGeoManager->
                  GetListOfVolumes()->FindObject("trd1mod1gas");
    if (!fm){
      TObjArray* volList = gGeoManager->GetListOfVolumes();
      TObjArrayIter iter(volList);
      TGeoVolume* vol   = NULL;
      cout << "********** List of available volumes ************ " << endl; 
      while( (vol = (TGeoVolume*)iter.Next()) ) {
        cout << vol->GetName() << endl; 
      }
      cout << "***************** End of List ******************* " << endl; 
      cout << " -E- Could not find volume trd1gas or trd1mod1gas. " << endl;
      cout << " -E- If there is no list above this text then probably "<< endl;
      cout << " -E- the geometry was not loaded in the macro." << endl;
      cout << " -E- Please do it with fRun->LoadGeometry().  " << endl;
      cout << " -E- If you see a list probably the names of the " << endl;
      cout << " -E- volumes have changed and CbmTrdRadiator has to " << endl;
      cout << " -E- be changed accordingly. " << endl;
      Fatal("CbmTrdGas","No volumes defined.");;
    } 

    // check if detector is of GSI or Muenster/Bucarest type
    // the type is coded in the master volume name
    TGeoVolume *fm1 = NULL;
    fm1 = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject("trd1mb");

    // get pointer to shape of gas volume
    TGeoBBox *shape = (TGeoBBox *)fm->GetShape(); 
    
    if (fm1) { //MB type
      fDetType = 1;
      // only halve thickness of gas layer tacken into account because the
      // total absorbed TR is calculated in two steps since the real chamber
      // consists of two gas volumes with both halve the thickness as from
      // geometry
      fGasThick = 2 * ( shape->GetDZ() ); 
      cout << " -I- Detector type : double sided geometry (1) "<< endl;
      cout << " -I- Gas thickness : " << fGasThick << " cm"<<endl;
    } else { //GSI type
      fDetType = 0;
      fGasThick = 2 * ( shape->GetDZ() );
      cout << " -I- Detector type : standard GSI geometry (2) "<< endl;
      cout << " -I- Gas thickness : " << fGasThick << " cm" << endl;
    }


    // Get all the necessary properties of the gas mixture
    TGeoMixture *mixt = (TGeoMixture*)fm->GetMaterial();
    Int_t nmixt = mixt->GetNelements();
    if (nmixt != 3) {
      cout << "-E- This is not a mixture composed out of three" << endl;
      cout << "-E- different elements. Don't know what to do, so" << endl;
      cout << "-E- stop execution here." << endl;
      Fatal("CbmTrdGas","Unknown gas mixture.");;
    }    
    Bool_t foundCarbon=kFALSE;
    Bool_t foundOxygen=kFALSE;   
    Int_t carbon  =0;
    Int_t oxygen  =0;
    Int_t noblegas=0;

    Double_t *elem   = mixt->GetZmixt();
    Double_t *weight = mixt->GetWmixt();
    Double_t *amixt  = mixt->GetAmixt();

    for (Int_t i=0 ; i<nmixt; i++){
      if (elem[i] == 6.0) {
	carbon=i;
	foundCarbon=kTRUE;
      }
      else if (elem[i] == 8.0) {
	oxygen=i;
	foundOxygen=kTRUE;
      }
      else noblegas=i;
    }
    if (!(foundCarbon && foundOxygen)) {
      cout << "-E- This gas mixture has no CO2 admixture" << endl;
      cout << "-E- If you want to use this mixture you have to change" << endl;
      cout << "-E- CbmTrdRadiator to be consistent" << endl;
      Fatal("CbmTrdGas","Unknown gas mixture.");;
    }
    if (elem[noblegas] != 54) {
      cout << "-E- This gas mixture has no Xe admixture" << endl;
      cout << "-E- If you want to use this mixture you have to change" << endl;
      cout << "-E- CbmTrdRadiator to be consistent" << endl;
      Fatal("CbmTrdGas","Unknown gas mixture.");;
    } else {
      fNobleGasType=1;
    }


    Double_t massC =amixt[carbon];
    Double_t massO =amixt[oxygen];
    Double_t massXe=amixt[noblegas];
    Double_t x     =weight[noblegas];
    Double_t percentNoblegas=100*(((massC*x)+(2*massO*x))/
			      (massXe+massC*x+2*massO*x-massXe*x));

    
    fPercentNobleGas = TMath::Nint(percentNoblegas)/100.;
    fPercentCO2 = 1-fPercentNobleGas;

    if (elem[noblegas] == 54) cout << " -I- Percent  (Xe) : " <<(fPercentNobleGas*100) <<endl;
    cout << " -I- Percent (CO2) : " <<(fPercentCO2*100) <<endl;

    cout << "********** End of TRD Gas Init ************ " << endl; 

    SetFileName();

}

TString CbmTrdGas::GetFileName(TString method) const {

  if (method.Contains("Like")) {
    return fFileNameLike;
  }  else if (method.Contains("ANN")) {
    return fFileNameANN;
  } else {
    cout <<" -E- CbmTrdGas::GetFileName" << endl;
    cout <<" -E- Electron ID method "<<method<<" not known."<< endl;
    cout <<" -E- The input must be either Like or ANN"<<endl;
    TString returnvalue ="";
    return returnvalue;
  }
}

void CbmTrdGas::SetFileName() {

  Int_t fraction =  TMath::Nint(fPercentNobleGas*100);
  Int_t thickness =  TMath::Nint(fGasThick*10);

  const char* detector = "";
  if (fDetType == 0) {
    detector ="GSI";
  } else if (fDetType == 1){
    detector ="MB"; 
  } else {
    cout <<"Detector type "<<fDetType<<" not known"<< endl;
    cout <<"Stop execution of program due to initilization error."<< endl;
    Fatal("CbmTrdGas","Unknown detector type.");
  }
  const char* gastype = "";
  if (fNobleGasType == 1) {
    gastype = "Xenon";
    //  } else if (GasType == 2){
    //    gastype = "Argon"; 
  } else {
    cout <<"Gas type "<<fNobleGasType<<" not known"<< endl;
    cout <<"Stop execution of program due to initilization error."<< endl;
    Fatal("CbmTrdGas","Unknown gas type.");
  }

  //  TString InFile;
  TString path = getenv("VMCWORKDIR");
  path = path+"/parameters/trd/";
  fFileNameLike.Form("Likelihood_%s_%d_%s_%d.root",gastype,fraction,detector,thickness);
  fFileNameANN.Form("ANN_%s_%d_%s_%d.root",gastype,fraction,detector,thickness);

  fFileNameLike=path+fFileNameLike;
  fFileNameANN=path+fFileNameANN;

}


ClassImp(CbmTrdGas)