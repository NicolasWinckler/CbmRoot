#include "CbmLitMaterialInfo.h"

#include <iostream>
#include <sstream>

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

std::string CbmLitMaterialInfo::ToString() const
{
	std::stringstream ss;
	ss << "MaterialInfo: length=" << fLength << " rl=" << fRL
		<< " rho=" << fRho << " Z=" << fZ << " A=" << fA << " zpos=" << fZpos
		<< std::endl;
	return ss.str();
}
