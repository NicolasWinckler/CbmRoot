// -------------------------------------------------------------------------
// -----                   CbmEcalHitProducer source file              -----
// -----               Created 11/03/05  by Yu.Kharlov                 -----
// -------------------------------------------------------------------------

/* $Id: CbmEcalHitProducer.cxx,v 1.14 2006/07/29 00:33:53 prokudin Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalHitProducer.cxx,v $
 * Revision 1.14  2006/07/29 00:33:53  prokudin
 * Commenting. Bringing code to coding conventions. New way of parameter file processing
 *
 * Revision 1.13  2006/07/24 21:45:02  prokudin
 * Now we write information to parameter file
 *
 * Revision 1.12  2006/07/19 12:31:54  prokudin
 * Commenting
 *
 * Revision 1.11  2006/07/19 11:49:38  prokudin
 * Commenting, optimizing hitproducer
 *
 * Revision 1.10  2006/07/18 20:38:36  prokudin
 * MCPointsToUHits() Added. Corrected for fVerbose
 *
 * Revision 1.9  2006/07/18 13:26:38  prokudin
 * PS and ECAL energy was swaped
 *
 * Revision 1.8  2006/07/15 18:31:10  kharlov
 * Compilation warnings and coding conventions are corrected
 *
 * Revision 1.7  2006/07/14 20:02:15  prokudin
 * Some optimizations. Thanks to Denis for profiler
 *
 * Revision 1.6  2006/07/14 15:10:42  prokudin
 * Some corrections
 *
 * Revision 1.5  2006/07/13 14:42:45  prokudin
 * New version of hit producer
 *
 * Revision 1.4  2006/06/28 06:08:26  kharlov
 * Add hits with non-zero energy only
 *
 * Revision 1.3  2006/04/25 06:52:18  kharlov
 * Remove CbmEcalv[1,2] and leave the only class CbmEcal
 *
 */

#include "CbmEcalHitProducer.h"

#include "CbmEcal.h"
#include "CbmEcalPointLite.h"
#include "CbmEcalHit.h"

#include "CbmRootManager.h"
#include "CbmMCTrack.h"

#include "TVector3.h"
#include "TRandom.h"

#include <iostream>
#include <iomanip>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::map;
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmEcalHitProducer::CbmEcalHitProducer() :CbmTask() {}
// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmEcalHitProducer::CbmEcalHitProducer(const char *name, const Int_t iVerbose, const char* fileGeo) :CbmTask(name,iVerbose)
{
  fEvent = 0;
  fNHits = 0;
  fInited= kFALSE;
  fUseMCPoints=kFALSE;
  fUseSummableHits=kFALSE;
  fUseUnSummableHits=kFALSE;
  fProduceSummableHits=kTRUE;
  fThreshold=0;
  fThresholdPS=0;
  fNoise=0;
  fNoisePS=0;
  fFileGeo=fileGeo;
  fInf=CbmEcalInf::GetInstance(fFileGeo);
  fStandAlone=kTRUE;
  fEcalBinSize=-1;
  fPSBinSize=-1;
  fProduceHits=kTRUE;
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmEcalHitProducer::~CbmEcalHitProducer()
{
  CbmRootManager *fManager =CbmRootManager::Instance();
  fManager->Write();
}
// -------------------------------------------------------------------------

void CbmEcalHitProducer::SetParContainers()
{
  ;
}

// -----   Initialization   ------------------------------------------------
InitStatus CbmEcalHitProducer::Init()
{
  fInited=kTRUE;
  CbmRootManager* fManager = CbmRootManager::Instance();

  if (fUseMCPoints==kFALSE&&fUseSummableHits==kFALSE&&fUseUnSummableHits==kFALSE)
  {
    cerr << "Hit producer need some data source!!!" << endl;
    cerr << "Use CbmEcalHitProducer::SummableHitProducer() to produce summable hits" << endl;
    cerr << "Or CbmEcalHitProducer::UnSummableHitProducer() to produce unsummable hits." << endl;
    return kFATAL;
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
  
  // all tracks
  fListStack = (TClonesArray *)fManager->ActivateBranch("MCTrack");
  fHitCollection = new TClonesArray("CbmEcalHit",100);
  if (fProduceHits)
    if (fProduceSummableHits)
      fManager->Register("EcalSumHit","ECAL",fHitCollection,kTRUE);
    else
      fManager->Register("EcalHit","ECAL",fHitCollection,kTRUE);
  else
    if (fProduceSummableHits)
      fManager->Register("EcalSumHit","ECAL",fHitCollection,kFALSE);
    else
      fManager->Register("EcalHit","ECAL",fHitCollection,kFALSE);

  if (fStandAlone)
  {
    fInf->CheckVariables();
    fStr=new CbmEcalStructure(fInf);
    fStr->Construct();
  }
  else
  {
    fStr=(CbmEcalStructure*)fManager->ActivateBranch("EcalStructure");
    if (!fStr) 
    {
      Fatal("Init()", "Can't find calorimeter structure in the system.");
      return kFATAL;
    }
    fInf=fStr->GetEcalInf();
  }
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------

/** Loop over MCPoints hits and add them to cells **/
void CbmEcalHitProducer::LoopForMCPoints()
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

	if (fProduceSummableHits==kFALSE)
	for(UInt_t j=0; j<n; j++)
	{
	  pt = (CbmEcalPointLite*)fListECALpts->At(j);
	  cell=fStr->GetCell(pt->GetDetectorID(), ten, isPS);
	  if (ten==0)
	    if (isPS)
	      cell->AddPSEnergy(pt->GetEnergyLoss());
	    else
	      cell->AddEnergy(pt->GetEnergyLoss());
        } 
	else
	for(UInt_t j=0; j<n; j++)
	{
	  pt = (CbmEcalPointLite*)fListECALpts->At(j);
	  cell=fStr->GetCell(pt->GetDetectorID(), ten, isPS);
	  if (ten==0)
	    if (isPS)
	      cell->AddTrackPSEnergy(pt->GetTrackID(),pt->GetEnergyLoss());
	    else
	      cell->AddTrackEnergy(pt->GetTrackID(),pt->GetEnergyLoss());
 	} 

}

/** Loop over summable hits and add them to cells **/
void CbmEcalHitProducer::LoopForSummableHits()
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
	if (fProduceSummableHits==kFALSE)
	for(Int_t j=0; j<n; j++)
	{
        	pt = (CbmEcalHit*)fListHits->At(j);
		cell=fStr->GetHitCell(pt->GetDetectorId());
		cell->AddEnergy(pt->GetEnergy());
		cell->AddPSEnergy(pt->GetPSEnergy());
	}  else
	for(Int_t j=0; j<n; j++)
	{
        	pt = (CbmEcalHit*)fListHits->At(j);
		cell=fStr->GetHitCell(pt->GetDetectorId());
		cell->AddTrackEnergy(pt->GetTrackId(), pt->GetEnergy());
		cell->AddTrackPSEnergy(pt->GetTrackId(), pt->GetPSEnergy());

	}
}

/** Loop over hits and add them to cells **/
void CbmEcalHitProducer::LoopForUnSummableHits()
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
        	pt = (CbmEcalHit*)fListUHits->At(j);
		cell=fStr->GetHitCell(pt->GetDetectorId());
		cell->AddEnergy(pt->GetEnergy());
		cell->AddPSEnergy(pt->GetPSEnergy());

	}
}

