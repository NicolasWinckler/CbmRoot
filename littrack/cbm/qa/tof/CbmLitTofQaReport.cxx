/**
 * \file CbmLitTofQaReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
#include "CbmLitTofQaReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"
#include "std/utils/CbmLitUtils.h"
#include "TH1.h"
#include "TF1.h"
#include "TPad.h"
#include "TDirectory.h"
#include "TLatex.h"
#include "TCanvas.h"
#include <boost/assign/list_of.hpp>
#include <map>

using boost::assign::list_of;
using lit::NumberToString;
using lit::Split;
using std::map;

CbmLitTofQaReport::CbmLitTofQaReport():
		CbmSimulationReport()
{
	SetReportName("tof_qa");
}

CbmLitTofQaReport::~CbmLitTofQaReport()
{
}

void CbmLitTofQaReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   PrintCanvases();

   Out() << R()->DocumentEnd();
}

void CbmLitTofQaReport::Draw()
{
	DrawH2ByPattern("hmp_Tof_Reco_.+_m2p", kLinear, kLinear, kLog, "colz");
	DrawH2ByPattern("hmp_Tof_RecoMCID_.+_m2p", kLinear, kLinear, kLog, "colz");
}

ClassImp(CbmLitTofQaReport)

