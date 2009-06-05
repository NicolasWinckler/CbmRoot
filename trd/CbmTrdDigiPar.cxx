// -------------------------------------------------------------------------
// -----                CbmTrdDigiPar source file              -----
// -----                  Created 05/06/08  by F. Uhlig                -----
// -------------------------------------------------------------------------

#include "CbmTrdDigiPar.h"
#include "CbmTrdModule.h"

#include "FairParamList.h"
#include "FairDetParIo.h"
#include "FairParIo.h"

#include "TString.h"
#include "TMath.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Standard constructor   ------------------------------------------
CbmTrdDigiPar::CbmTrdDigiPar(const char* name, 
					     const char* title,
					     const char* context)
  : FairParGenericSet(name, title, context) {
  detName="Trd";
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdDigiPar::~CbmTrdDigiPar() {
  clear();
}
// -------------------------------------------------------------------------


// -----   Public method clear   -------------------------------------------
void CbmTrdDigiPar::clear() {
  status = kFALSE;
  resetInputVersions();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------

void CbmTrdDigiPar::putParams(FairParamList* l) {

  //  cout << " I am in CbmTrdDigiPar::putParams " << endl;
  if (!l) return;

   l->add("NrOfModules", fNrOfModules);
   l->add("ModuleIdArray", fModuleIdArray);
   l->add("ModuleRotArray", fModuleRotArray);

   TArrayD values(7);
 
   for (Int_t i=0; i < fNrOfModules; i++){
     //     Int_t j = fModuleIdArray[i];
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetX(),0);         
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetY(),1);          
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetZ(),2);          
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSizex(),3);      
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSizey(),4);      
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetPadSizex(),5);   
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetPadSizey(),6);   
     TString text;
     text += fModuleIdArray[i];
     l->add(text.Data(), values);
   }


}

//------------------------------------------------------

Bool_t CbmTrdDigiPar::getParams(FairParamList* l) {

  //    cout << " I am in CbmTrdDigiPar::getParams " << endl;

    if (!l) return kFALSE;
    
    if ( ! l->fill("NrOfModules", &fNrOfModules) ) return kFALSE;
    
    fModuleIdArray.Set(fNrOfModules);
    fModuleRotArray.Set(fNrOfModules);
    if ( ! l->fill("ModuleIdArray", &fModuleIdArray) ) return kFALSE;
    if ( ! l->fill("ModuleRotArray", &fModuleRotArray) ) return kFALSE;
      
    TArrayD *values = new TArrayD(7);
    TString text;
    for (Int_t i=0; i < fNrOfModules; i++){
      text="";
      text += fModuleIdArray[i];
      if ( ! l->fill(text.Data(), values) ) return kFALSE;
      Int_t VolumeID = text.Atoi();
      //      cout <<"VolID: "<<VolumeID<<endl;
      fModuleMap[VolumeID] = new CbmTrdModule(VolumeID, values->At(0), 
                  values->At(1), values->At(2), values->At(3), values->At(4), 
                  values->At(5), values->At(6), (Bool_t)fModuleRotArray[i]);
    }

    return kTRUE;
}



ClassImp(CbmTrdDigiPar)