/** Adds hits to the hit collection from information
 ** from cell **/
void CbmEcalHitProducer::CellToHits(CbmEcalCell* cell)
{
  Float_t energy;
  Float_t psenergy;
  Int_t ebins;
  Int_t psbins;
  if (fProduceSummableHits==kFALSE)
  {
    if (fNoise>0)
      energy=cell->GetEnergy()+gRandom->Gaus(0,fNoise);
    else
      energy=cell->GetEnergy();
    if (fNoisePS>0)
      psenergy=cell->GetPSEnergy()+gRandom->Gaus(0,fNoisePS);
    else
      psenergy=cell->GetPSEnergy();

    if (fEcalBinSize>0)
    {
      ebins=(Int_t)(energy/fEcalBinSize);
      if (energy<0) ebins--;
      energy=ebins*fEcalBinSize;
    }
    if (fPSBinSize>0)
    {
      psbins=(Int_t)(psenergy/fPSBinSize);
      if (psenergy<0) psbins--;
      psenergy=psbins*fPSBinSize;
    }

    if (energy<fThreshold) energy=0;
    if (psenergy<fThresholdPS) psenergy=0;
    cell->SetEnergy(energy);
    cell->SetPSEnergy(psenergy);
    if (fProduceHits)
    if (energy!=0||psenergy!=0)
      AddHit(cell->GetCellNumber(),energy,psenergy, -1, cell->GetTime());
  }
  else
  {
    map<Int_t, Float_t>::const_iterator p=cell->GetTrackEnergyBegin();
    for(;p!=cell->GetTrackEnergyEnd();++p)
    {
      psenergy=cell->GetTrackPSEnergy(p->first);
      energy=p->second;
      AddHit(cell->GetCellNumber(),energy,psenergy, p->first, cell->GetTrackTime(p->first));
    }
    p=cell->GetTrackPSEnergyBegin();
    for(;p!=cell->GetTrackPSEnergyEnd();++p)
    {
      energy=cell->GetTrackEnergy(p->first);
      if (energy==0)
      {
	psenergy=p->second;
	AddHit(cell->GetCellNumber(),energy,psenergy, p->first, cell->GetTrackTime(p->first));
      }
    }
  }
}
// -----   Execution of Task   ---------------------------------------------
void CbmEcalHitProducer::Exec(Option_t* option)
{

  vector<CbmEcalModule*>::const_iterator p;
  vector<CbmEcalModule*> modules;
  CbmEcalModule* module;
  vector<CbmEcalCell*> cells;

  fEvent++;
  if (fVerbose>0)
    cout << "--> Event no. " << fEvent;

  fHitCollection->Clear();
  fNHits = 0;
  UInt_t i;

  if (fStandAlone)
  {
    if (fProduceSummableHits==kFALSE)
      fStr->ResetModulesFast();
    else
      fStr->ResetModules();
    if (fUseMCPoints) LoopForMCPoints();
    if (fUseSummableHits) LoopForSummableHits();
    if (fUseUnSummableHits) LoopForUnSummableHits();
  }

  if (fVerbose>0)
    cout << endl << flush;
  fStr->GetStructure(modules);
  for(p=modules.begin();p!=modules.end();++p)
  {
    module=*p;
    if (module)
    {
      cells=module->GetCells();
      for(i=0;i<cells.size();i++)
          CellToHits(cells[i]);
    }
  }
}
// -------------------------------------------------------------------------


// -----   Add Hit to HitCollection   --------------------------------------
void CbmEcalHitProducer::AddHit(Int_t cellnumber, Float_t energy, Float_t psenergy, Int_t trackID, Double32_t time)
{
  new((*fHitCollection)[fNHits++]) CbmEcalHit(cellnumber,energy,psenergy,trackID,time); //No time information at the moment
}
// -------------------------------------------------------------------------


// -----   Finish Task   ---------------------------------------------------
void CbmEcalHitProducer::Finish()
{
  fHitCollection->Clear();
}
// -------------------------------------------------------------------------


ClassImp(CbmEcalHitProducer)
