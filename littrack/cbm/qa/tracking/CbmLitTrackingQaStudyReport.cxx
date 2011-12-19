/**
 * \file CbmLitTrackingQaStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitTrackingQaStudyReport.h"
#include "../report/CbmLitReportElement.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;

CbmLitTrackingQaStudyReport::CbmLitTrackingQaStudyReport(
      LitReportType reportType) : CbmLitStudyReport(reportType)
{
}

CbmLitTrackingQaStudyReport::~CbmLitTrackingQaStudyReport()
{
}

void CbmLitTrackingQaStudyReport::Create(
      ostream& out,
      const vector<string>& studyNames,
      const vector<boost::property_tree::ptree*>& qa,
      boost::property_tree::ptree* ideal,
      const vector<boost::property_tree::ptree*>& check)
{
   fStudyNames = studyNames;
   fQa = qa;
   fIdeal = ideal;
   fCheck = check;

   out.precision(3);
   out << fR->DocumentBegin();
   out << (fTitle != "") ? fR->Title(0, fTitle) : string("");

   out << fR->TableBegin("Number of objects", list_of(string("")).range(fStudyNames));
   out << fR->TableRow(list_of("hEventNo")("Number of events"));

   if (fIsSts) {
      out << PrintRow("hNofStsTracks", "STS tracks");
   }

   if (fIsRich) {
      out << PrintRow("hNofRichProjections", "RICH projections");
      out << PrintRow("hNofRichRings", "RICH rings");
   }

   if (fIsTrd) {
      out << PrintRow("hNofTrdTracks", "TRD tracks");
   }

   if (fIsMuch) {
      out << PrintRow("hNofMuchTracks", "MUCH tracks");
   }

   out << PrintRow("hNofGlobalTracks", "Global tracks");
   out << fR->TableEnd() << endl;


   string signal = "el";
   if (!fIsElectronSetup) signal = "mu";
   string recDet = (fIsTrd) ? "TRD" : (fIsMuch) ? "MUCH" :
         (fIsMuch && fIsTrd) ? "MUCH+TRD" : "";

   out << fR->TableBegin("Reconstruction efficiency", fStudyNames);
   out << PrintRowEff("hSts3D.all", "STS all");
   out << PrintRowEff("hSts3D." + signal, "STS " + signal);
   out << PrintRowEff("hRec3D.all", recDet + " all");
   out << PrintRowEff("hRec3D." + signal, recDet + " " + signal);
   out << PrintRowEff("hTof3D.all", "TOF all");
   out << PrintRowEff("hTof3D." + signal, "TOF " + signal);

   out << fR->TableEmptyRow(fStudyNames.size()+1, "Normalization STS+" + recDet);
   out << PrintRowEff("hSts3DNormHalfGlobal.all", "STS all");
   out << PrintRowEff("hSts3DNormHalfGlobal." + signal, "STS " + signal);
   out << PrintRowEff("hHalfGlobal3D.all", "STS+" + recDet + " all");
   out << PrintRowEff("hHalfGlobal3D." + signal, "STS+" + recDet + " " + signal);

   out << fR->TableEmptyRow(fStudyNames.size()+1, "Normalization STS+" + recDet + "+TOF");
   out << PrintRowEff("hSts3DNormGlobal.all", "STS all");
   out << PrintRowEff("hSts3DNormGlobal." + signal, "STS " + signal);
   out << PrintRowEff("hHalfGlobal3DNormGlobal.all", "STS+" + recDet + " all");
   out << PrintRowEff("hHalfGlobal3DNormGlobal." + signal, "STS+" + recDet + " " + signal);
   out << PrintRowEff("hGlobal3D.all", "STS+" + recDet + "+TOF all");
   out << PrintRowEff("hGlobal3D.all", "STS+" + recDet + "+TOF " + signal);

   if (fIsRich) {
      out << fR->TableEmptyRow(fStudyNames.size()+1, "Efficiency with RICH");
      out << PrintRowEff("hRich3D.El", "RICH el");
      out << PrintRowEff("hRich3D.ElRef", "RICH el ref");

      out << fR->TableEmptyRow(fStudyNames.size()+1, "Normalization STS+RICH");
      out << PrintRowEff("hSts3DNormStsRich.El", "STS el");
      out << PrintRowEff("hStsRich3D.El", "STS+RICH el");

      out << fR->TableEmptyRow(fStudyNames.size()+1, "Normalization STS+RICH+TRD");
      out << PrintRowEff("hSts3DNormStsRichTrd.El", "STS el");
      out << PrintRowEff("hStsRich3DNormStsRichTrd.El", "STS+RICH el");
      out << PrintRowEff("hStsRichTrd3D.El", "STS+RICH+TRD el");

      out << fR->TableEmptyRow(fStudyNames.size()+1, "Normalization STS+RICH+TRD+TOF");
      out << PrintRowEff("hSts3DNormStsRichTrdTof.El", "STS el");
      out << PrintRowEff("hStsRich3DNormStsRichTrdTof.El", "STS+RICH el");
      out << PrintRowEff("hStsRichTrd3DNormStsRichTrdTof.El", "STS+RICH+TRD el");
      out << PrintRowEff("hStsRichTrdTof3D.El", "STS+RICH+TRD+TOF el");
   }
   out << fR->TableEnd() << endl;

   out << fR->TableBegin("Ghost rate", fStudyNames);
   out << PrintRow("fhStsGhostNh", "STS");
   out << PrintRow("fhRecGhostNh", recDet);
   if (fIsRich) {
      out << PrintRow("fhRichGhostNh", "RICH");
      out << fR->TableEmptyRow(fStudyNames.size()+1, "after STS-RICH matching");
      out << PrintRow("fhStsGhostRichMatchingNh", "STS");
      out << PrintRow("fhRichGhostStsMatchingNh", "RICH");
      out << fR->TableEmptyRow(fStudyNames.size()+1, "after STS-RICH matching and el identification");
      out << PrintRow("fhRichGhostElIdNh", "RICH");
   }
   out << fR->TableEnd() << endl;
   // For image paths put only file name without type, e.g. ".eps" or ".png".
   // Type will be added automatically.
   if (fIsSts) out << PrintImage("STS reconstruction efficiency", "rec_qa_sts_efficiency");
   if (fIsTrd || fIsMuch) out << PrintImage(recDet + " reconstruction efficiency", "rec_qa_rec_efficiency");
   if (fIsTof) out << PrintImage("TOF hit matching efficiency", "rec_qa_tof_efficiency");
   if (fIsRich) out << PrintImage("RICH efficiency electrons", "rec_qa_rich_efficiency_electrons");
   out << PrintImage("Global reconstruction efficiency", "rec_qa_global_efficiency_all");
   out << PrintImage("Global reconstruction efficiency for signal", "rec_qa_global_efficiency_signal");
   if (fIsRich) out << PrintImage("Global reconstruction efficiency with RICH", "rec_qa_sts_rich_trd_tof_efficiency_electrons");

   out <<  fR->DocumentEnd();
}

string CbmLitTrackingQaStudyReport::PrintRow(
      const string& property,
      const string& name)
{
   string n[7] = {"","","","","","",""};
   for (int i = 0; i < fStudyNames.size(); i++) {
      n[i] = PrintValue(i, property);
   }
   return fR->TableRow(list_of(name)(n[0])(n[1])(n[2])(n[3])(n[4])(n[5])(n[6]));
}

string CbmLitTrackingQaStudyReport::PrintRowEff(
      const string& property,
      const string& name)
{
   string n[7] = {"","","","","","",""};
   for (int i = 0; i < fStudyNames.size(); i++) {
      stringstream ss;
      string eff = PrintValue(i, property + ".eff");
      string acc = PrintValue(i, property + ".acc");
      string rec = PrintValue(i, property + ".rec");
      ss << eff << " ("<< rec << "/" << acc << ")";
      n[i] = ss.str();
   }
   return fR->TableRow(list_of(name)(n[0])(n[1])(n[2])(n[3])(n[4])(n[5])(n[6]));
}

string CbmLitTrackingQaStudyReport::PrintImage(
      const string& title,
      const string& file)
{
   stringstream ss;
   for (int i = 0; i < fStudyNames.size(); i++){
      string fileName = "./" + file; //fStudyResults[i] + "/" + file;
      ss << fR->Image(fStudyNames[i] + " " +title, fileName);
   }
   return ss.str();
}

string CbmLitTrackingQaStudyReport::PrintValue(
      int studyId,
      const string& valueName)
{
   stringstream ss;
   ss << fQa[studyId]->get(valueName, -1.);
   return ss.str();
}
