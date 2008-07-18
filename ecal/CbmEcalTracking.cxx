#include "CbmEcalTracking.h"

#include "CbmEcalStructure.h"
#include "CbmEcalTrackExtrapolation.h"
#include "CbmRootManager.h"

#include "CbmTrackParam.h"

#include "TClonesArray.h"

#include <iostream>

using namespace std;

/** Default constructor **/
CbmEcalTracking::CbmEcalTracking()
  : CbmTask(), fStru(NULL), fTrE(NULL)
{
  ;
}

/** Standerd constructor **/
CbmEcalTracking::CbmEcalTracking(const char* name, const Int_t verbose)
  : CbmTask(name, verbose), fStru(NULL), fTrE(NULL)
{
  ;
}

/** Task initialization **/
InitStatus CbmEcalTracking::Init()
{
  if (!fTrE) return kFATAL; 
  
  CbmRootManager* io=CbmRootManager::Instance();
  if (!io)
  {
    Fatal("Init()", "Can't find IOManager.");
    return kFATAL;
  }
  fGTArray=(TClonesArray*)io->GetObject("GlobalTrack");
  if (!fGTArray)
  {
    Fatal("Init()", "Can't find GlobalTrack.");
    return kFATAL;
  }
  fTrackPar=new TClonesArray("CbmTrackParam", 1000);
  io->Register("EcalTrackParam", "ECAL", fTrackPar, kFALSE);
  fStru=(CbmEcalStructure*)io->ActivateBranch("EcalStructure");
  if (!fStru) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
  fTrE->SetStructure(fStru);
  
  fTrE->Init();
  return kSUCCESS;
}

/** Exec for task **/
void CbmEcalTracking::Exec(Option_t* opt)
{
  Int_t tn;
  
  fTrackPar->Clear();
  tn=fTrE->DoExtrapolate(fGTArray, fTrackPar);
  cout << "Total " << tn << " tracks before ECAL." << endl;
/*
  tn=fTrackPar->GetEntriesFast();
  for(i=0;i<tn;i++)
  {
    par=(CbmTrackParam*)fTrackPar->At(i);
    if (!par) continue;
    cout << "Track " << i << ":(" << par->GetX() << ", " << par->GetY();
    cout << ", " << par->GetZ() << ")" << endl;
  }
*/
}
  
/** Finish task **/
void CbmEcalTracking::Finish()
{
  fTrE->Finish();
}

/** virtual destructor **/
CbmEcalTracking::~CbmEcalTracking()
{
  fTrackPar->Delete();
  delete fTrackPar;
}

ClassImp(CbmEcalTracking)
