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

#include <iostream>

using std::cout;
using std::endl;

// -----   Standard constructor   ------------------------------------------
CbmTrdDigiPar::CbmTrdDigiPar(const char* name, 
			     const char* title,
			     const char* context)
  : FairParGenericSet(name, title, context) 
{
  detName="Trd";
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmTrdDigiPar::~CbmTrdDigiPar() 
{
  clear();
}
// -------------------------------------------------------------------------


// -----   Public method clear   -------------------------------------------
void CbmTrdDigiPar::clear() 
{
  status = kFALSE;
  resetInputVersions();
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------

void CbmTrdDigiPar::putParams(FairParamList* l) 
{

  //  cout << " I am in CbmTrdDigiPar::putParams " << endl;
  if (!l) return;

   l->add("NrOfModules", fNrOfModules);
   l->add("MaxSectors", fMaxSectors);
   l->add("ModuleIdArray", fModuleIdArray);
   l->add("ModuleRotArray", fModuleRotArray);

   //   TArrayD values(7);
   // Instead of a fixed number of values the number of values to
   // store now depends on the maximum number of sectors per module
   Int_t nrValues = 5 + ( fMaxSectors * 4 );
   TArrayD values(nrValues);
 
   for (Int_t i=0; i < fNrOfModules; i++){
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetX(),0);         
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetY(),1);          
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetZ(),2);          
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSizex(),3);      
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSizey(),4); 
     for (Int_t j=0; j < fMaxSectors; j++){       
       Int_t k=5+(j*4);
       values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSectorSizex(j),k);   
       values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSectorSizey(j),k+1);   
       values.AddAt(fModuleMap[fModuleIdArray[i]]->GetPadSizex(j),k+2);   
       values.AddAt(fModuleMap[fModuleIdArray[i]]->GetPadSizey(j),k+3);   
     }
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
  if ( ! l->fill("MaxSectors", &fMaxSectors) ) return kFALSE;
  
  fModuleIdArray.Set(fNrOfModules);
  fModuleRotArray.Set(fNrOfModules);
  if ( ! l->fill("ModuleIdArray", &fModuleIdArray) ) return kFALSE;
  if ( ! l->fill("ModuleRotArray", &fModuleRotArray) ) return kFALSE;
  
  //TArrayD *values = new TArrayD(7);
  // Instead of a fixed number of values the number of values to
  // store now depends on the maximum number of sectors per module
  Int_t nrValues = 5 + ( fMaxSectors * 4 );
  TArrayD *values = new TArrayD(nrValues);
  TArrayD sectorSizeX(fMaxSectors);
  TArrayD sectorSizeY(fMaxSectors);
  TArrayD padSizeX(fMaxSectors);
  TArrayD padSizeY(fMaxSectors);
  Double_t x;
  Double_t y;
  Double_t z;
  Double_t sizex;
  Double_t sizey;

  TString text;
  for (Int_t i=0; i < fNrOfModules; i++){
    text="";
    text += fModuleIdArray[i];
    if ( ! l->fill(text.Data(), values) ) return kFALSE;
    Int_t VolumeID = text.Atoi();
    x=values->At(0);
    y=values->At(1);
    z=values->At(2);
    sizex= values->At(3);
    sizey= values->At(4);
    for (Int_t j=0; j < fMaxSectors; j++){       
      Int_t k=5+(j*4);
      sectorSizeX.AddAt(values->At(k),j);
      sectorSizeY.AddAt(values->At(k+1),j);
      padSizeX.AddAt(values->At(k+2),j);
      padSizeY.AddAt(values->At(k+3),j);
    }

    fModuleMap[VolumeID] = new CbmTrdModule(VolumeID, x, y, z,
					    sizex, sizey, fMaxSectors,
					    sectorSizeX, sectorSizeY,
					    padSizeX, padSizeY,
					    (Bool_t)fModuleRotArray[i]);
  }

    return kTRUE;
}


ClassImp(CbmTrdDigiPar)
