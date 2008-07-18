/** CbmMuchTask.cxx
 *@author Mikhail Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@since 22.03.07
 *@version 1.0
 **
 ** Base CBM task class for finding hits in the MUCH
 ** Task level RECO
 **/

#include "CbmMuchTask.h"

// -----   Default constructor   ------------------------------------------
CbmMuchTask::CbmMuchTask() {
  fHits    = NULL;
  fVerbose = 0;
  fName = "MuchTask";
  fGeoScheme = CbmMuchGeoScheme::Instance();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmMuchTask::CbmMuchTask(Int_t iVerbose) {
  fVerbose = iVerbose;
  fName = "MuchTask";
  fHits    = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmMuchTask::CbmMuchTask(const char* name, Int_t iVerbose) {
  fVerbose = iVerbose;
  fName = TString(name);
  fHits    = NULL;
  fGeoScheme = CbmMuchGeoScheme::Instance();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmMuchTask::~CbmMuchTask() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchTask)
