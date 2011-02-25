#ifndef CBMLITTRACKPARAM_H_
#define CBMLITTRACKPARAM_H_

#include "base/CbmLitFloat.h"

#include <vector>
#include <string>

class CbmLitTrackParam
{
public:
	CbmLitTrackParam();
	virtual ~CbmLitTrackParam();

	myf GetX() const { return fX; }
	myf GetY() const { return fY; }
	myf GetZ() const { return fZ; }
	myf GetTx() const { return fTx; }
	myf GetTy() const { return fTy; }
	myf GetQp() const { return fQp; }
	void GetDirCos(myf& nx, myf& ny, myf& nz) const;
	std::vector<myf> GetStateVector() const;
	const std::vector<myf>& GetCovMatrix() const { return fCovMatrix; }
	myf GetCovariance(int index) const { return fCovMatrix[index]; }

	void SetX(myf x) { fX  = x;  }
	void SetY(myf y) { fY  = y;  }
	void SetZ(myf z) { fZ  = z;  }
	void SetTx(myf tx) { fTx = tx; }
	void SetTy(myf ty) { fTy = ty; }
	void SetQp(myf qp) { fQp = qp; }
	void SetStateVector(const std::vector<myf>& x);
	void SetCovMatrix(const std::vector<myf>& C) {
		fCovMatrix.assign(C.begin(), C.end());
	}
	void SetCovariance(int index, myf cov) { fCovMatrix[index] = cov; }

	std::string ToString() const;

private:
	myf fX, fY, fZ;
	myf fTx, fTy;
	myf fQp;

//  Upper triangle symmetric matrix
//  a[0,0..4], a[1,1..4], a[2,2..4], a[3,3..4], a[4,4].
	std::vector<myf> fCovMatrix;
};

#endif /*CBMLITTRACKPARAM_H_*/
