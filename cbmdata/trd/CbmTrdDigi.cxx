#include "CbmTrdDigi.h"

#include <iostream>
using std::cout;
using std::endl;

CbmTrdDigi::CbmTrdDigi() 
  : TObject(),
    fDetId(-1),
    fCol(-1),
    fRow(-1),
    fCharge(-1.0),
    fTime(-1.0),
    fMCIndex(0)
{
}

CbmTrdDigi::CbmTrdDigi(Int_t detId, Int_t col, Int_t row, Double_t charge, 
                       Double_t time, Int_t index)
  : TObject(),
    fDetId(detId),
    fCol(col),
    fRow(row),
    fCharge(charge),
    fTime(time),
    fMCIndex()
{
  if (index >= 0)
          fMCIndex.push_back(index);
};
CbmTrdDigi::~CbmTrdDigi()
{
  fMCIndex.clear();
}
void CbmTrdDigi::Print() {
  cout<<"fDetID : "<<fDetId <<endl;
  cout<<"Col    : "<<fCol <<endl;
  cout<<"Row    : "<<fRow <<endl;
  cout<<"ELoss  : "<<fCharge <<endl;
  cout<<"Time   : "<<fTime <<endl;
}

ClassImp(CbmTrdDigi)
