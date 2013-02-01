#include "CbmTofDigiPar.h"

#include "CbmTofCell.h"

#include "FairParamList.h"

#include <iostream>

ClassImp(CbmTofDigiPar)

CbmTofDigiPar::CbmTofDigiPar(const char* name,
    const char* title,
    const char* context)
  : FairParGenericSet(name,title,context),
    fCellMap(),
    fCellMapIt(),
    fCellIdArray(),
    fNrOfCells(-1)
{
	detName="Tof";
}

CbmTofDigiPar::~CbmTofDigiPar(void)
{
	  LOG(DEBUG4)<<"Enter CbmTrdDigiPar destructor"<< FairLogger::endl;
	  for (fCellMapIt = fCellMap.begin(); fCellMapIt != fCellMap.end();
	       ++fCellMapIt) {
	    delete fCellMapIt->second;
	  }
	  fCellMap.clear();
	  clear();
	  LOG(DEBUG4)<<"Leave CbmTrdDigiPar destructor"<< FairLogger::endl;
}

void CbmTofDigiPar::clear(void)
{
	  status = kFALSE;
	  resetInputVersions();
}

void CbmTofDigiPar::putParams(FairParamList* l)
{
  if (!l) { return; }

  l->add("NrOfCells", fNrOfCells);
  l->add("CellIdArray", fCellIdArray);

  // The parameters are:
  // X. Y, Z             : position of the middle of the 8 gaps belonging to one cell.
  // SizeX, SizeY        : size of the cell. The values are only the
  //                       half size which are the values returned by
  //                       geant.

  Int_t nrValues = 5;
  TArrayD values(nrValues);

  for (Int_t i=0; i < fNrOfCells; i++){
    values.AddAt(fCellMap[fCellIdArray[i]]->GetX(),0);
    values.AddAt(fCellMap[fCellIdArray[i]]->GetY(),1);
    values.AddAt(fCellMap[fCellIdArray[i]]->GetZ(),2);
    values.AddAt(fCellMap[fCellIdArray[i]]->GetSizex(),3);
    values.AddAt(fCellMap[fCellIdArray[i]]->GetSizey(),4);

    TString text;
    text += fCellIdArray[i];
    l->add(text.Data(), values);
  }


}

Bool_t CbmTofDigiPar::getParams(FairParamList* l)
{
  if (!l) { return kFALSE; }

  if ( ! l->fill("NrOfCells", &fNrOfCells) ) return kFALSE;

  fCellIdArray.Set(fNrOfCells);
  if ( ! l->fill("CellIdArray", &fCellIdArray) ) return kFALSE;

  Int_t nrValues = 5;
  TArrayD *values = new TArrayD(nrValues);

  Double_t x;
  Double_t y;
  Double_t z;
  Double_t sizex;
  Double_t sizey;

  TString text;
  for (Int_t i=0; i < fNrOfCells; i++){
    text="";
    text += fCellIdArray[i];
    if ( ! l->fill(text.Data(), values) ) return kFALSE;
    Int_t VolumeID = text.Atoi();
    x=values->At(0);
    y=values->At(1);
    z=values->At(2);
    sizex= values->At(3);
    sizey= values->At(4);

    fCellMap[VolumeID] = new CbmTofCell(VolumeID, x, y, z,
                                            sizex, sizey);
  }

  delete values;


  return kTRUE;
}
