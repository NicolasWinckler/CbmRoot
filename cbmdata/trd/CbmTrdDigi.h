#ifndef CbmTrdDigi_HH
#define CbmTrdDigi_HH

#include "TObject.h"

#include <vector>

class CbmTrdDigi : public TObject
{
  public:  
    CbmTrdDigi();
    CbmTrdDigi(Int_t detId, Int_t col, Int_t row, Double_t charge, 
               Double_t time, Int_t index=-1);
    ~CbmTrdDigi();
   
    void SetDetId(Int_t detId) {fDetId = detId;};
    void SetCol(Int_t col) {fCol = col;};
    void SetRow(Int_t row) {fRow = row;};
    void SetCharge(Double_t charge) {fCharge = charge;};
    void SetTime(Double_t time) {fTime = time;};
		  
    Int_t GetDetId() const {return fDetId;};
    Int_t GetCol() const {return fCol;};
    Int_t GetRow() const {return fRow;};
    Double_t GetCharge() const {return fCharge;};
    Double_t GetTime() const {return fTime;};

    std::vector<int> GetMCIndex(){return fMCIndex;};
    int GetFirstMCIndex(){return fMCIndex[0];};
		  
    void AddCharge(Double_t charge){fCharge+= charge;};
    void AddMCIndex(int i){fMCIndex.push_back(i);};
		
    void Print();
  
  private: 
    Int_t fDetId;
    Int_t fCol;
    Int_t fRow;
    Double_t fCharge;
    Double_t fTime;

    std::vector<int> fMCIndex;//! List of indices of the corresponding MC hits

  ClassDef(CbmTrdDigi,2);
};


#endif
