#ifndef CbmTrdDigi_HH
#define CbmTrdDigi_HH

#include <iostream>
#include <vector>
#include "Rtypes.h"

class CbmTrdDigi
{
	public :  CbmTrdDigi(){
			fCol = -1;
			fRow = -1;
			fCharge = -1.0;
			fDetId = -1;
                       
		  };
		  
		  CbmTrdDigi(Int_t detId, Int_t col, Int_t row, Double_t charge, int index=-1){
		  	fDetId = detId;
			fCol = col;
			fRow = row;
			fCharge = charge;
			if (index >= 0)
				fMCIndex.push_back(index);
		  };
		  
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
		  
		  friend std::ostream& operator<< (std::ostream& out, CbmTrdDigi pixel)
		  {
			out << "Detector: " << pixel.GetDetId() << " Pixel (C/R): " << pixel.GetCol() << " " << pixel.GetRow()
			    << " Charge: " << pixel.GetCharge() << " from MCHit: " << pixel.GetFirstMCIndex();
			return out;
		  };	

	
	private : 
		  Int_t fCol;
		  Int_t fRow;
		  Double_t fCharge;
		  Int_t fDetId;
		  std::vector<int> fMCIndex;		///< List of indices of the corresponding MC hits
};


#endif
