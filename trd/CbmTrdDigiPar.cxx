// -------------------------------------------------------------------------
// -----                CbmTrdDigiPar source file              -----
// -----                  Created 05/06/08  by F. Uhlig                -----
// -------------------------------------------------------------------------

#include "CbmTrdDigiPar.h"
#include "CbmTrdModule.h"

#include "FairParamList.h"
#include "FairDetParIo.h"
#include "FairParIo.h"
#include "FairLogger.h"

#include "TString.h"

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
  FairLogger *logger = FairLogger::GetLogger();
  logger->Debug4(MESSAGE_ORIGIN, "Enter CbmTrdDigiPar::~CbmTrdDigiPar");
  for (fModuleMapIt = fModuleMap.begin(); fModuleMapIt != fModuleMap.end(); 
       ++fModuleMapIt) {
    delete fModuleMapIt->second;
  }      
  fModuleMap.clear();
  clear();
  logger->Debug4(MESSAGE_ORIGIN, "Leave CbmTrdDigiPar::~CbmTrdDigiPar");
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

  if (!l) return;

   l->add("NrOfModules", fNrOfModules);
   l->add("MaxSectors", fMaxSectors);
   l->add("ModuleIdArray", fModuleIdArray);

   // Instead of a fixed number of values the number of values to
   // store now depends on the maximum number of sectors per module
   // The first six parameters are for the complete module. The rest
   // of the parameters depend on the number of sectors.
   // The parametrs are:
   // X. Y, Z             : position of the middle of the gaslayer.
   // SizeX, SizeY, SizeZ : size of the gaslayer. The values are only the
   //                       half size which are the values returned by
   //                       geant.
   // SectroSizeX(Y)      : size of a sector
   // PadSizeX(Y)         : size of the pads in this sector

   Int_t nrValues = 6 + ( fMaxSectors * 4 );
   TArrayD values(nrValues);
 
   for (Int_t i=0; i < fNrOfModules; i++){
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetX(),0);         
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetY(),1);          
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetZ(),2);          
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSizex(),3);      
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSizey(),4); 
     values.AddAt(fModuleMap[fModuleIdArray[i]]->GetSizez(),5); 
     for (Int_t j=0; j < fMaxSectors; j++){       
       Int_t k=6+(j*4);
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

  if (!l) return kFALSE;
  
  if ( ! l->fill("NrOfModules", &fNrOfModules) ) return kFALSE;
  if ( ! l->fill("MaxSectors", &fMaxSectors) ) return kFALSE;
  
  fModuleIdArray.Set(fNrOfModules);
  if ( ! l->fill("ModuleIdArray", &fModuleIdArray) ) return kFALSE;
  
  // Instead of a fixed number of values the number of values to
  // store now depends on the maximum number of sectors per module
  Int_t nrValues = 6 + ( fMaxSectors * 4 );
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
  Double_t sizez;

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
    sizez= values->At(5);
    for (Int_t j=0; j < fMaxSectors; j++){       
      Int_t k=6+(j*4);
      sectorSizeX.AddAt(values->At(k),j);
      sectorSizeY.AddAt(values->At(k+1),j);
      padSizeX.AddAt(values->At(k+2),j);
      padSizeY.AddAt(values->At(k+3),j);
    }

    fModuleMap[VolumeID] = new CbmTrdModule(VolumeID, x, y, z,
					    sizex, sizey, sizez, fMaxSectors,
					    sectorSizeX, sectorSizeY,
					    padSizeX, padSizeY);
  }

  delete values;
  return kTRUE;
}


ClassImp(CbmTrdDigiPar)
