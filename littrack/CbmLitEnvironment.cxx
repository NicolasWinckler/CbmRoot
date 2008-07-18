// -------------------------------------------------------------------------
// -----                  CbmLitEnvironment source file               -----
// -----                  Created 16/07/07  by A. Lebedev               -----
// -------------------------------------------------------------------------
 

#include "CbmLitEnvironment.h"

#include "CbmLitMaterial.h"
#include "CbmLitCircleMaterial.h"
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
   fThickWall(5.),
   fField(NULL)
{
   fvMaterials.clear();
}

CbmLitEnvironment::~CbmLitEnvironment() 
{
   if (fInstance != NULL) delete fInstance;
   
   for_each(fvMaterials.begin(), fvMaterials.end(), DeleteObject());
   fvMaterials.clear();
}

CbmLitEnvironment* CbmLitEnvironment::Instance()
{
   if (fInstance == NULL) {
      fInstance = new CbmLitEnvironment();
   }
   return fInstance;
}

void CbmLitEnvironment::ReadFromGeoManager()
{
	std::cout << "ReadFromGeoManager():" << std::endl;
	
	TGeoNode* cave = gGeoManager->GetTopNode();
	
	TObjArray* nodes = cave->GetNodes();
	for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
		
		TGeoNode* node = (TGeoNode*) nodes->At(iNode);
		
		// MUCH
		if (TString(node->GetName()).Contains("much")) {
			
			TGeoNode* much = node; 
			const Double_t* muchPos  = much->GetMatrix()->GetTranslation();
			TObjArray* muchNodes = much->GetNodes();
			for (Int_t iMuchNode = 0; iMuchNode < muchNodes->GetEntriesFast(); iMuchNode++) {
				
				TGeoNode* muchNode = (TGeoNode*) muchNodes->At(iMuchNode);
			    
				CbmLitCircleMaterial* mat = new CbmLitCircleMaterial();
			    
			    TString nodeName = muchNode->GetName();
				const Double_t* pos = muchNode->GetMatrix()->GetTranslation();
			    TGeoVolume* volume = muchNode->GetVolume();
			    TGeoPcon* shape = (TGeoPcon*) volume->GetShape();
			    			    
			    FillMaterialInfo(volume->GetMaterial(), mat);
			    
			    mat->SetName(nodeName);
			    if (shape->GetDZ() < 5.) mat->SetZ(muchPos[2] + shape->GetZ(0) + shape->GetDZ());
			    else mat->SetZ(muchPos[2] + shape->GetZ(0) + 2 * shape->GetDZ());
			    mat->SetThickness(2 * shape->GetDZ());
			    mat->SetInnerRadius(shape->GetRmin(0));
			    mat->SetOuterRadius(shape->GetRmax(1));
			    mat->SetXCenter(muchPos[0]+pos[0]);
			    mat->SetYCenter(muchPos[1]+pos[1]);
			    //mat->Print();
		
			    fvMaterials.push_back(mat);
			}
		}
		// end MUCH
		
		// TRD
		if ( ((TString)node->GetName()).Contains("trd")) {
			
			TGeoNode* station = node; 
			const Double_t* stationPos = station->GetMatrix()->GetTranslation();
			TObjArray* layers = station->GetNodes();
			for (Int_t iLayer = 0; iLayer < layers->GetEntriesFast(); iLayer++) {
				
				TGeoNode* layer = (TGeoNode*) layers->At(iLayer);
				const Double_t* layerPos = layer->GetMatrix()->GetTranslation();
				
				/// save this node to vector
								
				TObjArray* layerParts = layer->GetNodes();
				for (Int_t iLayerPart = 0; iLayerPart < layerParts->GetEntriesFast(); iLayerPart++) {
				
					TGeoNode* layerPart = (TGeoNode*) layerParts->At(iLayerPart);
					
					CbmLitCircleMaterial* mat = new CbmLitCircleMaterial();
								    
					TString nodeName = layerPart->GetName();
					const Double_t* pos = layerPart->GetMatrix()->GetTranslation();
					TGeoVolume* volume = layerPart->GetVolume();
					TGeoPgon* shape = (TGeoPgon*) volume->GetShape();
													
					FillMaterialInfo(volume->GetMaterial(), mat);
					
					mat->SetName(nodeName);
					if (shape->GetDZ() < 5.) 
						mat->SetZ(stationPos[2] + layerPos[2] + pos[2]);
					else 
						mat->SetZ(stationPos[2] + layerPos[2] + pos[2] +  shape->GetDZ());
					mat->SetThickness(2 * shape->GetDZ());
					mat->SetInnerRadius(shape->GetRmin(0));
					mat->SetOuterRadius(shape->GetRmax(1));
					mat->SetXCenter(stationPos[0]+layerPos[0]+pos[0]);
					mat->SetYCenter(stationPos[1]+layerPos[1]+pos[1]);
					//mat->Print();
							
					fvMaterials.push_back(mat);
				}
			}
		}
		//end TRD
		
		// RICH
		if ( ((TString)node->GetName()).Contains("rich")) {
			
			TGeoNode* rich = node; 
			const Double_t* richPos  = rich->GetMatrix()->GetTranslation();
			TObjArray* richNodes = rich->GetNodes();
			for (Int_t iRichNode = 0; iRichNode < richNodes->GetEntriesFast(); iRichNode++) {
				
				TGeoNode* richNode = (TGeoNode*) richNodes->At(iRichNode);
			    
				CbmLitCircleMaterial* mat = new CbmLitCircleMaterial();
			    
			    TString nodeName = richNode->GetName();
				const Double_t* pos = richNode->GetMatrix()->GetTranslation();
			    TGeoVolume* volume = richNode->GetVolume();
			    TGeoTrap* shape = (TGeoTrap*) volume->GetShape();
			    			    
			    FillMaterialInfo(volume->GetMaterial(), mat);
			    
			    mat->SetName(nodeName);
			    if (shape->GetDZ() < 5.) 
			    	mat->SetZ(richPos[2] + pos[2]);
			    else 
			    	mat->SetZ(richPos[2] + pos[2] + shape->GetDZ());
			    mat->SetThickness(2 * shape->GetDZ());
			    mat->SetInnerRadius(0.);
			    mat->SetOuterRadius(300.);
			    mat->SetXCenter(richPos[0]+pos[0]);
			    mat->SetYCenter(richPos[1]+pos[1]);
			    
			    fvMaterials.push_back(mat);
			    
			    if (nodeName.Contains("gas3")) {
			    	TGeoNode* miror = (TGeoNode*) richNode->GetNodes()->FindObject("rich1mgl_1");
					
			    	CbmLitCircleMaterial* mat = new CbmLitCircleMaterial();
				    
				    TString nodeName = miror->GetName();
					const Double_t* mirorPos = miror->GetMatrix()->GetTranslation();
				    TGeoVolume* volume = miror->GetVolume();
				    TGeoSphere* shape = (TGeoSphere*) volume->GetShape();
				    TGeoMaterial* material = volume->GetMaterial();
				    
				    FillMaterialInfo(volume->GetMaterial(), mat);
				    
				    mat->SetName(nodeName);
			    	mat->SetZ(richPos[2] + pos[2] + mirorPos[2] 
			    	          + (shape->GetRmin() + shape->GetRmax()) / 2.);
				    mat->SetThickness(shape->GetRmax()-shape->GetRmin());
				    mat->SetInnerRadius(0.);
				    mat->SetOuterRadius(300.);
				    mat->SetXCenter(richPos[0]+pos[0]+mirorPos[0]);
				    mat->SetYCenter(richPos[1]+pos[1]+mirorPos[1]);
				    
				    fvMaterials.push_back(mat);
			    }
			}			    
		}	
		// end RICH
	}
}


