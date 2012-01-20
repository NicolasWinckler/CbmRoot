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
