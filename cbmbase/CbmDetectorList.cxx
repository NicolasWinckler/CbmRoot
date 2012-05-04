/** CbmDetectorList.cxx
 *@Author V.Friese  <V.Friese@Gsi.De>
 *@Since 29.04.2010
**/

#include "CbmDetectorList.h"

#include "TString.h"

// -----   Constructor   ---------------------------------------------------
CbmDetectorList::CbmDetectorList() {
}
// -------------------------------------------------------------------------



// -----   Public method GetSystemName   -----------------------------------
void CbmDetectorList::GetSystemName(DetectorId det, TString& name) {

  switch(det) {

  case kREF:  name = "ref";  break;
  case kMVD:  name = "mvd";  break;
  case kSTS:  name = "sts";  break;
  case kRICH: name = "rich"; break;
  case kMUCH: name = "much"; break;
  case kTRD:  name = "trd";  break;
  case kTOF:  name = "tof";  break;
  case kECAL: name = "ecal"; break;
  default: name = "unknown"; break;

  }

}
// -------------------------------------------------------------------------



ClassImp(CbmDetectorList)

