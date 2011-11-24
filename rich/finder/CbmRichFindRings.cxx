/** CbmRichFindRings.cxx
 * @author S. Das, contact Semen Lebedev <s.lebedev@gsi.de>
 * @since 2006
 * @version 1.0
 **/

#include "CbmRichFindRings.h"
#include "CbmRichRing.h"
#include "CbmRichRingFinder.h"
#include "FairHit.h"
#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

CbmRichFindRings::CbmRichFindRings()
  : FairTask("RICH Find Rings")
{
  fFinder = NULL;
  fRichHitArray = NULL;
  fProjArray = NULL;
  fRingArray = NULL;
  fNofRings = 0;
  fVerbose = 1;
}

CbmRichFindRings::CbmRichFindRings(
      CbmRichRingFinder* finder,
      Int_t verbose)
  : FairTask("RICH Find Rings")
{
  fFinder = finder;
  fRichHitArray = NULL;
  fProjArray = NULL;
  fRingArray = NULL;
  fNofRings = 0;
  fVerbose = verbose;
}

CbmRichFindRings::CbmRichFindRings(
      const char* name,
      const char* title,
      CbmRichRingFinder* finder,
		Int_t verbose)
  : FairTask(name)
{
  fFinder = finder;
  fRichHitArray = NULL;
  fProjArray = NULL;
  fRingArray = NULL;
  fNofRings = 0;
  fVerbose = verbose;
}

CbmRichFindRings::~CbmRichFindRings()
{
  fRingArray->Delete();
}

InitStatus CbmRichFindRings::Init()
{
  if (NULL == fFinder) { Fatal("CbmRichFindRings::Init","No ring finder selected!"); }

  FairRootManager* ioman = FairRootManager::Instance();
  if (NULL == ioman) { Fatal("CbmRichFindRings::Init","RootManager not instantised!"); }

  fRichHitArray = (TClonesArray*) ioman->GetObject("RichHit");
  if (NULL == fRichHitArray) { Fatal("CbmRichFindRings::Init","No Rich Hit array!"); }

  fProjArray = (TClonesArray*) ioman->GetObject("RichProjection");
  //if (NULL == fProjArray) { Fatal("CbmRichFindRings::Init","No Rich Ring Array of projected tracks!"); }

  fRingArray = new TClonesArray("CbmRichRing",100);
  ioman->Register("RichRing", "RICH", fRingArray, kTRUE);

  fFinder->SetVerbose(fVerbose);
  fFinder->Init();

  return kSUCCESS;
}

void CbmRichFindRings::Exec(
      Option_t* opt)
{
  fRingArray->Clear();
  fNofRings = fFinder->DoFind(fRichHitArray, fProjArray, fRingArray);
}

void CbmRichFindRings::Finish()
{
  fRingArray->Clear();
}

ClassImp(CbmRichFindRings)
