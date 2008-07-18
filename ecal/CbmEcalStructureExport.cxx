#include "CbmEcalStructureExport.h"

#include "CbmRootManager.h"
#include "CbmEcalStructure.h"
#include "CbmEcalCell.h"

#include "TTree.h"

#include <iostream>
#include <list>

using namespace std;

/** Default constructor **/
CbmEcalStructureExport::CbmEcalStructureExport()
{
  ;
}

/** Standerd constructor **/
CbmEcalStructureExport::CbmEcalStructureExport(const char* name, const Int_t verbose)
  : CbmTask(name, verbose)
{
  fVerbose=verbose;
}

/** Task initialization **/
InitStatus CbmEcalStructureExport::Init()
{
  fEventN=0;
  fStructureOut=NULL;
  CbmRootManager* io=CbmRootManager::Instance();
  if (!io)
  {
    Fatal("Init()", "Can't find IOManager.");
    return kFATAL;
  }
  fStr=(CbmEcalStructure*)io->ActivateBranch("EcalStructure");
  if (!fStr) 
  {
    Fatal("Init()", "Can't find calorimeter structure in the system.");
    return kFATAL;
  }
}

/** Exec for task **/
void CbmEcalStructureExport::Exec(Option_t* opt)
{
  if (fStructureOut==NULL)
  {
    fStructureOut=new TTree("EcalStr", "Exported calorimeter structure");
    fStructureOut->Branch("ev", &fEventN, "ev/I");
    fStructureOut->Branch("x", &fX, "x/F");
    fStructureOut->Branch("y", &fY, "y/F");
    fStructureOut->Branch("e", &fE, "e/F");
    fStructureOut->Branch("pse", &fPSE, "pse/F");
  }
 
  Int_t n=0;
  Int_t t=0;
  list<CbmEcalCell*> cells;
  fStr->GetCells(cells);
  list<CbmEcalCell*>::const_iterator p=cells.begin();
  for(;p!=cells.end();++p)
  {
    if ((*p)->GetTotalEnergy()>0)
    {
      fE=(*p)->GetEnergy();
      fPSE=(*p)->GetPSEnergy();
      fX=(*p)->GetCenterX();
      fY=(*p)->GetCenterY();
      n++;
      fStructureOut->Fill();
    }
    t++;
  }
  if (fVerbose>0)
  {
    Info("Exec()", "%d of %d cells writen to a file", n, t);
  }
  fEventN++;
}
  
/** Finish task **/
void CbmEcalStructureExport::Finish()
{
  if (fStructureOut) fStructureOut->Write();
}

/** virtual destructor **/
CbmEcalStructureExport::~CbmEcalStructureExport()
{
  ;
}

ClassImp(CbmEcalStructureExport)
