/** @file CbmStsSetup.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 27.05.2013
 **/

// Include class header
#include "setup/CbmStsSetup.h"

// Includes from C++
#include <iostream>

// Includes from ROOT
#include "TGeoManager.h"

// Includes from CbmRoot
#include "CbmDetectorList.h"

// Includes from STS
#include "setup/CbmStsModule.h"



// -----   Initialisation of static singleton pointer   --------------------
CbmStsSetup* CbmStsSetup::fgInstance = NULL;
// -------------------------------------------------------------------------



// -----   Level names   ---------------------------------------------------
const TString CbmStsSetup::fgkLevelName[] = { "sts",
                                              "station",
                                              "ladder",
                                              "halfladder",
                                              "module",
                                              "sensor",
                                              "side",
                                              "channel" };
// -------------------------------------------------------------------------



// -----   Constructor   ---------------------------------------------------
CbmStsSetup::CbmStsSetup() : CbmStsElement("STS", "system", kStsSystem),
		                         fDigitizer(NULL) {
	if ( gGeoManager) Init(gGeoManager);
}
// -------------------------------------------------------------------------



// -----   Get an element from the STS setup   -----------------------------
CbmStsElement* CbmStsSetup::GetElement(UInt_t address, Int_t level) {

	// --- Check for initialisation
	if ( ! fAddress ) LOG(FATAL) << fName << ": not initialised!"
			                         << FairLogger::endl;

	// --- Catch non-STS addresses
	if ( CbmStsAddress::GetSystemId(address) != kSTS ) {
		LOG(WARNING) << fName << ": No STS address " << address
				     << FairLogger::endl;
		return NULL;
	}

	// --- Catch illegal level numbers
	if ( level < 0 || level >= kStsNofLevels ) {
		LOG(WARNING) << fName << ": Illegal level " << level
				   << FairLogger::endl;
		return NULL;
	}

	CbmStsElement* element = this;
	for (Int_t iLevel = 1; iLevel <= level; iLevel++)
		element =
				element->GetDaughter(CbmStsAddress::GetElementId(address, iLevel));

	return element;
}
// -------------------------------------------------------------------------



// -----   Initialisation from TGeoManager   -------------------------------
Bool_t CbmStsSetup::Init(TGeoManager* geo) {

  cout << endl;
  LOG(INFO) << "=========================================================="
		    << FairLogger::endl;
  LOG(INFO) << "Initialising STS Setup" << FairLogger::endl;


  // --- Catch non-existence of GeoManager
  if ( ! geo ) {
    LOG(FATAL) << fName << ": no TGeoManager!" << FairLogger::endl;
    return kFALSE;
  }

  // --- Get cave (top node)
  LOG(INFO) << fName << ": Reading geometry from TGeoManager "
            << geo->GetName() << FairLogger::endl;
  geo->CdTop();
  TGeoNode* cave = geo->GetCurrentNode();

  // --- Get top STS node
  TGeoNode* sts = NULL;
  for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
    TString name = cave->GetDaughter(iNode)->GetName();
     if ( name.Contains("STS", TString::kIgnoreCase) ) {
      sts = cave->GetDaughter(iNode);
      geo->CdDown(iNode);
      LOG(INFO) << fName << ": STS top node is " << sts->GetName()
                << FairLogger::endl;
      break;
    }
  }
  if ( ! sts ) {
    LOG(ERROR) << fName << ": No top STS node found in geometry!"
               << FairLogger::endl;
    return kFALSE;
  }

  // --- Create physical node for sts
  TString path = cave->GetName();
  path = path + "/" + sts->GetName();
  fNode = new TGeoPhysicalNode(path);

  // --- Set system address
  fAddress = kSTS;

  // --- Recursively initialise daughter elements
  InitDaughters();

  // --- Build arrays of modules and sensors
  for (Int_t iStat = 0; iStat < GetNofDaughters(); iStat++) {
  	CbmStsElement* stat = GetDaughter(iStat);
  	for (Int_t iLad = 0; iLad < stat->GetNofDaughters(); iLad++) {
  		CbmStsElement* ladd = stat->GetDaughter(iLad);
  		for (Int_t iHla = 0; iHla < ladd->GetNofDaughters(); iHla++) {
  			CbmStsElement* hlad = ladd->GetDaughter(iHla);
  			for (Int_t iMod = 0; iMod < hlad->GetNofDaughters(); iMod++) {
  				CbmStsElement* modu = hlad->GetDaughter(iMod);
  				fModules.push_back(dynamic_cast<CbmStsModule*>(modu));
  				for (Int_t iSen = 0; iSen < modu->GetNofDaughters(); iSen++) {
  					CbmStsSenzor* sensor =
  							dynamic_cast<CbmStsSenzor*>(modu->GetDaughter(iSen));
  					fSensors.push_back(sensor);
  				}
  			}
  		}
  	}
  }

  // --- Consistency check
  if ( GetNofSensors() != GetNofElements(kStsSensor) )
  	LOG(FATAL) << GetName() << ": inconsistent number of sensors! "
  			       << GetNofElements(kStsSensor) << " " << GetNofSensors()
  			       << FairLogger::endl;

  // --- Statistics
  for (Int_t iLevel = 1; iLevel <= kStsSensor; iLevel++) {
	  TString name = fgkLevelName[iLevel];
	  name += "s";
	  LOG(INFO) << setw(12) << name << setw(5) << right
	  		      << GetNofElements(iLevel) << FairLogger::endl;
  }
  LOG(INFO) << "=========================================================="
		    << FairLogger::endl;
  cout << endl;

  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   Instance   ------------------------------------------------------
CbmStsSetup* CbmStsSetup::Instance() {
  if ( ! fgInstance ) fgInstance = new CbmStsSetup();
  return fgInstance;
}
// -------------------------------------------------------------------------



ClassImp(CbmStsSetup)

