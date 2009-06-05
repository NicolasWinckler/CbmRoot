#include "CbmTrdDigi.h"

#include <iostream>
using std::cout;
using std::endl;

CbmTrdDigi::CbmTrdDigi() {
  fCol = -1;
  fRow = -1;
  fCharge = -1.0;
  fDetId = -1;
}

CbmTrdDigi::CbmTrdDigi(Int_t detId, Int_t col, Int_t row, Double_t charge, 
                       Int_t index){
  fDetId = detId;
  fCol = col;
  fRow = row;
  fCharge = charge;
  if (index >= 0)
          fMCIndex.push_back(index);
};

void CbmTrdDigi::Print() {
  cout<<"Col : "<<fCol <<endl;
  cout<<"Row : "<<fRow <<endl;
  cout<<"ELoss : "<<fCharge <<endl;
  cout<<"fDetID : "<<fDetId <<endl;
}

ClassImp(CbmTrdDigi)
