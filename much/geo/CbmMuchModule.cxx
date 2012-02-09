/** CbmMuchModule.cxx
 *@author  M.Ryzhinskiy <m.ryzhinskiy@gsi.de>
 *@version 1.0
 *@since   11.02.08
 **
 ** This class holds the transport geometry parameters
 ** of one side of MuCh module.
 **/

#include "CbmMuchModule.h"
#include "CbmMuchSector.h"
#include "CbmMuchPad.h"
#include "CbmMuchGeoScheme.h"

#include "TMath.h"
#include "TPolyLine.h"
#include "TPave.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>

using std::cout;
using std::endl;
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmMuchModule::CbmMuchModule() : TPave() {
  fDetectorId = 0;
  fCutRadius = 0;
  fPosition = TVector3();
  fSize = TVector3();
  fPoints = NULL;
  fHits = NULL;
  fClusters = NULL;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchModule::CbmMuchModule(Int_t detId, TVector3 position, TVector3 size,
        Double_t cutRadius)
: TPave(position[0] - size[0] / 2, position[1] - size[1] / 2, position[0]
        + size[0] / 2, position[1] + size[1] / 2, 1) {
  fDetectorId = detId;
  fPosition = position;
  fSize = size;
  fCutRadius = cutRadius;
  fPoints = NULL;
  fHits = NULL;
  fClusters = NULL;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmMuchModule::CbmMuchModule(Int_t iStation, Int_t iLayer, Bool_t iSide,
        Int_t iModule, TVector3 position, TVector3 size, Double_t cutRadius)
: TPave(position[0] - size[0] / 2, position[1] - size[1] / 2, position[0]
         + size[0] / 2, position[1] + size[1] / 2, 1) {
  fDetectorId = CbmMuchGeoScheme::GetDetectorId(iStation, iLayer, iSide,
          iModule);
  fPosition = position;
  fSize = size;
  fCutRadius = cutRadius;
  fPoints = NULL;
  fHits = NULL;
  fClusters = NULL;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmMuchModule::~CbmMuchModule() {
}
// -------------------------------------------------------------------------


ClassImp(CbmMuchModule)
