/** CbmMuchFindHits.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 22.03.07
 *@version 1.0
 **
 ** CBM task class for finding hits in the MUCH
 ** Task level RECO
 ** Produces objects of type CbmMuchHit out of CbmMuchDigi.
 **/

#include "CbmMuchFindHits.h"
#include "CbmMuchFindClusters.h"
#include "CbmMuchFindHitsSimple.h"

// -----   Default constructor   ------------------------------------------
CbmMuchFindHits::CbmMuchFindHits() : FairTask("MuchFindHits", 1) {
  fDigiFile    = NULL;
  fSubTask = NULL;
  fUseClustering = 0;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchFindHits::CbmMuchFindHits(Int_t iVerbose) 
  : FairTask("MuchFindHits", iVerbose) {
  fDigiFile    = NULL;
  fSubTask = NULL;
  fUseClustering = 0;
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmMuchFindHits::CbmMuchFindHits(const char* name, const char* digiFileName, Int_t iVerbose) 
  : FairTask(name, iVerbose) {
  fDigiFile    = new TFile(digiFileName);
  fSubTask = NULL;
  fUseClustering = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchFindHits::~CbmMuchFindHits() {
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmMuchFindHits::Exec(Option_t* opt) {
  fSubTask->Exec(opt);
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void CbmMuchFindHits::SetParContainers() {
  if(fUseClustering) {
    fSubTask = new CbmMuchFindClusters("FindClusters", fDigiFile, fVerbose);
  }
  else
    fSubTask = new CbmMuchFindHitsSimple("FindHitsSimple", fDigiFile, fVerbose);
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus CbmMuchFindHits::Init() {

  return fSubTask->Init();
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus CbmMuchFindHits::ReInit() {
  return fSubTask->ReInit();
}
// -------------------------------------------------------------------------
    

ClassImp(CbmMuchFindHits)


	
	
	
  
		      





 
  
