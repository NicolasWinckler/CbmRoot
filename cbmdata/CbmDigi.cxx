// -------------------------------------------------------------------------
// -----                        CbmDigi source file                    -----
// -----                  Created 09/05/07  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmDigi.h"



// -----   Default constructor   -------------------------------------------
CbmDigi::CbmDigi() : fLinks(NULL) {
}
// -------------------------------------------------------------------------


// -----   Copy constructor   -------------------------------------------
CbmDigi::CbmDigi(const CbmDigi& digi) : fLinks(NULL) {
  if ( digi.GetLinkObject() )
    fLinks = new FairMultiLinkedData(digi.GetLinkObject()->GetLinks(),
                                     digi.GetLinkObject()->GetPersistanceCheck());
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
CbmDigi::~CbmDigi() {
  if ( fLinks ) delete fLinks;
}
// -------------------------------------------------------------------------


// -----   Assignment operator   -------------------------------------------
CbmDigi& CbmDigi::operator= (const CbmDigi& digi) {

  if ( this != &digi) {                    // Protect against self-assignment

    if ( digi.GetLinkObject() )                  // Other digi has FairLinks
      fLinks = new FairMultiLinkedData(digi.GetLinkObject()->GetLinks(),
                                       digi.GetLinkObject()->GetPersistanceCheck());
    else fLinks = NULL;

  }

  return *this;
}
// -------------------------------------------------------------------------




ClassImp(CbmDigi)
