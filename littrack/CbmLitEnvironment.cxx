// -------------------------------------------------------------------------
// -----                  CbmLitEnvironment source file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------

#include "CbmLitEnvironment.h"
#include "CbmLitMemoryManagment.h"
#include "CbmLitComparators.h"

#include "CbmField.h"
#include "CbmRunAna.h"

#include "TGeoNode.h"
#include "TGeoMaterial.h"
#include "TGeoManager.h"
#include "TGeoPcon.h"
#include "TGeoPgon.h"
#include "TGeoArb8.h"
#include "TGeoSphere.h"
#include "TGeoMatrix.h"

#include <set>
 

CbmLitEnvironment* CbmLitEnvironment::fInstance = NULL;

CbmLitEnvironment::CbmLitEnvironment():
   fField(NULL)
{
}

CbmLitEnvironment::~CbmLitEnvironment() 
{
   if (fInstance != NULL) delete fInstance;
}

CbmLitEnvironment* CbmLitEnvironment::Instance()
{
   if (fInstance == NULL) {
      fInstance = new CbmLitEnvironment();
   }
   return fInstance;
}

CbmField* CbmLitEnvironment::GetField()
{
   if (fField == NULL) {
      CbmRunAna *Run = CbmRunAna::Instance();
         
      if(NULL == Run) {
         std::cout << "-E- CbmLitEnvironment::GetField : "
            << "Run Ana is not instantiated" << std::endl;
         return NULL;
      }
      
      std::cout << "-I- CbmLitEnvironment::GetField : "
         << "Reading Magnetic Field " << std::endl;
      
      fField = (CbmField*) Run->GetField();
   }
   return fField;
}

CbmLitDetectorLayout CbmLitEnvironment::GetMuchLayout()
{
	static Bool_t layoutCreated = false;
	
	if (!layoutCreated) {
		//CbmLitDetectorLayout layout;
		std::set<Double_t> zPosSet;
		
		TGeoNode* much = (TGeoNode*) gGeoManager->GetTopNode()->GetNodes()->FindObject("much1_0");
			
		const Double_t* muchPos  = much->GetMatrix()->GetTranslation();
		TObjArray* muchNodes = much->GetNodes();
		for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
			TGeoNode* muchNode = (TGeoNode*) muchNodes->At(iMuchNode);
			if (TString(muchNode->GetName()).Contains("station")) {
				TGeoPcon* shape = (TGeoPcon*) muchNode->GetVolume()->GetShape();
				zPosSet.insert(muchPos[2] + shape->GetZ(0) + shape->GetDZ());
			}
		}
		
		std::vector<Double_t> layersZPos(zPosSet.begin(), zPosSet.end());
		fMuchLayout.SetLayerZ(layersZPos);
		fMuchLayout.SetNofLayers(layersZPos.size());
		DetermineLayout(fMuchLayout);
		fMuchLayout.Print();
		layoutCreated = true;
	}
	return fMuchLayout;
}

CbmLitDetectorLayout CbmLitEnvironment::GetTrdLayout()
{
	static Bool_t layoutCreated = false;
	
	if (!layoutCreated) {
		std::set<Double_t> zPosSet;
			
		TObjArray* nodes = gGeoManager->GetTopNode()->GetNodes();
		for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
			
			TGeoNode* node = (TGeoNode*) nodes->At(iNode);
			if (TString(node->GetName()).Contains("trd")) {
				
				TGeoNode* station = node; 
				const Double_t* stationPos = station->GetMatrix()->GetTranslation();
				TObjArray* layers = station->GetNodes();
				for (Int_t iLayer = 0; iLayer < layers->GetEntriesFast(); iLayer++) {
								
					TGeoNode* layer = (TGeoNode*) layers->At(iLayer);
					const Double_t* layerPos = layer->GetMatrix()->GetTranslation();
								
					TObjArray* layerParts = layer->GetNodes();
					for (Int_t iLayerPart = 0; iLayerPart < layerParts->GetEntriesFast(); iLayerPart++) {
								
						TGeoNode* layerPart = (TGeoNode*) layerParts->At(iLayerPart);
										
						if (TString(layerPart->GetName()).Contains("gas")) {
							const Double_t* pos = layerPart->GetMatrix()->GetTranslation();
							TGeoPgon* shape = (TGeoPgon*) layerPart->GetVolume()->GetShape();
							zPosSet.insert(stationPos[2] + layerPos[2] + pos[2] - shape->GetDZ());
						}					
					}
				}
			}
		}
		
		std::vector<Double_t> layersZPos(zPosSet.begin(), zPosSet.end());
		fTrdLayout.SetLayerZ(layersZPos);
		fTrdLayout.SetNofLayers(layersZPos.size());
		DetermineLayout(fTrdLayout);
		fTrdLayout.Print();
	}
	
	return fTrdLayout;
}


void CbmLitEnvironment::DetermineLayout(
		CbmLitDetectorLayout& layout)
{
	std::vector<Int_t> nofLayersPerStation;	   
	Int_t nofStations = 0;
	Int_t nofLayers = layout.GetNofLayers();
	Int_t layers = 0;
	Double_t prev;
	Int_t cnt = 0;
	for (Int_t i = 0; i < nofLayers; i++) {
		if (i != 0) {
			if ((layout.GetLayerZ(i) - prev) > 20.) {
	            nofStations++;
	            nofLayersPerStation.push_back(layers);
	            layers = 0;
	        }
	        if (cnt == nofLayers-1) {
	            nofStations++;
	            nofLayersPerStation.push_back(++layers);
	        }
	    }      
	    layers++;  
	    cnt++;
	    prev = layout.GetLayerZ(i);      
	}
	layout.SetNofStations(nofStations);
	layout.SetNofLayersPerStation(nofLayersPerStation);
}

ClassImp(CbmLitEnvironment)
