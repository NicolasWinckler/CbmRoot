#include "data/CbmLitTrackParam.h"

#include "TVector3.h"

#include <sstream>
#include <cmath>

CbmLitTrackParam::CbmLitTrackParam():
	fX(0.),
	fY(0.),
	fZ(0.),
	fTx(0.),
	fTy(0.),
	fQp(0.),
	fCovMatrix(15, 0.)
{
}

CbmLitTrackParam::~CbmLitTrackParam()
{
}

void CbmLitTrackParam::GetDirCos(
		myf& nx,
		myf& ny,
		myf& nz) const
{
	myf p  = (std::abs(fQp) != 0.) ? 1./std::abs(fQp) : 1.e20;
	myf pz = std::sqrt(p*p / (fTx*fTx + fTy*fTy + 1));
	myf px = fTx * pz;
	myf py = fTy * pz;
	TVector3 unit = TVector3(px, py, pz).Unit();
	nx = unit.X();
	ny = unit.Y();
	nz = unit.Z();
}

std::vector<myf> CbmLitTrackParam::GetStateVector() const {
	std::vector<myf> state(5);
	state[0] = GetX();
	state[1] = GetY();
	state[2] = GetTx();
	state[3] = GetTy();
	state[4] = GetQp();
	return state;
}

void CbmLitTrackParam::SetStateVector(
		const std::vector<myf>& x)
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
    	      << ") tx=" << fTx << " ty=" << fTy << " qp=" << fQp;// << std::endl;
//	ss << "cov: ";
//	for (Int_t i = 0; i < 15; i++) ss << fCovMatrix[i] << " ";
//	ss << std::endl;
	ss.precision(3);
	ss << " cov: x=" << fCovMatrix[0] <<  " y=" << fCovMatrix[5]
	   <<  " tx=" << fCovMatrix[9] <<  " ty=" << fCovMatrix[12]
	   <<  " q/p=" << fCovMatrix[14] << std::endl;
	return ss.str();
}
