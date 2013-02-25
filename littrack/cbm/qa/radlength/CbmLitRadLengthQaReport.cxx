/**
 * \file CbmLitRadLengthQaReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2013
 */
#include "CbmLitRadLengthQaReport.h"
#include "CbmReportElement.h"
#include "CbmHistManager.h"
#include "CbmDrawHist.h"

#include "TH1.h"
#include "TProfile2D.h"

CbmLitRadLengthQaReport::CbmLitRadLengthQaReport():
		CbmSimulationReport()
{
	SetReportName("radlength_qa");
}

CbmLitRadLengthQaReport::~CbmLitRadLengthQaReport()
{
}

void CbmLitRadLengthQaReport::Create()
{
   Out().precision(3);
   Out() << R()->DocumentBegin();
   Out() << R()->Title(0, GetTitle());

   PrintCanvases();

   Out() <<  R()->DocumentEnd();
}

void CbmLitRadLengthQaReport::Draw()
{
   DrawDetector("Total");
   DrawDetector("Mvd");
   DrawDetector("Sts");
   DrawDetector("Rich");
   DrawDetector("Trd");
   DrawDetector("Much");
   DrawDetector("Tof");

   DrawH1ByPattern("hrl_RadThickness_Trd_.+_.+_H1");
   DrawP2ByPattern("hrl_RadThickness_Trd_.+_.+_P2");
   DrawH1ByPattern("hrl_Thickness_Trd_.+_.+_H1");
   DrawP2ByPattern("hrl_Thickness_Trd_.+_.+_P2");
   DrawH1ByPattern("hrl_ThicknessSilicon_Trd_.+_.+_H1");
   DrawP2ByPattern("hrl_ThicknessSilicon_Trd_.+_.+_P2");
}

void CbmLitRadLengthQaReport::DrawDetector(
      const string& detName)
{
   if (HM()->Exists("hrl_Thickness_" + detName + "_H1")) {
      TCanvas* canvas1 = CreateCanvas(string("hrl_Thickness_" + detName + "_H1").c_str(), string("hrl_Thickness_" + detName + "_H1").c_str(), 1200, 1000);
      DrawH1(HM()->H1("hrl_Thickness_" + detName + "_H1"));
   }

   if (HM()->Exists("hrl_Thickness_" + detName + "_P2")) {
      TCanvas* canvas2 = CreateCanvas(string("hrl_Thickness_" + detName + "_P2").c_str(), string("hrl_Thickness_" + detName + "_P2").c_str(), 1200, 1000);
      DrawH2(HM()->P2("hrl_Thickness_" + detName + "_P2"));
   }

   if (HM()->Exists("hrl_RadThickness_" + detName + "_H1")) {
      TCanvas* canvas3 = CreateCanvas(string("hrl_RadThickness_" + detName + "_H1").c_str(), string("hrl_RadThickness_" + detName + "_H1").c_str(), 1200, 1000);
      DrawH1(HM()->H1("hrl_RadThickness_" + detName + "_H1"));
   }

   if (HM()->Exists("hrl_RadThickness_" + detName + "_P2")) {
      TCanvas* canvas4 = CreateCanvas(string("hrl_RadThickness_" + detName + "_P2").c_str(), string("hrl_RadThickness_" + detName + "_P2").c_str(), 1200, 1000);
      DrawH2(HM()->P2("hrl_RadThickness_" + detName + "_P2"));
   }

   if (HM()->Exists("hrl_ThicknessSilicon_" + detName + "_H1")) {
      TCanvas* canvas5 = CreateCanvas(string("hrl_ThicknessSilicon_" + detName + "_H1").c_str(), string("hrl_ThicknessSilicon_" + detName + "_H1").c_str(), 1200, 1000);
      DrawH1(HM()->H1("hrl_ThicknessSilicon_" + detName + "_H1"));
   }

   if (HM()->Exists("hrl_ThicknessSilicon_" + detName + "_P2")) {
      TCanvas* canvas6 = CreateCanvas(string("hrl_ThicknessSilicon_" + detName + "_P2").c_str(), string("hrl_ThicknessSilicon_" + detName + "_P2").c_str(), 1200, 1000);
      DrawH2(HM()->P2("hrl_ThicknessSilicon_" + detName + "_P2"));
   }
}

void CbmLitRadLengthQaReport::DrawH1ByPattern(
      const string& pattern)
{
   std::vector<TH1*> histos = HM()->H1Vector(pattern);
   Int_t nofHistos = histos.size();
   if (nofHistos < 1) return;
   for (Int_t i = 0; i < nofHistos; i++) {
      TH1* hist = histos[i];
      TCanvas* canvas = CreateCanvas(hist->GetName(), hist->GetName(), 1200, 1000);
      DrawH1(hist);
   }
}

void CbmLitRadLengthQaReport::DrawP2ByPattern(
      const string& pattern)
{
   std::vector<TProfile2D*> profiles = HM()->P2Vector(pattern);
   Int_t nofProfiles = profiles.size();
   if (nofProfiles < 1) return;
   for (Int_t i = 0; i < nofProfiles; i++) {
      TProfile2D* profile = profiles[i];
      TCanvas* canvas = CreateCanvas(profile->GetName(), profile->GetName(), 1200, 1000);
      DrawH2(profile);
   }
}

ClassImp(CbmLitRadLengthQaReport)
