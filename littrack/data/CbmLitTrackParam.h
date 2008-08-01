#ifndef CBMLITTRACKPARAM_H_
#define CBMLITTRACKPARAM_H_

#include "TObject.h"
#include "TMatrixDSym.h"
#include "TVectorD.h"

#include <vector>

class CbmLitTrackParam
{
public:
	CbmLitTrackParam();
	virtual ~CbmLitTrackParam();

	Double_t GetX() const { return fX; }
	Double_t GetY() const { return fY; }
	Double_t GetZ() const { return fZ; }
	Double_t GetTx() const { return fTx; }
	Double_t GetTy() const { return fTy; }
	Double_t GetQp() const { return fQp; }
	void GetDirCos(Double_t& nx, Double_t& ny, Double_t& nz) const;
	std::vector<Double_t> GetStateVector() const;
	const std::vector<Double_t>& GetCovMatrix() const { return fCovMatrix; }
	Double_t GetCovariance(Int_t index) const { return fCovMatrix[index]; }
	void GetStateVector(TVectorD& x) const;
	void GetCovMatrix(TMatrixDSym& C) const;	
	
	void SetX(Double_t x) { fX  = x;  }
	void SetY(Double_t y) { fY  = y;  }
	void SetZ(Double_t z) { fZ  = z;  }
	void SetTx(Double_t tx) { fTx = tx; }
	void SetTy(Double_t ty) { fTy = ty; }
	void SetQp(Double_t qp) { fQp = qp; }
	void SetStateVector(const std::vector<Double_t>& x);
	void SetCovMatrix(const std::vector<Double_t>& C) {
		fCovMatrix.assign(C.begin(), C.end());
	}
	void SetCovariance(Int_t index, Double_t cov) { fCovMatrix[index] = cov; }
	void SetStateVector(const TVectorD& x);
	void SetCovMatrix(const TMatrixDSym& cov);
		
	void Print() const;

private:

	Double_t fX, fY, fZ;
	Double_t fTx, fTy;
	Double_t fQp;

//  Upper triangle symmetric matrix 
//  a[0,0..4], a[1,1..4], a[2,2..4], a[3,3..4], a[4,4].
	std::vector<Double_t> fCovMatrix;
	
	ClassDef(CbmLitTrackParam, 1);
};

#endif /*CBMLITTRACKPARAM_H_*/
