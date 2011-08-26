/**
 * \file CbmLitQaBaseGenerator.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaBaseGenerator.h"

CbmLitQaBaseGenerator::CbmLitQaBaseGenerator():
   fIsElectronSetup(true),
   fIsMvd(true),
   fIsSts(true),
   fIsRich(true),
   fIsTrd(true),
   fIsMuch(true),
   fIsTof(true),
   fErrorColor("#FF0000"),
   fWarningColor("#FFFF00"),
   fNormalColor("#00FF00"),
   fIsUseChecking(false),
   fTitle("")
{

}

CbmLitQaBaseGenerator::~CbmLitQaBaseGenerator()
{

}

void CbmLitQaBaseGenerator::SetDetectorPresence(
         DetectorId detId,
         bool isDet)
{
   switch(detId) {
      case kMVD: fIsMvd = isDet; break;
      case kSTS: fIsSts = isDet; break;
      case kRICH: fIsRich = isDet; break;
      case kTRD: fIsTrd = isDet; break;
      case kMUCH: fIsMuch = isDet; break;
      case kTOF: fIsTof = isDet; break;
      default: break;
   }
}
