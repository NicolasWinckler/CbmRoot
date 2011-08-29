/** CbmLitQaLatexGenerator.cxx
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#include "qa/CbmLitQaLatexGenerator.h"

#include <iostream>
//#include <sstream>
//#include <fstream>

#include "utils/CbmLitUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

void CbmLitQaLatexGenerator::PrintRowGroup(
      const std::string& groupName,
      Int_t nofRows,
      const std::string& rowName,
      const std::string& name1,
      const std::string& name2,
      const std::string& name3,
      const std::vector<boost::property_tree::ptree>& pts)
{
   std::cout << "\\multirow{"<<nofRows << "}{15mm}{\\begin{sideways}\\parbox{15mm}{"<<groupName
         << "}\\end{sideways}} & ";

   std::cout << rowName ;
   for (Int_t i = 0; i < pts.size(); i++){
      std::cout << " & " << ToStringAutoPrecision(pts[i].get(name1, -1.)) ;
      if (name2 != "" && name3 != ""){
         std::cout << "("<< ToStringAutoPrecision(pts[i].get(name2, -1.));
         std::cout << "/"<< ToStringAutoPrecision(pts[i].get(name3, -1.)) << ")";
      }
   }
   std::cout << " \\\\ \n" << std::endl;
}

void CbmLitQaLatexGenerator::PrintRow(
      const std::string& rowName,
      const std::string& name1,
      const std::string& name2,
      const std::string& name3,
      const std::vector<boost::property_tree::ptree>& pts)
{
   std::cout << " & ";
   std::cout << rowName ;
   for (Int_t i = 0; i < pts.size(); i++){
      std::cout << " & " << ToStringAutoPrecision(pts[i].get(name1, -1.));
      if (name2 != "" && name3 != ""){
         std::cout << "("<< ToStringAutoPrecision(pts[i].get(name2, -1.));
         std::cout << "/"<< ToStringAutoPrecision(pts[i].get(name3, -1.)) << ")";
      }
   }
   std::cout << " \\\\ \n " << std::endl;
}

std::string CbmLitQaLatexGenerator::ToStringAutoPrecision(Double_t d)
{
//   if (d < .05) return lit::ToString<Double_t>(d, 3);
//   if (d < 5.0) return lit::ToString<Double_t>(d, 2);
//   if (d < 100.0) return lit::ToString<Double_t>(d, 1);
//   return lit::ToString<Double_t>(d, 0);

   return lit::NumberToString<Double_t>(d, 1);
}

void CbmLitQaLatexGenerator::PrintLatexTable(
      const std::vector<std::string>& files)
{
   Int_t n = files.size();
   std::vector<boost::property_tree::ptree> pts;
   pts.resize(n);
   for (Int_t i = 0; i < n; i++) {
      read_json(files[i], pts[i]);
   }

   Int_t i = 1;
   PrintRow(lit::ToString<Int_t>((i++))+". nof events", "hEventNo", "", "", pts);

   PrintRowGroup("Nof hits, tracks", 9, lit::ToString<Int_t>((i++))+". global tracks", "hNofGlobalTracks", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". MVD hits", "hNofMvdHits", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS hits", "hNofStsHits", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS tracks", "hNofStsTracks", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". RICH hits", "hNofRichHits", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". RICH rings", "hNofRichRings", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". TRD hits", "hNofTrdHits", "", "",pts);
   PrintRow(lit::ToString<Int_t>((i++))+". TRD tracks", "hNofTrdTracks", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". TOF hits", "hNofTofHits", "", "", pts);
   std::cout << "\\hline" <<std::endl;

   PrintRowGroup("Local rec.",
         10, lit::ToString<Int_t>((i++))+". STS", "hStsMom.el.eff", "hStsMom.el.rec", "hStsMom.el.acc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". nof STS ghosts", "fhStsGhostNh", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". nof STS ghosts, matching", "fhStsGhostRichMatchingNh", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". RICH", "hRichMom.richEl.eff", "hRichMom.richEl.rec", "hRichMom.richEl.acc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". nof RICH ghosts", "fhRichGhostNh", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". nof RICH ghosts, matching", "fhRichGhostStsMatchingNh", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". nof RICH ghosts, el. id.", "fhRichGhostElIdNh", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". TRD", "hRecMom.el.eff", "hRecMom.el.rec", "hRecMom.el.acc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". nof TRD ghosts", "fhRecGhostNh", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". TOF matching", "hTofMom.el.eff", "hTofMom.el.rec", "hTofMom.el.acc", pts);
   std::cout << "\\hline" <<std::endl;

   PrintRowGroup("Global rec.",
         4, lit::ToString<Int_t>((i++))+". STS", "hStsMomNormStsRichTrdTof.richEl.eff", "hStsMomNormStsRichTrdTof.richEl.rec",
         "hStsMomNormStsRichTrdTof.richEl.acc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+RICH", "hStsRichMomNormStsRichTrdTof.richEl.eff",
         "hStsRichMomNormStsRichTrdTof.richEl.rec", "hStsRichMomNormStsRichTrdTof.richEl.acc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+RICH+TRD", "hStsRichTrdMomNormStsRichTrdTof.richEl.eff",
         "hStsRichTrdMomNormStsRichTrdTof.richEl.rec", "hStsRichTrdMomNormStsRichTrdTof.richEl.acc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+RICH+TRD+TOF", "hStsRichTrdTofMom.richEl.eff",
         "hStsRichTrdTofMom.richEl.rec", "hStsRichTrdTofMom.richEl.acc", pts);
   std::cout << "\\hline" <<std::endl;

   PrintRowGroup("El.id., $\\pi$ supp.",
         6, lit::ToString<Int_t>((i++))+". RICH", "hStsRichMomElIdNormStsRichTrdTof.el.eff",
         "hStsRichMomElIdNormStsRichTrdTof.el.rec", "hStsRichMomElIdNormStsRichTrdTof.el.acc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". RICH", "hStsRichMomElIdNormStsRichTrdTof.pi.supp",
         "hStsRichMomElIdNormStsRichTrdTof.pi.acc", "hStsRichMomElIdNormStsRichTrdTof.pi.rec", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". RICH+TRD", "hStsRichTrdMomElIdNormStsRichTrdTof.el.eff",
         "hStsRichTrdMomElIdNormStsRichTrdTof.el.rec", "hStsRichTrdMomElIdNormStsRichTrdTof.el.acc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". RICH+TRD", "hStsRichTrdMomElIdNormStsRichTrdTof.pi.supp",
         "hStsRichTrdMomElIdNormStsRichTrdTof.pi.acc", "hStsRichTrdMomElIdNormStsRichTrdTof.pi.rec", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". RICH+TRD+TOF", "hStsRichTrdTofMomElId.el.eff",
         "hStsRichTrdTofMomElId.el.rec", "hStsRichTrdTofMomElId.el.acc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". RICH+TRD+TOF", "hStsRichTrdTofMomElId.pi.supp",
         "hStsRichTrdTofMomElId.pi.acc", "hStsRichTrdTofMomElId.pi.rec", pts);
   std::cout << "\\hline" <<std::endl;

   PrintRowGroup("ACC/MC", 6,lit::ToString<Int_t>((i++))+". STS", "hStsDetAccEl.detAccAcc.eff",
         "hStsDetAccEl.detAccAcc.acc", "hStsDetAccEl.detAccAcc.mc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+RICH", "hStsRichDetAccEl.detAccAcc.eff",
         "hStsRichDetAccEl.detAccAcc.acc", "hStsRichDetAccEl.detAccAcc.mc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+TRD", "hStsTrdDetAccEl.detAccAcc.eff",
         "hStsTrdDetAccEl.detAccAcc.acc", "hStsTrdDetAccEl.detAccAcc.mc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+RICH+TRD", "hStsRichTrdDetAccEl.detAccAcc.eff",
         "hStsRichTrdDetAccEl.detAccAcc.acc", "hStsRichTrdDetAccEl.detAccAcc.mc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+RICH+TRD+TOF", "hStsRichTrdTofDetAccEl.detAccAcc.eff",
         "hStsRichTrdTofDetAccEl.detAccAcc.acc", "hStsRichTrdTofDetAccEl.detAccAcc.mc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+TRD+TOF", "hStsTrdTofDetAccEl.detAccAcc.eff",
         "hStsTrdTofDetAccEl.detAccAcc.acc", "hStsTrdTofDetAccEl.detAccAcc.mc", pts);
   std::cout << "\\hline" <<std::endl;

   PrintRowGroup("REC/MC", 6,lit::ToString<Int_t>((i++))+". STS", "hStsDetAccEl.detAccRec.eff",
         "hStsDetAccEl.detAccRec.rec", "hStsDetAccEl.detAccRec.mc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+RICH", "hStsRichDetAccEl.detAccRec.eff",
         "hStsRichDetAccEl.detAccRec.rec", "hStsRichDetAccEl.detAccRec.mc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+TRD", "hStsTrdDetAccEl.detAccRec.eff",
         "hStsTrdDetAccEl.detAccRec.rec", "hStsTrdDetAccEl.detAccRec.mc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+RICH+TRD", "hStsRichTrdDetAccEl.detAccRec.eff",
         "hStsRichTrdDetAccEl.detAccRec.rec", "hStsRichTrdDetAccEl.detAccRec.mc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+RICH+TRD+TOF", "hStsRichTrdTofDetAccEl.detAccRec.eff",
         "hStsRichTrdTofDetAccEl.detAccRec.rec", "hStsRichTrdTofDetAccEl.detAccRec.mc", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". STS+TRD+TOF", "hStsTrdTofDetAccEl.detAccRec.eff",
         "hStsTrdTofDetAccEl.detAccRec.rec", "hStsTrdTofDetAccEl.detAccRec.mc", pts);
   std::cout << "\\hline" <<std::endl;

   PrintRowGroup("STS qa", 4,lit::ToString<Int_t>((i++))+". $\\chi^2_{prim}$ mean", "fhStsChiprim.mean", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". $\\chi^2_{prim}$ rms", "fhStsChiprim.rms", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". $dP/P$ mean [\\%]", "fhStsMomresVsMom.mean", "", "", pts);
   PrintRow(lit::ToString<Int_t>((i++))+". $dP/P$ rms [\\%]", "fhStsMomresVsMom.rms", "", "", pts);
   std::cout << "\\hline" <<std::endl;
}
