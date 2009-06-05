#ifndef CbmTrdDigi_HH
#define CbmTrdDigi_HH

#include "TObject.h"

#include <iostream>
#include <vector>
#include "Rtypes.h"

class CbmTrdDigi : public TObject
{
  public:  
    CbmTrdDigi();
    CbmTrdDigi(Int_t detId, Int_t col, Int_t row, Double_t charge, Int_t index=-1);
    void SetCol(Int_t col) {fCol = col;};
    void SetRow(Int_t row) {fRow = row;};
    void SetCharge(Double_t charge) {fCharge = charge;};
    void SetDetId(Int_t detId) {fDetId = detId;};
		  
    Int_t GetCol() const {return fCol;};
    Int_t GetRow() const {return fRow;};
    Double_t GetCharge() const {return fCharge;};
    Int_t GetDetId() const {return fDetId;};

    std::vector<int> GetMCIndex(){return fMCIndex;};
    int GetFirstMCIndex(){return fMCIndex[0];};
		  
    void AddCharge(Double_t charge){fCharge+= charge;};
    void AddMCIndex(int i){fMCIndex.push_back(i);};
		
    void Print();
  
  private: 
    Int_t fCol;
    Int_t fRow;
    Double_t fCharge;
    Int_t fDetId;
    std::vector<int> fMCIndex;///< List of indices of the corresponding MC hits

  ClassDef(CbmTrdDigi,1);
};


#endif