void CbmLitEnvironment::FillMaterialInfo(
							TGeoMaterial* material,
							CbmLitMaterial* mat
							)
{
	mat->SetRadLength(material->GetRadLen());
	mat->SetZeff(material->GetZ());
	mat->SetAeff(material->GetA());
	mat->SetDensity(material->GetDensity()); 	
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
	CbmLitDetectorLayout layout;
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
	layout.SetLayerZ(layersZPos);
	layout.SetNofLayers(layersZPos.size());
	DetermineLayout(layout);
	layout.Print();
	return layout;
}

CbmLitDetectorLayout CbmLitEnvironment::GetTrdLayout()
{
	CbmLitDetectorLayout layout;
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
	layout.SetLayerZ(layersZPos);
	layout.SetNofLayers(layersZPos.size());
	DetermineLayout(layout);
	layout.Print();
	return layout;
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

const std::vector<CbmLitMaterial*> & CbmLitEnvironment::GetMaterials()
{

   if (fvMaterials.empty()) {
      fvMaterials.clear();
      ReadFromGeoManager();
      sort(fvMaterials.begin(), fvMaterials.end(), CompareMaterialPtrZLess());
      
      std::cout << "-I- CbmLitEnvironment::ReadMaterials(): " << std::endl;
      for (UInt_t i = 0; i < fvMaterials.size(); i++) {
    	  fvMaterials[i]->Print();
      }   
   }
   return fvMaterials;
}

const std::vector<CbmLitMaterial*> & CbmLitEnvironment::GetMaterialsSimple()
{

   if (fvMaterials.empty()) {
      fvMaterials.clear();

  	  TGeoNode* cave = gGeoManager->GetTopNode();
  	
  	  TObjArray* nodes = cave->GetNodes();
  	  for (Int_t iNode = 0; iNode < nodes->GetEntriesFast(); iNode++) {
  		
  		TGeoNode* node = (TGeoNode*) nodes->At(iNode);

  		if (TString(node->GetName()).Contains("muchstation")) {
  			
  			TGeoNode* muchNode = node;
  			    
  			CbmLitCircleMaterial* mat = new CbmLitCircleMaterial();
  			    
  			TString nodeName = muchNode->GetName();
  			const Double_t* pos = muchNode->GetMatrix()->GetTranslation();
  			TGeoVolume* volume = muchNode->GetVolume();
  			TGeoPcon* shape = (TGeoPcon*) volume->GetShape();
  			    			    
  			FillMaterialInfo(volume->GetMaterial(), mat);
  			    
  			mat->SetName(nodeName);
  			if (shape->GetDZ() < 5.) mat->SetZ(shape->GetZ(0) + shape->GetDZ());
  			else mat->SetZ(shape->GetZ(0) + 2 * shape->GetDZ());
  			mat->SetThickness(2 * shape->GetDZ());
  			mat->SetInnerRadius(shape->GetRmin(0));
  			mat->SetOuterRadius(shape->GetRmax(1));
  			mat->SetXCenter(pos[0]);
  			mat->SetYCenter(pos[1]);
  			  		
  			fvMaterials.push_back(mat);
  		}
  		// end MUCH
  	  }
  	  
      std::sort(fvMaterials.begin(), fvMaterials.end(), CompareMaterialPtrZLess());
      
      std::cout << "-I- CbmLitEnvironment::ReadMaterials(): " << std::endl;
      for (UInt_t i = 0; i < fvMaterials.size(); i++) {
    	  fvMaterials[i]->Print();
      }   
   }
   return fvMaterials;
}

ClassImp(CbmLitEnvironment)


