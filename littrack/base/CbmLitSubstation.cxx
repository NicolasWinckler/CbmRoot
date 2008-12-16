#include "CbmLitSubstation.h"

#include <sstream>

CbmLitSubstation::CbmLitSubstation():
	fZ(0.)
{
}

CbmLitSubstation::~CbmLitSubstation()
{
}

std::string CbmLitSubstation::ToString() const 
{
	std::stringstream ss;
	ss << "Substation: z=" << GetZ() << std::endl;
	return ss.str();
}

ClassImp(CbmLitSubstation);
