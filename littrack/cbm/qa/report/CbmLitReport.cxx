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

CbmLitReport::CbmLitReport():
   fTitle(""),
   fAuthor(""),
   fErrorColor(""),
   fWarningColor(""),
   fNormalColor(""),
   fIsUseChecking(true),
   fR(NULL)
{
}

CbmLitReport::~CbmLitReport()
{
   //TODO: Check why it crashes here when trying to delete memory.
//   DeleteReportElement();
}

void CbmLitReport::CreateReportElement(
      LitReportType reportType)
{
   if (reportType == kLitLatex) fR = new CbmLitLatexReportElement();
   else if (reportType == kLitHtml) fR = new CbmLitHtmlReportElement();
   else if (reportType == kLitText) fR = new CbmLitTextReportElement();
}

void CbmLitReport::DeleteReportElement()
{
   if (NULL != fR) delete fR;
}
