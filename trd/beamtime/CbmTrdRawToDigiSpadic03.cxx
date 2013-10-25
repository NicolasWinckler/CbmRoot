#include "TPrincipal.h"
#include "TMatrix.h"
#include "TClonesArray.h"
#include "CbmTrdDigiPar.h"
#include "CbmTrdModule.h"
#include "CbmTrdRawToDigiSpadic03.h"
#include "CbmTrdGeoHandler.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#define NUM_SPADIC_CHA             8
#define SPADIC_TRACE_SIZE         45
#define SPADIC_ADC_MAX           255
#define noBaselineTB               5 

CbmTrdRawToDigiSpadic03::CbmTrdRawToDigiSpadic03()
: FairTask("TrdSpadic03"),
  fPrincipal(NULL),
  fDigis(NULL),
  fDigiPar(NULL),
  fModuleInfo(NULL)
{
}


CbmTrdRawToDigiSpadic03::CbmTrdRawToDigiSpadic03(const char *name, const char *title)
: FairTask(name),
  fPrincipal(NULL),
  fDigis(NULL),
  fDigiPar(NULL),
  fModuleInfo(NULL)
{
}

CbmTrdRawToDigiSpadic03::~CbmTrdRawToDigiSpadic03()
{
  FairRootManager *ioman =FairRootManager::Instance();
  ioman->Write();
  fDigis->Delete();
  delete fDigis;
  if(fDigiPar)
    delete fDigiPar;
  if(fModuleInfo)
    delete fModuleInfo;
}

void CbmTrdRawToDigiSpadic03::SetParContainers()
{
    cout<<" * CbmTrdRawToDigiSpadic03 * :: SetParContainers() "<<endl;


    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}


// ---- ReInit  -------------------------------------------------------
InitStatus CbmTrdRawToDigiSpadic03::ReInit(){

  cout<<" * CbmTrdClusterizer * :: ReInit() "<<endl;


  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}
// --------------------------------------------------------------------

// ---- Init ----------------------------------------------------------
InitStatus CbmTrdRawToDigiSpadic03::Init()
{

  cout<<" * CbmTrdRawToDigiSpadic03 * :: Init() "<<endl;

  FairRootManager *ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");

  fDigis = new TClonesArray("CbmTrdDigi", 100);
  ioman->Register("TrdDigi","TRD Digis",fDigis,kTRUE);

  fGeoHandler->Init();

  return kSUCCESS;

}
// --------------------------------------------------------------------


// ---- Exec ----------------------------------------------------------
void CbmTrdRawToDigiSpadic03::Exec(Option_t * option)
{
  fPrincipal = new TPrincipal(NUM_SPADIC_CHA,"ND");
}

ClassImp(CbmTrdRawToDigiSpadic03)
