#ifndef CBMLITTRACKPARAM_H_
#define CBMLITTRACKPARAM_H_

#include "TMatrixDSym.h"
#include "TVectorD.h"

#include <vector>
#include <string>

class CbmLitTrackParam
{
public:
	CbmLitTrackParam();
	virtual ~CbmLitTrackParam();

	double GetX() const { return fX; }
	double GetY() const { return fY; }
	double GetZ() const { return fZ; }
	double GetTx() const { return fTx; }
	double GetTy() const { return fTy; }
	double GetQp() const { return fQp; }
	void GetDirCos(double& nx, double& ny, double& nz) const;
	std::vector<double> GetStateVector() const;
	const std::vector<double>& GetCovMatrix() const { return fCovMatrix; }
	double GetCovariance(int index) const { return fCovMatrix[index]; }
	void GetStateVector(TVectorD& x) const;
	void GetCovMatrix(TMatrixDSym& C) const;

	void SetX(double x) { fX  = x;  }
	void SetY(double y) { fY  = y;  }
	void SetZ(double z) { fZ  = z;  }
	void SetTx(double tx) { fTx = tx; }
	void SetTy(double ty) { fTy = ty; }
	void SetQp(double qp) { fQp = qp; }
	void SetStateVector(const std::vector<double>& x);
	void SetCovMatrix(const std::vector<double>& C) {
		fCovMatrix.assign(C.begin(), C.end());
	}
	void SetCovariance(int index, double cov) { fCovMatrix[index] = cov; }
	void SetStateVector(const TVectorD& x);
	void SetCovMatrix(const TMatrixDSym& cov);

	std::string ToString() const;

private:

	double fX, fY, fZ;
	double fTx, fTy;
	double fQp;

//  Upper triangle symmetric matrix
//  a[0,0..4], a[1,1..4], a[2,2..4], a[3,3..4], a[4,4].
	std::vector<double> fCovMatrix;
};

#endif /*CBMLITTRACKPARAM_H_*/
