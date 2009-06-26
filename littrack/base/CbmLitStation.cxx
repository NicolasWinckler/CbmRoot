#include "CbmLitStation.h"

#include <sstream>

CbmLitStation::CbmLitStation():
	fType(kLITPIXELHIT)
{
}

CbmLitStation::~CbmLitStation()
{
}

std::string CbmLitStation::ToString() const
{
	std::stringstream ss;
	ss << "Station: type=" << GetType()
		<< " nofSubstations=" << GetNofSubstations() << std::endl;
	return ss.str();
}
