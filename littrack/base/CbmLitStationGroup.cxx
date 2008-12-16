#include "CbmLitStationGroup.h"

#include <sstream>

CbmLitStationGroup::CbmLitStationGroup()
{
}

CbmLitStationGroup::~CbmLitStationGroup()
{
}

std::string CbmLitStationGroup::ToString() const 
{
	std::stringstream ss;
	ss << "Station group: nofStations=" << GetNofStations() << std::endl;
	return ss.str();
}

ClassImp(CbmLitStationGroup);
