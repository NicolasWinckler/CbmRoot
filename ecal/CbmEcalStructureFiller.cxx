#include "CbmEcalStructureFiller.h"
#include "CbmRootManager.h"
#include "CbmMCTrack.h"
#include "CbmEcalPointLite.h"
#include "CbmEcalHit.h"
#include "CbmEcal.h"
#include "TVector3.h"
#include "TRandom.h"
#include "CbmEcal.h"
#include <vector>
#include <TRandom.h>
#include <iostream>
using namespace std;

// -----   Default constructor   -------------------------------------------
CbmEcalStructureFiller::CbmEcalStructureFiller() :CbmTask() {}
// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmEcalStructureFiller::CbmEcalStructureFiller(const char *name, const Int_t iVerbose, const char* fileGeo) :CbmTask(name,iVerbose)
{
  fEvent = 0;
  fInited=kFALSE;
  fUseMCPoints=kFALSE;
  fUseSummableHits=kFALSE;
  fUseUnSummableHits=kFALSE;
  fStoreTrackInfo=kTRUE;
  fFileGeo=fileGeo;
  fInf=CbmEcalInf::GetInstance(fFileGeo);
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmEcalStructureFiller::~CbmEcalStructureFiller()
{
  CbmRootManager *fManager =CbmRootManager::Instance();
  fManager->Write();
}
// -------------------------------------------------------------------------

void CbmEcalStructureFiller::SetParContainers()
{
  ;
}

// -----   Initialization   ------------------------------------------------
InitStatus CbmEcalStructureFiller::Init()
{
  fInited=kTRUE;
  CbmRootManager* fManager = CbmRootManager::Instance();

  if (fUseMCPoints==kFALSE&&fUseSummableHits==kFALSE&&fUseUnSummableHits==kFALSE)
  {
    Warning("Init()", "No data source for structure filler! Calorimeter structure will be empty.");
//    return kFATAL;
  }
  //ECAL MC points
  if (fUseMCPoints) 
    fListECALpts = (TClonesArray*)fManager->ActivateBranch("ECALPointLite");

  //ECAL Summable hits
  if (fUseSummableHits) 
    fListHits = (TClonesArray*)fManager->ActivateBranch("EcalSumHit");
  
  //ECAL UnSummable hits
  if (fUseUnSummableHits) 
    fListUHits = (TClonesArray*)fManager->ActivateBranch("EcalHit");
  
  // all MC tracks
  //fListStack = (TClonesArray *)fManager->ActivateBranch("MCTrack");
  fInf->CheckVariables();
  fStr=new CbmEcalStructure(fInf);
  fStr->Construct();
  
  fManager->Register("EcalStructure", "ECAL", fStr, kFALSE);
  return kSUCCESS;
}
// -------------------------------------------------------------------------

/** Loop over MCPoints hits and add them to cells **/
void CbmEcalStructureFiller::LoopForMCPoints()
{
  CbmEcalPointLite* pt=NULL;
  CbmEcalCell* cell;
  Int_t ten;
  UInt_t n;
  Bool_t isPS;

  n=fListECALpts->GetEntriesFast();
  if (fVerbose>0)
  {
    cout << ". Number of input MC points: ";
    cout << n << flush;
  }

  for(UInt_t j=0; j<n; j++)
  {
    pt=(CbmEcalPointLite*)fListECALpts->At(j);
    cell=fStr->GetCell(pt->GetDetectorID(), ten, isPS);
    if (ten==0)
      if (isPS)
	cell->AddPSEnergy(pt->GetEnergyLoss());
      else
	cell->AddEnergy(pt->GetEnergyLoss());
  }
  if (fStoreTrackInfo)
  for(UInt_t j=0; j<n; j++)
  {
    pt=(CbmEcalPointLite*)fListECALpts->At(j);
    cell=fStr->GetCell(pt->GetDetectorID(), ten, isPS);
    if (ten==0)
      if (isPS)
	cell->AddTrackPSEnergy(pt->GetTrackID(),pt->GetEnergyLoss());
      else
	cell->AddTrackEnergy(pt->GetTrackID(),pt->GetEnergyLoss());
  }
}

/** Loop over summable hits and add them to cells **/
void CbmEcalStructureFiller::LoopForSummableHits()
{
  CbmEcalHit* pt=NULL;
  CbmEcalCell* cell;
  Int_t n;

  n=fListHits->GetEntriesFast();
  if (fVerbose>0)
  {
    cout << ". Number of input summable hits: ";
    cout << n << flush;
  }
  for(Int_t j=0; j<n; j++)
  {
    pt=(CbmEcalHit*)fListHits->At(j);
    cell=fStr->GetHitCell(pt->GetDetectorId());
    cell->AddEnergy(pt->GetEnergy());
    cell->AddPSEnergy(pt->GetPSEnergy());
  }
  if (fStoreTrackInfo)
  for(Int_t j=0; j<n; j++)
  {
    pt=(CbmEcalHit*)fListHits->At(j);
    cell=fStr->GetHitCell(pt->GetDetectorId());
    cell->AddTrackEnergy(pt->GetTrackId(), pt->GetEnergy());
    cell->AddTrackPSEnergy(pt->GetTrackId(), pt->GetPSEnergy());
  }
}

/** Loop over hits and add them to cells **/
void CbmEcalStructureFiller::LoopForUnSummableHits()
{
  CbmEcalHit* pt=NULL;
  CbmEcalCell* cell;
  Int_t n;

  n=fListUHits->GetEntriesFast();
  if (fVerbose>0)
  {
    cout << ". Number of input unsummable hits: ";
    cout << n << flush;
  }
  for(Int_t j=0; j<n; j++)
  {
    pt=(CbmEcalHit*)fListUHits->At(j);
    cell=fStr->GetHitCell(pt->GetDetectorId());
    cell->AddEnergy(pt->GetEnergy());
    cell->AddPSEnergy(pt->GetPSEnergy());
  }
}

// -----   Execution of Task   ---------------------------------------------
void CbmEcalStructureFiller::Exec(Option_t* option)
{
  fEvent++;
  if (fVerbose>0)
    cout << "--> Event no. " << fEvent;

  if (fStoreTrackInfo==kFALSE)
    fStr->ResetModulesFast();
  else
    fStr->ResetModules();
  if (fUseMCPoints) LoopForMCPoints();
  if (fUseSummableHits) LoopForSummableHits();
  if (fUseUnSummableHits) LoopForUnSummableHits();

  if (fVerbose>0)
    cout << endl << flush;
}
// -------------------------------------------------------------------------


// -----   Finish Task   ---------------------------------------------------
void CbmEcalStructureFiller::Finish()
{
  ;
}
// -------------------------------------------------------------------------


ClassImp(CbmEcalStructureFiller)
