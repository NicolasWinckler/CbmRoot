/**
 * \file CbmLitTrackingQaStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaStudyReport.h"
#include "../report/CbmLitReportElement.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;

CbmLitTrackingQaStudyReport::CbmLitTrackingQaStudyReport()
{
}

CbmLitTrackingQaStudyReport::~CbmLitTrackingQaStudyReport()
{
}

void CbmLitTrackingQaStudyReport::Create(
      LitReportType reportType,
      ostream& out,
      const vector<string>& resultDirectories,
      const vector<string>& studyNames)
{
   CreateReportElement(reportType);

   int nofStudies = resultDirectories.size();

   fResultDirectories = resultDirectories;
   fStudyNames = studyNames;

   fQa.resize(nofStudies);
   fCheck.resize(nofStudies);
   for(int iStudy = 0; iStudy < nofStudies; iStudy++) {
      read_json(resultDirectories[iStudy] + "/tracking_qa.json", fQa[iStudy]);
      read_json(resultDirectories[iStudy] + "/tracking_qa_check.json", fCheck[iStudy]);
   }
   string idealFile = string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/tracking_qa_ideal.json");
   read_json(idealFile.c_str(), fIdeal);

   Create(out);

   fResultDirectories.clear();
   fStudyNames.clear();
   fQa.clear();
   fCheck.clear();
   fIdeal.clear();

   DeleteReportElement();
}

void CbmLitTrackingQaStudyReport::Create(
      ostream& out)
{
   // TODO: Correct detector presence.
   bool isSts = PropertyExists("hNofStsTracks");
   bool isRich = PropertyExists("hNofRichRings");
   bool isTrd = PropertyExists("hNofTrdTracks");
   bool isMuch = PropertyExists("hNofMuchTracks");
   bool isTof = true;

   out.precision(3);
   out << fR->DocumentBegin();
   out << (fTitle != "") ? fR->Title(0, fTitle) : string("");

   out << fR->TableBegin("Number of events", list_of(string("")).range(fStudyNames));
   if (PropertyExists("hEventNo")) out << PrintRow("hEventNo", "Number of events");
   out << fR->TableEnd() << endl;

   out << fR->TableBegin("Number of objects", list_of(string("")).range(fStudyNames));
   if (PropertyExists("hNofStsTracks")) out << PrintRow("hNofStsTracks", "STS tracks");
   if (PropertyExists("hNofRichProjections"))out << PrintRow("hNofRichProjections", "RICH projections");
   if (PropertyExists("hNofRichRings"))out << PrintRow("hNofRichRings", "RICH rings");
   if (PropertyExists("hNofTrdTracks"))out << PrintRow("hNofTrdTracks", "TRD tracks");
   if (PropertyExists("hNofMuchTracks"))out << PrintRow("hNofMuchTracks", "MUCH tracks");
   if (PropertyExists("hNofGlobalTracks"))out << PrintRow("hNofGlobalTracks", "Global tracks");
   out << fR->TableEnd() << endl;

   string signal = "el";
   if (!isRich) signal = "mu";
   string recDet = (isTrd) ? "TRD" : (isMuch) ? "MUCH" :
         (isTrd && isMuch) ? "MUCH+TRD" : "";

   out << fR->TableBegin("Reconstruction efficiency", list_of(string("")).range(fStudyNames));
   out << PrintRowEff("hSts3D.all", "STS all");
   out << PrintRowEff("hSts3D." + signal, "STS " + signal);
   out << PrintRowEff("hRec3D.all", recDet + " all");
   out << PrintRowEff("hRec3D." + signal, recDet + " " + signal);
   out << PrintRowEff("hTof3D.all", "TOF all");
   out << PrintRowEff("hTof3D." + signal, "TOF " + signal);

   out << fR->TableEmptyRow(fStudyNames.size() + 1, "Normalization STS+" + recDet);
   out << PrintRowEff("hSts3DNormHalfGlobal.all", "STS all");
   out << PrintRowEff("hSts3DNormHalfGlobal." + signal, "STS " + signal);
   out << PrintRowEff("hHalfGlobal3D.all", "STS+" + recDet + " all");
   out << PrintRowEff("hHalfGlobal3D." + signal, "STS+" + recDet + " " + signal);

   out << fR->TableEmptyRow(fStudyNames.size() + 1, "Normalization STS+" + recDet + "+TOF");
   out << PrintRowEff("hSts3DNormGlobal.all", "STS all");
   out << PrintRowEff("hSts3DNormGlobal." + signal, "STS " + signal);
   out << PrintRowEff("hHalfGlobal3DNormGlobal.all", "STS+" + recDet + " all");
   out << PrintRowEff("hHalfGlobal3DNormGlobal." + signal, "STS+" + recDet + " " + signal);
   out << PrintRowEff("hGlobal3D.all", "STS+" + recDet + "+TOF all");
   out << PrintRowEff("hGlobal3D.all", "STS+" + recDet + "+TOF " + signal);

   if (isRich) {
      out << fR->TableEmptyRow(fStudyNames.size() + 1, "Efficiency with RICH");
      out << PrintRowEff("hRich3D.El", "RICH el");
      out << PrintRowEff("hRich3D.ElRef", "RICH el ref");

      out << fR->TableEmptyRow(fStudyNames.size() + 1, "Normalization STS+RICH");
      out << PrintRowEff("hSts3DNormStsRich.El", "STS el");
      out << PrintRowEff("hStsRich3D.El", "STS+RICH el");

      out << fR->TableEmptyRow(fStudyNames.size() + 1, "Normalization STS+RICH+TRD");
      out << PrintRowEff("hSts3DNormStsRichTrd.El", "STS el");
      out << PrintRowEff("hStsRich3DNormStsRichTrd.El", "STS+RICH el");
      out << PrintRowEff("hStsRichTrd3D.El", "STS+RICH+TRD el");

      out << fR->TableEmptyRow(fStudyNames.size() + 1, "Normalization STS+RICH+TRD+TOF");
      out << PrintRowEff("hSts3DNormStsRichTrdTof.El", "STS el");
      out << PrintRowEff("hStsRich3DNormStsRichTrdTof.El", "STS+RICH el");
      out << PrintRowEff("hStsRichTrd3DNormStsRichTrdTof.El", "STS+RICH+TRD el");
      out << PrintRowEff("hStsRichTrdTof3D.El", "STS+RICH+TRD+TOF el");
   }
   out << fR->TableEnd() << endl;

   out << fR->TableBegin("Ghost rate", list_of(string("")).range(fStudyNames));
   out << PrintRow("fhStsGhostNh", "STS");
   out << PrintRow("fhRecGhostNh", recDet);
   if (isRich) {
      out << PrintRow("fhRichGhostNh", "RICH");
      out << fR->TableEmptyRow(fStudyNames.size() + 1, "after STS-RICH matching");
      out << PrintRow("fhStsGhostRichMatchingNh", "STS");
      out << PrintRow("fhRichGhostStsMatchingNh", "RICH");
      out << fR->TableEmptyRow(fStudyNames.size() + 1, "after STS-RICH matching and el identification");
      out << PrintRow("fhRichGhostElIdNh", "RICH");
   }
   out << fR->TableEnd() << endl;
   // For image paths put only file name without type, e.g. ".eps" or ".png".
   // Type will be added automatically.
   if (isSts) out << PrintImageTable("STS reconstruction efficiency", "tracking_qa_sts_efficiency");
   if (isTrd || isMuch) out << PrintImageTable(recDet + " reconstruction efficiency", "tracking_qa_rec_efficiency");
   if (isTof) out << PrintImageTable("TOF hit matching efficiency", "tracking_qa_tof_efficiency");
   if (isRich) out << PrintImageTable("RICH efficiency electrons", "tracking_qa_rich_efficiency_electrons");
   out << PrintImageTable("Global reconstruction efficiency", "tracking_qa_global_efficiency_all");
   out << PrintImageTable("Global reconstruction efficiency for signal", "tracking_qa_global_efficiency_signal");
   if (isRich) out << PrintImageTable("Global reconstruction efficiency with RICH", "tracking_qa_sts_rich_trd_tof_efficiency_electrons");

   out <<  fR->DocumentEnd();
}

string CbmLitTrackingQaStudyReport::PrintRow(
      const string& property,
      const string& name)
{
   vector<string> n(fStudyNames.size(), "");
   for (int i = 0; i < fStudyNames.size(); i++) {
      n[i] = PrintValue(i, property);
   }
   return fR->TableRow(list_of(name).range(n));
}

string CbmLitTrackingQaStudyReport::PrintRowEff(
      const string& property,
      const string& name)
{
   vector<string> n(fStudyNames.size(), "");
   for (int i = 0; i < fStudyNames.size(); i++) {
      stringstream ss;
      string eff = PrintValue(i, property + ".eff");
      string acc = PrintValue(i, property + ".acc");
      string rec = PrintValue(i, property + ".rec");
      ss << eff << " ("<< rec << "/" << acc << ")";
      n[i] = ss.str();
   }
   return fR->TableRow(list_of(name).range(n));
}

string CbmLitTrackingQaStudyReport::PrintImageTable(
      const string& title,
      const string& file)
{
   int nofStudies = fStudyNames.size();
   int nofCols = 2;
   int nofLoops = ((nofStudies % nofCols) == 0) ? nofStudies / nofCols : nofStudies / nofCols + 1;
   stringstream ss;
   ss << fR->TableBegin(title, list_of(string("")).repeat(nofCols - 1, string("")));
   for (int iL = 0; iL < nofLoops; iL++) {
      std::vector<string> images;
      for (int iC = 0; iC < nofCols; iC++) {
         int index = nofCols * iL + iC;
         if (index >= nofStudies) continue;
         string fileName = fResultDirectories[index] + "/" + file;
         images.push_back(fR->Image(fStudyNames[index], fileName));
      }
      ss << fR->TableRow(images);
   }
   ss << fR->TableEnd();
   return ss.str();
}

string CbmLitTrackingQaStudyReport::PrintValue(
      int studyId,
      const string& valueName)
{
   stringstream ss;
   ss << fQa[studyId].get(valueName, -1.);
   return ss.str();
}
