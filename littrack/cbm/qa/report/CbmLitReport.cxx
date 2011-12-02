/**
 * \file CbmLitReport.cxx
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitReport.h"
#include "CbmLitLatexReportElement.h"
#include "CbmLitHtmlReportElement.h"
#include "CbmLitTextReportElement.h"

CbmLitReport::CbmLitReport(LitReportType reportType):
   fTitle(""),
   fAuthor(""),
   fIsElectronSetup(true),
   fIsMvd(true),
   fIsSts(true),
   fIsRich(true),
   fIsTrd(true),
   fIsMuch(true),
   fIsTof(true),
   fErrorColor(""),
   fWarningColor(""),
   fNormalColor(""),
   fIsUseChecking(true),
   fR(NULL)
{
   if (reportType == kLitLatex) fR = new CbmLitLatexReportElement();
   else if (reportType == kLitHtml) fR = new CbmLitHtmlReportElement();
   else if (reportType == kLitText) fR = new CbmLitTextReportElement();
}

CbmLitReport::~CbmLitReport()
{
   if (fR) delete fR;
}

void CbmLitReport::SetDetectorPresence(
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
