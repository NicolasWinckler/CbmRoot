//-------------------------------------------------------------------
//-----                  CbmTrdHitProducerIdeal                 -----
//-----               Created 20/06/06 by D.Kresan              -----
//-------------------------------------------------------------------

// Includes from ROOT  
#include "TClonesArray.h"
#include "TVector3.h"

// Includes from base
#include "FairRootManager.h"
#include "FairLogger.h"

// Includes from TRD
#include "CbmTrdHitProducerIdeal.h"
#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"
#include "CbmTrdGeoHandler.h"
#include "CbmTrdDetectorId.h"

#include <iostream>

using std::cout;
using std::endl;


//___________________________________________________________________
//
// CbmTrdHitProducerIdeal
//
// Task for creating the TRD hits using MC information.
// No smearing is involved - ideal case.
//


// ------------------------------------------------------------------
CbmTrdHitProducerIdeal::CbmTrdHitProducerIdeal()
  :FairTask("TrdHitProducer"),
   fArrayTrdPoint(NULL),
   fArrayTrdHit(new TClonesArray("CbmTrdHit", 100)),
   fEvents(0),
   fLayersBeforeStation() 
{
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmTrdHitProducerIdeal::CbmTrdHitProducerIdeal(const char *name,
					       Int_t verbose)
  :FairTask(name, verbose),
   fArrayTrdPoint(NULL),
   fArrayTrdHit(new TClonesArray("CbmTrdHit", 100)),
   fEvents(0),
   fLayersBeforeStation() 
{
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

  LOG(INFO)<<"********** Initilization of TRD Hitproducer ********"<<FairLogger::endl;

  // Initialisation of the task
  FairRootManager *rootMgr = FairRootManager::Instance();
  if(NULL == rootMgr) {
    LOG(ERROR) << "CbmTrdHitProducerIdeal::Init: "
	 << "ROOT manager is not instantiated!" << FairLogger::endl;
    return kFATAL;
  }
  
  // Activate data branches
  fArrayTrdPoint = (TClonesArray*) rootMgr->GetObject("TrdPoint");
  if(NULL == fArrayTrdPoint) {
    LOG(ERROR) << "CbmTrdHitProducerIdeal::Init: "
	 << "no TrdPoint array!" << FairLogger::endl;
    return kFATAL;
  }
  
  // Register
  rootMgr->Register("TrdHit", "TRD", fArrayTrdHit, kTRUE);
  LOG(INFO) << "CbmTrdHitProducerIdeal::Init: "
       <<"initialisation completed." << FairLogger::endl;
  
  // Extract information about the number of TRD stations and
  // the number of layers per TRD station from the geomanager.
  // Store the information about the number of layers at the entrance
  // of subsequent stations in a vector. 
  // This allows to calculate the layer number starting with 1 for the
  // first layer of the first station at a later stage by only adding 
  // the layer number in the station to the number of layers in 
  // previous stations 
  CbmTrdGeoHandler trdGeoInfo;
  
  Bool_t result = trdGeoInfo.GetLayerInfo(fLayersBeforeStation);
  
  if (!result) return kFATAL;
  
  // This HitProducer is only usefull to study tracking
  // Maybe it has to be deleted.
  //  fRadiator->Init();
  
  LOG(INFO)<<"********** End of TRD Hitproducer init ********"<<FairLogger::endl;

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
	Int_t detectorId = point->GetDetectorID();
	iPlane = fLayersBeforeStation[CbmTrdDetectorId::GetStationNr(detectorId) - 1] + CbmTrdDetectorId::GetLayerNr(detectorId);

	// Create hit
	new ((*fArrayTrdHit)[nHit]) CbmTrdHit(point->GetDetectorID(),
					      pos, dpos, 0., iPoint,
     					      iPlane, 0.,
					      point->GetEnergyLoss(),
					      point->GetEnergyLoss());
        nHit++;
    }

    LOG(INFO) << "CbmTrdHitProducerIdeal::Exec:  "
	<< "event: " << fEvents << ",  TRD points: " << nPoint
	<< ",  TRD hits created: " << nHit << FairLogger::endl;
    fEvents += 1;
}
// ------------------------------------------------------------------


ClassImp(CbmTrdHitProducerIdeal);


