#ifndef CBMLITDETECTORLAYOUT_H_
#define CBMLITDETECTORLAYOUT_H_

#include "TObject.h"

#include <vector>

class CbmLitDetectorLayout
{
public:
	CbmLitDetectorLayout();
	virtual ~CbmLitDetectorLayout();
	
	
	void SetNofStations(Int_t nofStations) {fNofStations = nofStations;}
	void SetNofLayers(Int_t nofLayers) {fNofLayers = nofLayers;}
	void SetNofLayersPerStation(const std::vector<Int_t>& nofLayersPerStation) {
		fNofLayersPerStation = nofLayersPerStation;
	}
	void SetLayerZ(const std::vector<Double_t>& layerZPos) {
		fLayerZPos = layerZPos;
	}
	
	Int_t GetNofStations() { return fNofStations; }
	Int_t GetNofLayers() { return fNofLayers; }
	Int_t GetNofLayers(Int_t station) { return fNofLayersPerStation[station]; }
	Double_t GetLayerZ(Int_t layer) { return fLayerZPos[layer]; }
	
	void Print();
	
private:
    Int_t fNofStations; 
    Int_t fNofLayers;
    std::vector<Int_t> fNofLayersPerStation;
    std::vector<Double_t> fLayerZPos;
	
	ClassDef(CbmLitDetectorLayout, 1);
};

#endif /*CBMLITDETECTORLAYOUT_H_*/
