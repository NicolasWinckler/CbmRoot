#include "CbmLitMaterialInfo.h"

#include <iostream>

CbmLitMaterialInfo::CbmLitMaterialInfo():
	fLength(0.),
	fRL(0.),
	fRho(0.),
	fZ(0.),
	fA(0.),
	fZpos(0.)
{
}

CbmLitMaterialInfo::~CbmLitMaterialInfo()
{
}

void CbmLitMaterialInfo::Print() const
{
	std::cout << "MaterialInfo: length=" << fLength << " rl=" << fRL 
		<< " rho=" << fRho << " Z=" << fZ << " A=" << fA << " zpos=" << fZpos 
		<< std::endl;
}
ClassImp(CbmLitMaterialInfo)
