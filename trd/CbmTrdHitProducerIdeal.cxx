//-------------------------------------------------------------------
//-----                  CbmTrdHitProducerIdeal                 -----
//-----               Created 20/06/06 by D.Kresan              -----
//-------------------------------------------------------------------

#include "CbmTrdHitProducerIdeal.h"

#include "CbmTrdPoint.h"
#include "CbmTrdHit.h"

#include "CbmRootManager.h"

#include "TClonesArray.h"
#include "TVector3.h"

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
    fNoTrdStations = 3;
    fNoTrdPerStation = 4;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmTrdHitProducerIdeal::CbmTrdHitProducerIdeal(const char *name,
					       Int_t verbose)
:CbmTask(name, verbose)
{
    // Standard constructor
    fArrayTrdPoint = NULL;
    fArrayTrdHit = new TClonesArray("CbmTrdHit", 100);
    fEvents = 0;
    fNoTrdStations = 3;
    fNoTrdPerStation = 4;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
CbmTrdHitProducerIdeal::~CbmTrdHitProducerIdeal()
{
    // Destructor
    CbmRootManager::Instance()->Write();
    fArrayTrdHit->Clear();
    delete fArrayTrdHit;
}
// ------------------------------------------------------------------


// ------------------------------------------------------------------
InitStatus CbmTrdHitProducerIdeal::Init()
{
    // Initialisation of the task
    CbmRootManager *rootMgr = CbmRootManager::Instance();
    if(NULL == rootMgr) {
	cout << "-E- CbmTrdHitProducerIdeal::Init: "
	    << "ROOT manager is not instantiated!" << endl;
        return kFATAL;
    }
    // Activate data branches
    fArrayTrdPoint = (TClonesArray*) rootMgr->GetObject("TRDPoint");
    if(NULL == fArrayTrdPoint) {
	cout << "-W- CbmTrdHitProducerIdeal::Init: "
            << "no TRD point array!" << endl;
    }
    // Register
    rootMgr->Register("TRDHit", "TRD", fArrayTrdHit, kTRUE);
    cout << "-I- CbmTrdHitProducerIdeal::Init: "
	<<"initialisation completed." << endl;
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
        iPlane = (point->GetStationNo()-1)*fNoTrdPerStation + point->GetLayerNo();
	// Create hit
	new ((*fArrayTrdHit)[nHit]) CbmTrdHit(point->GetDetectorID(),
					      pos, dpos, iPoint,
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


