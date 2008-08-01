#include "CbmLitDetectorLayout.h"

#include <iostream>

CbmLitDetectorLayout::CbmLitDetectorLayout():
	fNofStations(0),
	fNofLayers(0)
{
	
}

CbmLitDetectorLayout::~CbmLitDetectorLayout()
{
	
}

void CbmLitDetectorLayout::Print()
{
    std::cout << "Detector Layout:" << std::endl;
    std::cout << "-number of stations: " << fNofStations << std::endl;
    std::cout << "-number of layers: " << fNofLayers << std::endl;
    std::cout << "-number of layers per station: ";
    for (Int_t i = 0; i < fNofLayersPerStation.size(); i++)
    	std::cout << fNofLayersPerStation[i] << " ";
    std::cout << std::endl;
    std::cout << "-layer z positions: ";
    for (Int_t i = 0; i < fLayerZPos.size(); i++)
        	std::cout << fLayerZPos[i] << " ";
    std::cout << std::endl;
}

ClassImp(CbmLitDetectorLayout)
