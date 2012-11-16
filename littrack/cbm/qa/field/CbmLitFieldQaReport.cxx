/**
 * \file CbmLitFieldQaReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitFieldQaReport.h"
#include "CbmReportElement.h"


CbmLitFieldQaReport::CbmLitFieldQaReport()
{

}

CbmLitFieldQaReport::~CbmLitFieldQaReport()
{

}

void CbmLitFieldQaReport::Create()
{
   Out() << R()->DocumentBegin() << std::endl;
   Out() << R()->Title(0, "Magnetic field QA") << std::endl;
//   Out() << PrintImages(".*field_qa_.*png");
   Out() << R()->DocumentEnd();
}

