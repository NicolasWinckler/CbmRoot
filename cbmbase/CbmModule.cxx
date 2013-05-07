#include "CbmModule.h"

#include "FairGeoLoader.h"
#include "FairGeoInterface.h"
#include "FairGeoMedia.h"
#include "FairGeoBuilder.h"
#include "FairLogger.h"

#include "TGDMLParse.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoNode.h"
#include "TList.h"
#include "TFile.h"

std::map<TString, Int_t> CbmModule::fixedMats; 
Bool_t CbmModule::isFirstGDML=false;             

CbmModule::CbmModule()
{
}

CbmModule::CbmModule(const char* name, const char* Title)
  : FairModule (name, Title)
{
}

CbmModule::~CbmModule()
{
}

#ifdef ROOT_HAS_GDML
void CbmModule::ConstructGDMLGeometry(TGeoMatrix* posrot)
{
        TFile *old = gFile;
	TGDMLParse parser;
	TGeoVolume* gdmlTop;
	gdmlTop = parser.GDMLReadFile(GetGeometryFileName());
	gGeoManager->GetTopVolume()->AddNode(gdmlTop,1,posrot);
	ExpandNodeForGDML(gGeoManager->GetTopVolume()->GetNode(gGeoManager->GetTopVolume()->GetNdaughters()-1));
	isFirstGDML = 0;
        gFile = old;
}

void CbmModule::ExpandNodeForGDML(TGeoNode* curNode)
{
//	std::cout << "[ExpandNodeForGDML] Expanding node " << curNode->GetName() << std::endl;
	TGeoVolume* curVol = curNode->GetVolume();
	TString curMedName = curNode->GetMedium()->GetName();
	TGeoMedium* curMedInGeoManager = gGeoManager->GetMedium(curMedName);
	Int_t matIndToDel = gGeoManager->GetMaterialIndex(curMedName);

	if (curMedName.BeginsWith("G4_"))
	{
//		std::cout << "[ExpandNodeForGDML] Material in GDML begins with G4_" << std::endl;
		curMedName.Remove(0, 3);
	}
	else
	{
//		std::cout << "[ExpandNodeForGDML] Material in GDML does not start with G4_" << "\t\t" << curNode->GetName() << "\t\t" << curMedName << std::endl;
//		return;
	}

	Int_t nmed;

	FairGeoLoader* geoLoad = FairGeoLoader::Instance();
	FairGeoInterface* geoFace = geoLoad->getGeoInterface();
	FairGeoMedia* geoMediaBase =  geoFace->getMedia();
	FairGeoBuilder* geobuild = geoLoad->getGeoBuilder();
	FairGeoMedium* curMedInGeo;

	if (curMedInGeoManager == 0)
	{
		std::cout << "[ExpandNodeForGDML] New medium found in gmdl - it is not in gGeoManager list." << std::endl;
		//! New medium found in gmdl - it is not in gGeoManager list.
		//! This should not happen as GDML parser adds medium into the list
	}
	else
	{
		//! Medium is in the list in gGeoManager.
		//! Trying to replace it with the one from the Geo file.
//		std::cout << "Medium is in the list in gGeoManager." << std::endl;

		curMedInGeo = geoMediaBase->getMedium(curMedName);
		if (curMedInGeo == 0)
		{
			std::cout << "[ExpandNodeForGDML] Media not found in Geo file." << std::endl;
		}
		else
		{
			if (fixedMats.find(curMedName) == fixedMats.end())
			{
				nmed = geobuild->createMedium(curMedInGeo);
				fixedMats[curMedName] = gGeoManager->GetListOfMedia()->GetEntries();
//				std::cout << "[ExpandNodeForGDML] " << curMedName << " added to list\t" << fixedMats[curMedName] << "\t" << nmed << std::endl;
			}
			else
			{
//				std::cout << "[ExpandNodeForGDML] " << curMedName << " is already in the list.\t\t" << fixedMats[curMedName] << std::endl;
			}
	
			curNode->GetVolume()->SetMedium(gGeoManager->GetMedium(curMedName));
			gGeoManager->SetAllIndex();
		}
	}

//! Making volume sensitive according to the media 0-th flag
	if ((Int_t)(curVol->GetMedium()->GetParam(0)) == 1)
    {
		//! The volume is sensitive => add it to the list
//		std::cout << "\t\t\tAdding sensitive volume: " << curVol->GetName() << std::endl;
		AddSensitiveVolume(curVol);
	}
//! endof Making volume sensitive according to the media 0-th flag

//! Recursevly go down the tree of nodes
	if (curVol->GetNdaughters() != 0)
	{
		TObjArray* NodeChildList = curVol->GetNodes();
		TGeoNode* curNodeChild;
		for (Int_t j=0; j<NodeChildList->GetEntriesFast(); j++)
		{
			curNodeChild = (TGeoNode*)NodeChildList->At(j);
			ExpandNodeForGDML(curNodeChild);
		}
	}
}
#else
void CbmModule::ConstructGDMLGeometry(TGeoMatrix* posrot)
{
  gLogger->Error(MESSAGE_ORIGIN," Could not construct magnet geometry from gdml file. ");
  gLogger->Error(MESSAGE_ORIGIN," The used ROOT version does not support gdml. ");
  gLogger->Error(MESSAGE_ORIGIN," Please recompile ROOT with gdml support. ");
  gLogger->Fatal(MESSAGE_ORIGIN," Stop execution at this point. ");
}

void CbmModule::ExpandNodeForGDML(TGeoNode* curNode)
{
}
#endif

ClassImp(CbmModule);
