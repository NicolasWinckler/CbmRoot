#include "CbmLitTrackParam.h"

#include "TVector3.h"
#include "TMatrixDSym.h"
#include "TVectorD.h"

#include <sstream>
#include <cmath>

CbmLitTrackParam::CbmLitTrackParam():
	fX(0.),
	fY(0.),
	fZ(0.),
	fTx(0.),
	fTy(0.),
	fQp(0.)
{
	fCovMatrix.resize(15);
}

CbmLitTrackParam::~CbmLitTrackParam()
{
}

void CbmLitTrackParam::GetDirCos(
		Double_t& nx,
		Double_t& ny,
		Double_t& nz) const
{
	Double_t p  = (std::abs(fQp) != 0.) ? 1./std::abs(fQp) : 1.e20;
	Double_t pz = std::sqrt(p*p / (fTx*fTx + fTy*fTy + 1));
	Double_t px = fTx * pz;
	Double_t py = fTy * pz;
	TVector3 unit = TVector3(px, py, pz).Unit();
	nx = unit.X();
	ny = unit.Y();
	nz = unit.Z();
}

void CbmLitTrackParam::GetCovMatrix(
		TMatrixDSym& C) const
{
	C[0][0] = GetCovariance(0);
	C[0][1] = C[1][0] = GetCovariance(1);
	C[0][2] = C[2][0] = GetCovariance(2);
	C[0][3] = C[3][0] = GetCovariance(3);
	C[0][4] = C[4][0] = GetCovariance(4);

	C[1][1] = GetCovariance(5);
	C[1][2] = C[2][1] = GetCovariance(6);
	C[1][3] = C[3][1] = GetCovariance(7);
	C[1][4] = C[4][1] = GetCovariance(8);

	C[2][2] = GetCovariance(9);
	C[2][3] = C[3][2] = GetCovariance(10);
	C[2][4] = C[4][2] = GetCovariance(11);

	C[3][3] = GetCovariance(12);
	C[3][4] = C[4][3] = GetCovariance(13);

	C[4][4] = GetCovariance(14);
}

void CbmLitTrackParam::GetStateVector(
		TVectorD& x) const
{
	x[0] = GetX();
	x[1] = GetY();
	x[2] = GetTx();
	x[3] = GetTy();
	x[4] = GetQp();
}

std::vector<Double_t> CbmLitTrackParam::GetStateVector() const {
	std::vector<Double_t> state(5);
	state[0] = GetX();
	state[1] = GetY();
	state[2] = GetTx();
	state[3] = GetTy();
	state[4] = GetQp();
	return state;
}

void CbmLitTrackParam::SetCovMatrix(
		const TMatrixDSym& C)
{
	SetCovariance(0, C[0][0]);
	SetCovariance(1, C[0][1]);
	SetCovariance(2, C[0][2]);
	SetCovariance(3, C[0][3]);
	SetCovariance(4, C[0][4]);

	SetCovariance(5, C[1][1]);
	SetCovariance(6, C[1][2]);
	SetCovariance(7, C[1][3]);
	SetCovariance(8, C[1][4]);

	SetCovariance(9, C[2][2]);
	SetCovariance(10, C[2][3]);
	SetCovariance(11, C[2][4]);

	SetCovariance(12, C[3][3]);
	SetCovariance(13, C[3][4]);

	SetCovariance(14, C[4][4]);
}

void CbmLitTrackParam::SetStateVector(
		const TVectorD& x)
{
	SetX(x[0]);
	SetY(x[1]);
	SetTx(x[2]);
	SetTy(x[3]);
	SetQp(x[4]);
}

void CbmLitTrackParam::SetStateVector(
		const std::vector<Double_t>& x)
{
	SetX(x[0]);
	SetY(x[1]);
	SetTx(x[2]);
	SetTy(x[3]);
	SetQp(x[4]);
}

std::string CbmLitTrackParam::ToString() const
{
	std::stringstream ss;
	ss << "TrackParam: pos=(" << fX << "," << fY << "," << fZ
    	      << ") tx=" << fTx << " ty=" << fTy << " qp=" << fQp << std::endl;
//	ss << "cov: ";
//	for (Int_t i = 0; i < 15; i++) ss << fCovMatrix[i] << " ";
//	ss << std::endl;
	return ss.str();
}
