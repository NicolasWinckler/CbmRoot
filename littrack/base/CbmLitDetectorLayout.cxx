#include "CbmLitDetectorLayout.h"

#include <sstream>

CbmLitDetectorLayout::CbmLitDetectorLayout()
{
}

CbmLitDetectorLayout::~CbmLitDetectorLayout()
{
}

std::string CbmLitDetectorLayout::ToString() const
{
	std::stringstream ss;
    ss << "Detector Layout:" << std::endl
       << "-number of station groups: " << GetNofStationGroups() << std::endl
       << "-number of detector planes: " << GetNofPlanes() << std::endl
       << "-station groups: " << std::endl;
    for (int i = 0; i < GetNofStationGroups(); i++){
    	ss << " " << GetStationGroup(i).ToString();
    	for (int j = 0; j < GetNofStations(i); j++) {
        	ss << "  " << GetStation(i, j).ToString();
        	for (int k = 0; k < GetNofSubstations(i, j); k++) {
        		ss << "   " << GetSubstation(i, j, k).ToString();
        	}
    	}
    }
    ss << std::endl;
	return ss.str();
}
