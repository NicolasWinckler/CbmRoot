/**
 * \file CbmLitStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitStudyReport.h"

CbmLitStudyReport::CbmLitStudyReport(LitReportType reportType):
   CbmLitReport(reportType)
{

}

CbmLitStudyReport::~CbmLitStudyReport()
{

}

bool CbmLitStudyReport::PropertyExists(
      const std::string& name) const
{
   return (fQa[0]->count(name) != 0) ? true : false;
}
