//-------------------------------------------------------------------
//-----                  CbmTrdHitProducerIdeal                 -----
//-----               Created 20/06/06 by D.Kresan              -----
//-------------------------------------------------------------------

#include "CbmTrdHitProducerIdeal.h"

#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"

#include "FairRootManager.h"

#include "TClonesArray.h"
#include "TVector3.h"
#include "TGeoVolume.h"
#include "TGeoManager.h"

#include <iostream>
using std::cout;
using std::endl;


//___________________________________________________________________
//
// CbmTrdHitProducerIdeal
//
// Task for creating the TRD hits using MC information. No smearing
// is involved - ideal case.
//


// ------------------------------------------------------------------
CbmTrdHitProducerIdeal::CbmTrdHitProducerIdeal()
{
    // Default constructor
    fArrayTrdPoint = NULL;
    fArrayTrdHit = new TClonesArray("CbmTrdHit", 100);
    fEvents = 0;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmTrdHitProducerIdeal::CbmTrdHitProducerIdeal(const char *name,
					       Int_t verbose)
:FairTask(name, verbose)
{
    // Standard constructor
    fArrayTrdPoint = NULL;
    fArrayTrdHit = new TClonesArray("CbmTrdHit", 100);
    fEvents = 0;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmTrdHitProducerIdeal::~CbmTrdHitProducerIdeal()
{
    // Destructor
    FairRootManager::Instance()->Write();
    fArrayTrdHit->Clear();
    delete fArrayTrdHit;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
InitStatus CbmTrdHitProducerIdeal::Init()
{
    // Initialisation of the task
    FairRootManager *rootMgr = FairRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmTrdHitProducerIdeal::Init: "
	    << "ROOT manager is not instantiated!" << endl;
        return kFATAL;
    }

    // Activate data branches
    fArrayTrdPoint = (TClonesArray*) rootMgr->GetObject("TrdPoint");
    if(NULL == fArrayTrdPoint) {
	cout << "-W- CbmTrdHitProducerIdeal::Init: "
            << "no TrdPoint array!" << endl;
    }

    // Register
    rootMgr->Register("TrdHit", "TRD", fArrayTrdHit, kTRUE);
    cout << "-I- CbmTrdHitProducerIdeal::Init: "
	<<"initialisation completed." << endl;

    // Extract information about the number of TRD stations and
    // the number of layers per TRD station from the geomanager.
    // Store the information about the number of layers at the entrance
    // of subsequent stations in a vector. 
    // This allows to calculate the layer number starting with 1 for the
    // first layer of the first station at a later stage by only adding 
    // the layer number in the station to the number of layers in 
    // previous stations 
    TGeoVolume *fm=NULL;
    Int_t stationNr = 1;
    Int_t totalNrOfLayers = 0;
    fLayersBeforeStation.push_back(totalNrOfLayers);
    char volumeName[10];
    sprintf(volumeName, "trd%d", stationNr);
    fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
    if (fm){
      Int_t nrOfLayers = fm->GetNdaughters();
      totalNrOfLayers += nrOfLayers;
      fLayersBeforeStation.push_back(totalNrOfLayers);
      do {
	stationNr++;
        sprintf(volumeName, "trd%d", stationNr);
	fm = (TGeoVolume *)gGeoManager->GetListOfVolumes()->FindObject(volumeName);  
        if (fm) {
          nrOfLayers = fm->GetNdaughters();
	  totalNrOfLayers += nrOfLayers;
	  fLayersBeforeStation.push_back(totalNrOfLayers);
	}
      } while (fm);
    } else {
      cout << "***************************************" <<endl;
      cout << "                                       " <<endl;
      cout << " - FATAL ERROR Unknown geometry version" <<endl;
      cout << "   in CbmTrdHitProducerSmearing        " <<endl;
      cout << " No TRD stations found in the geometry " <<endl;
      cout << "                                       " <<endl;
      cout << "***************************************" <<endl;
      return kFATAL;
    }

    return kSUCCESS;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
void CbmTrdHitProducerIdeal::Exec(Option_t *option)
{
    // Execution of the task, copy position from TRD point to
    // TRD hit
    if(NULL == fArrayTrdPoint) return;

    fArrayTrdHit->Clear();

    Int_t nPoint = fArrayTrdPoint->GetEntriesFast();
    Int_t nHit = 0;
    CbmTrdPoint *point;
    TVector3 pos;
    TVector3 dpos(0., 0., 0.);
    Int_t iPlane;

    // Loop over TRD points
    for(Int_t iPoint = 0; iPoint < nPoint; iPoint++) {
	// Get pointer to the point
	point = (CbmTrdPoint*) fArrayTrdPoint->At(iPoint);
	if(NULL == point) continue;
	// Get position
	point->Position(pos);

	// Calculate plane number
        Int_t* detInfo = fDetId.GetDetectorInfo(point->GetDetectorID()); 
        iPlane=fLayersBeforeStation[(detInfo[1]-1)]+detInfo[2];

	// Create hit
	new ((*fArrayTrdHit)[nHit]) CbmTrdHit(point->GetDetectorID(),
					      pos, dpos, 0., iPoint,
     					      iPlane, 0.,
					      point->GetEnergyLoss(),
					      point->GetEnergyLoss());
        nHit++;
    }

    cout << "-I- CbmTrdHitProducerIdeal::Exec:  "
	<< "event: " << fEvents << ",  TRD points: " << nPoint
	<< ",  TRD hits created: " << nHit << endl;
    fEvents += 1;
}
// ------------------------------------------------------------------


ClassImp(CbmTrdHitProducerIdeal);


