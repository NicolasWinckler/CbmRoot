/** CbmLitQaPrintGenerator.cxx
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#include "qa/CbmLitQaPrintGenerator.h"

#include <iostream>
#include <map>
#include <cmath>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>

#include "utils/CbmLitUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>



void CbmLitQaPrintGenerator::PrintEventStatistics(
   std::ostream& out,
   boost::property_tree::ptree* pt)
{
   out << "-I- CbmLitReconstructionQaImpl event statistics:" << std::endl;
   out << "Number of events: " << pt->get("hEventNo", -1) << std::endl;

   // Print NOF statistics
   Int_t w = 12;
   out << std::right;
   out << std::setfill('_') << std::setw(8*w) << "_"<< std::endl;
   out << std::setfill(' ') << std::setw(w) << " " << std::setw(w)
      << "MVD" << std::setw(w) << "STS"<< std::setw(w) << "RICH"<< std::setw(w)
      << "TRD"<< std::setw(w) << "MUCH pix"<< std::setw(w) << "MUCH st" << std::setw(w)
      << "TOF"<< std::endl;
   out << std::setfill('_') << std::setw(8*w) << "_"<< std::endl;
   out << PrintNofStatisticsToString(pt, "Hits","hNofMvdHits", "hNofStsHits", "hNofRichHits",
         "hNofTrdHits", "hNofMuchPixelHits", "hNofMuchStrawHits", "hNofTofHits");
   out << PrintNofStatisticsToString(pt, "Tracks","", "hNofStsTracks", "hNofRichRings",
         "hNofTrdTracks", "hNofMuchTracks", "hNofMuchTracks", "");
   std::cout << std::endl;

   std::cout << "Number of reconstructed tracks: global=" << pt->get("hNofGlobalTracks", -1) << std::endl;

   // print reconstruction efficiency without RICH
   std::string det = "TRD(MUCH)";
   w = 17;
   std::cout << std::setfill('-') << std::setw(w) << "-" << std::setw(w)
	   << "all" << std::setw(w) << "reference" << std::setw(w) << "signal"<< std::endl;
   std::cout << std::setfill(' ');
   std::cout << std::left << std::setw(w) << "STS" << EventEfficiencyStatisticsToString(pt,"hStsMom", "event");
   std::cout << std::setw(w) << det << EventEfficiencyStatisticsToString(pt, "hRecMom", "event");
   std::cout << std::setw(w) << "TOF matching" << EventEfficiencyStatisticsToString(pt, "hTofMom", "event");
   std::string s = "STS+" + det;
   std::cout << std::setw(w) << s << EventEfficiencyStatisticsToString(pt,"hHalfGlobalMom", "event");
   std::string s2 = s + "+TOF";
   std::cout << std::setw(w) << s2 << EventEfficiencyStatisticsToString(pt, "hGlobalMom", "event");

   // print RICH reconstruction
   std::cout << std::right;
   std::cout << std::setfill('-') << std::setw(w) << "-" << std::setw(w)
	   << "all" << std::setw(w) << "electron"<< std::setw(w) << "electron ref" << std::endl;
   std::cout << std::setfill(' ') << std::left;
   std::cout << std::setw(w) << "RICH" << EventEfficiencyStatisticsRichToString(pt, "hRichMom", "event");
   std::cout << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(pt, "hStsRichMom", "event");
   std::cout << std::setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrdMom", "event");
   std::cout << std::setw(w) << "STS+RICH+TRD+TOF" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrdTofMom", "event");

   // detector acceptance efficiency
   w = 16;
   out << "Detector acceptance for primary electrons" << std::endl;
   out << std::setfill('_') << std::setw(3*w) << "_"<< std::endl;
   out << std::setfill(' ') << std::setw(w) << " " << std::setw(w)
       << std::right  << "kAcc/MC" << std::setw(w) << "kRec/MC"<< std::endl;
   out << std::setfill('_') << std::setw(3*w) << "_"<< std::endl;
   out << EventDetAccElStatisticsToString(pt, "STS", "hStsDetAccEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH","hStsRichDetAccEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH-TRD", "hStsRichTrdDetAccEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH-TRD-TOF", "hStsRichTrdTofDetAccEl");
   out << std::setfill('_') << std::setw(3*w) << "_"<< std::endl;
   out << std::endl;

   // ghost statistics
   out.precision(3);
   out << "Ghosts per events:";
   out << " STS=" << pt->get("fhStsGhostNh", -1.);
   out << " " << "TRD(MUCH)" << "=" << pt->get("fhRecGhostNh", -1.);
   out << " RICH=" << pt->get("fhRichGhostNh", -1.) << std::endl;
   out << "after STS-RICH matching: STS=" << pt->get("fhStsGhostRichMatchingNh", -1.);
   out << " RICH=" << pt->get("fhRichGhostStsMatchingNh", -1.) << std::endl;
   out << "after STS-RICH matching and el identification: RICH=" << pt->get("fhRichGhostElIdNh", -1.) << std::endl;
}


void CbmLitQaPrintGenerator::PrintFinalStatistics(
   std::ostream& out,
   boost::property_tree::ptree* pt)
{
   //fPt = pt;
   out << "-I- CbmLitReconstructionQaImpl final statistics:" << std::endl;
   out << "Number of events: " << pt->get("hEventNo", -1) << std::endl;

   // Print NOF statistics
   Int_t w = 12;
   out << std::right;
   out << std::setfill('_') << std::setw(8*w) << "_"<< std::endl;
   out << std::setfill(' ') << std::setw(w) << " " << std::setw(w)
      << "MVD" << std::setw(w) << "STS"<< std::setw(w) << "RICH"<< std::setw(w)
      << "TRD"<< std::setw(w) << "MUCH pix"<< std::setw(w) << "MUCH st" << std::setw(w)
      << "TOF"<< std::endl;

   out << std::setfill('_') << std::setw(8*w) << "_"<< std::endl;
   out << PrintNofStatisticsToString(pt, "Points","hNofMvdPoints", "hNofStsPoints", "hNofRichPoints",
         "hNofTrdPoints", "hNofMuchPoints", "hNofMuchPoints", "hNofTofPoints");
   out << PrintNofStatisticsToString(pt, "Digis", "hNofMvdDigis", "hNofStsDigis", "",
         "hNofTrdDigis", "hNofMuchDigis", "", "");
   out << PrintNofStatisticsToString(pt, "Clusters", "hNofMvdClusters", "hNofStsClusters", "",
         "hNofTrdClusters", "hNofMuchClusters", "", "");
   out << PrintNofStatisticsToString(pt, "Hits","hNofMvdHits", "hNofStsHits", "hNofRichHits",
         "hNofTrdHits", "hNofMuchPixelHits", "hNofMuchStrawHits", "hNofTofHits");
   out << PrintNofStatisticsToString(pt, "Tracks","", "hNofStsTracks", "hNofRichRings",
         "hNofTrdTracks", "hNofMuchTracks", "hNofMuchTracks", "");
   out << std::endl;
   out << "Number of global tracks per event: "<<(Int_t)pt->get("hNofGlobalTracks", -1.)<< std::endl;
   out << "Number of track projections in RICH: " <<(Int_t)pt->get("hNofRichProjections", -1.) << std::endl;

   //print hits histos statistics (nof all, true, fake hits in track/ring)
   w = 10;
   out << std::setfill('_') << std::setw(6*w) << "_"<< std::endl;
   out << std::setfill(' ') << std::setw(w) << " " << std::setw(w)
      << "all" << std::setw(w) << "true" << std::setw(w) << "fake"
      << std::setw(w) << "true/all"<< std::setw(w) << "fake/all" << std::endl;
   out << std::setfill('_') << std::setw(6*w) << "_"<< std::endl;
   out << std::setfill(' ');
   out << PrintHitsStatisticsToString(pt, "hMvdTrackHits", "MVD");
   out << PrintHitsStatisticsToString(pt, "hStsTrackHits", "STS");
   out << PrintHitsStatisticsToString(pt, "hTrdTrackHits", "TRD");
   out << PrintHitsStatisticsToString(pt, "hMuchTrackHits", "MUCH");
   out << PrintHitsStatisticsToString(pt, "hRichRingHits", "RICH");

   // print reconstruction efficiency without RICH
   w = 17;
   out << std::setfill('_') << std::setw(7*w) << "_"<< std::endl;
   out << std::setfill(' ') << std::setw(w) << " " << std::setw(w)
	   << "all" << std::setw(w) << "reference" << std::setw(w) << "primary"
	   << std::setw(w) << "secondary"<< std::setw(w) << "electron"<< std::setw(w) << "muon" << std::endl;
   out << std::setfill('_') << std::setw(7*17) << "_"<< std::endl;

   out << std::setfill(' ');
   out << std::left << std::setw(w) << "STS" << EventEfficiencyStatisticsToString(pt, "hStsMom", "final");
   out << std::setw(w) << "TRD(MUCH)" << EventEfficiencyStatisticsToString(pt, "hRecMom", "final");
   out << std::setw(w) << "TOF matching" << EventEfficiencyStatisticsToString(pt, "hTofMom", "final");

   out << std::setfill('-') << std::left;
   out << std::setw(7*17) << "Normalization STS+TRD(MUCH)" << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS" << EventEfficiencyStatisticsToString(pt, "hStsMomNormHalfGlobal", "final");
   std::string s = "STS+TRD(MUCH)";
   out << std::setw(w) << s << EventEfficiencyStatisticsToString(pt, "hHalfGlobalMom", "final");

   out << std::setfill('-') << std::left;
   out << std::setw(7*w) << "Normalization STS+TRD(MUCH)+TOF" << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS" << EventEfficiencyStatisticsToString(pt, "hStsMomNormGlobal", "final");
   out << std::setw(w) << s << EventEfficiencyStatisticsToString(pt, "hHalfGlobalMomNormGlobal", "final");
   std::string s2 = s + "+TOF";
   out << std::setw(w) << s2 << EventEfficiencyStatisticsToString(pt, "hGlobalMom", "final");
   out << std::setfill('_') << std::setw(7*w) << "_"<< std::endl;

   // print RICH reconstruction efficiency
   out << std::right;
   out << std::setfill('_') << std::setw(7*w) << "_"<< std::endl;
   out << std::setfill(' ') << std::setw(w) << " " << std::setw(w)
	   << "all" << std::setw(w) << "all reference" << std::setw(w) << "electron"
	   << std::setw(w) << "electron ref" << std::setw(w) << "pion"<< std::setw(w)
      << "pion ref" << std::endl;
   out << std::setfill('_') << std::setw(7*17) << "_"<< std::endl;

   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "RICH" << EventEfficiencyStatisticsRichToString(pt, "hRichMom", "final");

   out << std::setfill('-') << std::left;
   out << std::setw(7*w) << "Normalization STS+RICH " << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS" << EventEfficiencyStatisticsRichToString(pt, "hStsMomNormStsRich", "final");
   out << std::setw(w) << "STS+RICH no match" << EventEfficiencyStatisticsRichToString(pt, "hStsRichNoMatchingMom", "final");
   out << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(pt, "hStsRichMom", "final");

   out << std::setfill('-') << std::left;
   out << std::setw(7*w) << "Normalization STS+RICH+TRD " << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS" << EventEfficiencyStatisticsRichToString(pt, "hStsMomNormStsRichTrd", "final");
   out << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(pt, "hStsRichMomNormStsRichTrd", "final");
   out << std::setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrdMom", "final");

   out << std::setfill('-') << std::left;
   out << std::setw(7*w) << "Normalization STS+RICH+TRD+TOF " << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS" << EventEfficiencyStatisticsRichToString(pt, "hStsMomNormStsRichTrdTof", "final");
   out << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(pt, "hStsRichMomNormStsRichTrdTof", "final");
   out << std::setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrdMomNormStsRichTrdTof", "final");
   out << std::setw(w) << "STS+RICH+TRD+TOF" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrdTofMom", "final");
   out << std::setfill('_') << std::setw(7*17) << "_"<< std::endl;

   // print electron identification statistics
   out << std::setfill('_') << std::setw(3*w) << "_"<< std::endl;
   out << "Electron Identification" << std::endl;
   out << std::setfill('_') << std::setw(3*w) << "_"<< std::endl;
   out << std::setfill(' ') << std::right << std::setw(w) << " " << std::setw(w)
      << "efficiency" << std::setw(w) << "pion supp."<< std::endl;
   out << std::setfill('_') << std::setw(3*w) << "_"<< std::endl;

   out << std::setfill('-') << std::left;
   out << std::setw(3*w) << "Normalization STS+TRD" << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS+TRD" << EventEfficiencyStatisticsElIdToString(pt, "hStsTrdMomElId", "final");

   out << std::setfill('-') << std::left;
   out << std::setw(3*w) << "Normalization STS+TRD+TOF" << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS+TRD" << EventEfficiencyStatisticsElIdToString(pt, "hStsTrdMomElIdNormStsTrdTof", "final");
   out << std::setw(w) << "STS+TRD+TOF" << EventEfficiencyStatisticsElIdToString(pt, "hStsTrdTofMomElId", "final");

   out << std::setfill('-') << std::left;
   out << std::setw(3*w) << "Normalization STS+RICH " << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichMomElId", "final");

   out << std::setfill('-') << std::left;
   out << std::setw(3*w) << "Normalization STS+RICH+TRD " << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichMomElIdNormStsRichTrd", "final");
   out << std::setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichTrdMomElId", "final");

   out << std::setfill('-') << std::left;
   out << std::setw(3*w) << "Normalization STS+RICH+TRD+TOF " << std::endl;
   out << std::setfill(' ') << std::left;
   out << std::setw(w) << "STS+RICH" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichMomElIdNormStsRichTrdTof", "final");
   out << std::setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichTrdMomElIdNormStsRichTrdTof", "final");
   out << std::setw(w) << "STS+RICH+TRD+TOF" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichTrdTofMomElId", "final");
   out << std::setfill('_') << std::setw(3*w) << "_"<< std::endl;

   // detector acceptance efficiency
   w = 16;
   out << "Detector acceptance for primary electrons" << std::endl;
   out << std::setfill('_') << std::setw(3*w) << "_"<< std::endl;
   out << std::setfill(' ') << std::setw(w) << " " << std::setw(w)
       << std::right  << "kAcc/MC" << std::setw(w) << "kRec/MC"<< std::endl;
   out << std::setfill('_') << std::setw(3*w) << "_"<< std::endl;

   out << EventDetAccElStatisticsToString(pt, "STS", "hStsDetAccEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH","hStsRichDetAccEl");
   out << EventDetAccElStatisticsToString(pt, "STS-TRD", "hStsTrdDetAccEl");
   out << EventDetAccElStatisticsToString(pt, "STS-TOF", "hStsTofDetAccEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH-TRD", "hStsRichTrdDetAccEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH-TRD-TOF", "hStsRichTrdTofDetAccEl");
   out << EventDetAccElStatisticsToString(pt, "STS-TRD-TOF", "hStsTrdTofDetAccEl");
   out << std::setfill('_') << std::setw(3*w) << "_"<< std::endl;
   out << std::endl;

   // ghost statistics
   out.precision(3);
   out << "Ghosts per events:";
   out << " STS=" << pt->get("fhStsGhostNh", -1.);
   out << " " << "TRD(MUCH)" << "=" << pt->get("fhRecGhostNh", -1.);
   out << " RICH=" << pt->get("fhRichGhostNh", -1.) << std::endl;
   out << "after STS-RICH matching: STS=" << pt->get("fhStsGhostRichMatchingNh", -1.);
   out << " RICH=" << pt->get("fhRichGhostStsMatchingNh", -1.) << std::endl;
   out << "after STS-RICH matching and el identification: RICH=" << pt->get("fhRichGhostElIdNh", -1.) << std::endl;

   // STS quality numbers
   out << "Chi2 to primary vertex: mean = " << pt->get("fhStsChiprim.mean", -1.)
         << " RMS = " << pt->get("fhStsChiprim.rms", -1.) << std::endl;
   out << "Momentum resolution: mean = " << pt->get("fhStsMomresVsMom.mean", -1.)
         << " RMS = " << pt->get("fhStsMomresVsMom.rms", -1.) << std::endl;
   //track length
   out << "Track length 100%*(MC-Rec)/MC: mean = " << pt->get("fhTrackLength.mean", -1.)
         << " RMS = " << pt->get("fhTrackLength.rms", -1.) << std::endl;

   // Tracking efficiency vs. polar angle
   out << "Tracking efficiency in dependence on polar angle:" << std::endl;
   w = 17;
   out << std::setfill('_') << std::setw(7*w) << "_"<< std::endl;
   out << std::setfill(' ') << std::setw(w) << " " << std::setw(w)
      << "all" << std::setw(w) << "reference"
      << std::setw(w) << "primary"<< std::setw(w) << "secondary"
      << std::setw(w) << "electron"<< std::setw(w) << "muon" << std::endl;
   out << std::setfill('_') << std::setw(7*w) << "_"<< std::endl;

   out << std::setfill('-') << std::left << std::setw(7*w) << "STS" << std::endl;
   out << std::setfill(' ') << std::left << PolarAngleEfficiencyToString(pt, "hStsAngle");
   out << std::setfill('-') << std::left << std::setw(7*w) << "TRD(MUCH)" << std::endl;
   out << std::setfill(' ') << std::left << PolarAngleEfficiencyToString(pt, "hRecAngle");
   out << std::setfill('-') << std::left << std::setw(7*w) << "TOF" << std::endl;
   out << std::setfill(' ') << std::left << PolarAngleEfficiencyToString(pt, "hTofAngle");
   out << std::setfill('_') << std::setw(7*w) << "_" << std::endl;
}

std::string CbmLitQaPrintGenerator::PrintNofStatisticsToString(
   boost::property_tree::ptree* pt,
   const std::string& name,
   const std::string& mvd,
   const std::string& sts,
   const std::string& rich,
   const std::string& trd,
   const std::string& muchP,
   const std::string& muchS,
   const std::string& tof)
{
   std::stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6, ss7;
   Int_t w = 12;

   if (mvd != "") { ss1 << (Int_t)pt->get(mvd, -1.); }
      else { ss1 << "-"; }
   if (sts != "") { ss2 << (Int_t)pt->get(sts, -1.); }
      else { ss2 << "-"; }
   if (rich != "") { ss3 << (Int_t)pt->get(rich, -1.); }
      else { ss3 << "-"; }
   if (trd != "") { ss4 << (Int_t)pt->get(trd, -1.); }
      else { ss4 << "-"; }
   if (muchP != "") { ss5 << (Int_t)pt->get(muchP, -1.); }
      else { ss5 << "-"; }
   if (muchS != "") { ss6 << (Int_t)pt->get(muchS, -1.); }
      else { ss6 << "-"; }
   if (tof!= "") { ss7 << (Int_t)pt->get(tof, -1.); }
      else { ss7 << "-"; }

   ss << std::setw(w) << name <<
         std::setw(w) << ss1.str() << std::setw(w) << ss2.str() <<
         std::setw(w) << ss3.str() << std::setw(w) << ss4.str() <<
         std::setw(w) << ss5.str() << std::setw(w) << ss6.str() <<
         std::setw(w) << ss7.str() <<std::endl;

   return ss.str();
}

std::string CbmLitQaPrintGenerator::PrintHitsStatisticsToString(
   boost::property_tree::ptree* pt,
   const std::string& hist,
   const std::string& name)
{
   Double_t all = pt->get(hist+".all", -1.);
   Double_t trueh = pt->get(hist+".true", -1.);
   Double_t fakeh = pt->get(hist+".fake", -1.);
   Double_t toa = pt->get(hist+".trueOverAll", -1.);
   Double_t foa = pt->get(hist+".fakeOverAll", -1.);

   std::stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);
   Int_t w = 10;

   if (all == -1.) ss1 << "-"; else ss1 << all;
   if (trueh == -1.) ss2 << "-"; else ss2 << trueh;
   if (fakeh == -1.) ss3 << "-"; else ss3 << fakeh;
   if (toa == -1.) ss4 << "-"; else ss4 << 100. * toa;
   if (foa == -1.) ss5 << "-"; else ss5 << 100. * foa;

   ss << std::setw(w) << name << std::setw(w) << ss1.str() <<
         std::setw(w) << ss2.str() << std::setw(w) << ss3.str() <<
         std::setw(w) << ss4.str() << std::setw(w) << ss5.str() << std::endl;

   return ss.str();
}

std::string CbmLitQaPrintGenerator::EventEfficiencyStatisticsToString(
   boost::property_tree::ptree* pt,
   const std::string& name,
   const std::string& opt)
{
   Double_t allRec = pt->get(name+".all.rec", -1.);
   Double_t allAcc = pt->get(name+".all.acc", -1.);
   Double_t allEff = pt->get(name+".all.eff", -1.);
   Double_t refRec = pt->get(name+".ref.rec", -1.);
   Double_t refAcc = pt->get(name+".ref.acc", -1.);
   Double_t refEff = pt->get(name+".ref.eff", -1.);
   Double_t primRec = pt->get(name+".prim.rec", -1.);
   Double_t primAcc = pt->get(name+".prim.acc", -1.);
   Double_t primEff = pt->get(name+".prim.eff", -1.);
   Double_t secRec = pt->get(name+".sec.rec", -1.);
   Double_t secAcc = pt->get(name+".sec.acc", -1.);
   Double_t secEff = pt->get(name+".sec.eff", -1.);
   Double_t muRec = pt->get(name+".mu.rec", -1.);
   Double_t muAcc = pt->get(name+".mu.acc", -1.);
   Double_t muEff = pt->get(name+".mu.eff", -1.);
   Double_t elRec = pt->get(name+".el.rec", -1.);
   Double_t elAcc = pt->get(name+".el.acc", -1.);
   Double_t elEff = pt->get(name+".el.eff", -1.);

   std::stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);
   ss6.precision(3);
   Int_t w = 17;

   ss1 << allEff << "("<< allRec << "/" << allAcc << ")";
   ss2 << refEff << "("<< refRec << "/" << refAcc << ")";
   ss3 << primEff << "(" << primRec << "/" << primAcc << ")";
   ss4 << secEff << "(" << secRec << "/" << secAcc << ")";
   ss5 << elEff << "(" << elRec << "/" << elAcc << ")";
   ss6 << muEff << "(" << muRec << "/" << muAcc << ")";

   if (opt == "final"){
	   ss << std::setw(w) << ss1.str() << std::setw(w) << ss2.str() <<
			   std::setw(w) << ss3.str() << std::setw(w) << ss4.str() <<
			   std::setw(w) << ss5.str() << std::setw(w) << ss6.str() << std::endl;
   } else if (opt == "event"){
	   ss << std::setw(w) << ss1.str() << std::setw(w) << ss2.str();
	   if (pt->get("IsElectronSetup", true)){
		   ss << std::setw(w) << ss5.str() << std::endl;
	   }else {
		   ss << std::setw(w) << ss6.str() << std::endl;
	   }
   }

   return ss.str();
}

std::string CbmLitQaPrintGenerator::EventEfficiencyStatisticsRichToString(
   boost::property_tree::ptree* pt,
   const std::string& name,
   const std::string& opt)
{
   Double_t allRec = pt->get(name+".richAll.rec", -1.);
   Double_t allAcc = pt->get(name+".richAll.acc", -1.);
   Double_t allEff = pt->get(name+".richAll.eff", -1.);
   Double_t allRefRec = pt->get(name+".richAllRef.rec", -1.);
   Double_t allRefAcc = pt->get(name+".richAllRef.acc", -1.);
   Double_t allRefEff = pt->get(name+".richAllRef.eff", -1.);
   Double_t elRec = pt->get(name+".richEl.rec", -1.);
   Double_t elAcc = pt->get(name+".richEl.acc", -1.);
   Double_t elEff = pt->get(name+".richEl.eff", -1.);
   Double_t elRefRec = pt->get(name+".richElRef.rec", -1.);
   Double_t elRefAcc = pt->get(name+".richElRef.acc", -1.);
   Double_t elRefEff = pt->get(name+".richElRef.eff", -1.);
   Double_t piRec = pt->get(name+".richPi.rec", -1.);
   Double_t piAcc = pt->get(name+".richPi.acc", -1.);
   Double_t piEff = pt->get(name+".richPi.eff", -1.);
   Double_t piRefRec = pt->get(name+".richPiRef.rec", -1.);
   Double_t piRefAcc = pt->get(name+".richPiRef.acc", -1.);
   Double_t piRefEff = pt->get(name+".richPiRef.eff", -1.);

   std::stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);
   ss6.precision(3);
   Int_t w = 17;

   ss1 << allEff << "(" << allRec << "/" << allAcc << ")";
   ss2 << allRefEff << "(" << allRefRec << "/" << allRefAcc << ")";
   ss3 << elEff << "(" << elRec << "/" << elAcc << ")";
   ss4 << elRefEff << "(" << elRefRec << "/" << elRefAcc << ")";
   ss5 << piEff << "(" << piRec << "/" << piAcc << ")";
   ss6 << piRefEff << "(" << piRefRec << "/" << piRefAcc << ")";
   if (opt == "event"){
	   ss << std::setw(w) << ss1.str() << std::setw(w) << ss3.str()
			   << std::setw(w) << ss4.str() << std::endl;
   } else if (opt == "final") {
	   ss << std::setw(w) << ss1.str() << std::setw(w) << ss2.str()
	      << std::setw(w) << ss3.str() << std::setw(w) << ss4.str()
	      << std::setw(w) << ss5.str() << std::setw(w) << ss6.str() << std::endl;
   }

   return ss.str();
}

std::string CbmLitQaPrintGenerator::EventEfficiencyStatisticsElIdToString(
   boost::property_tree::ptree* pt,
   const std::string& name,
   const std::string& opt)
{
   Double_t elRec = pt->get(name+".el.rec", -1.);
   Double_t elAcc = pt->get(name+".el.acc", -1.);
   Double_t elEff = pt->get(name+".el.eff", -1.);
   Double_t piRec = pt->get(name+".pi.rec", -1.);
   Double_t piAcc = pt->get(name+".pi.acc", -1.);
   Double_t piSupp = pt->get(name+".pi.supp", -1.);

   std::stringstream ss, ss1, ss2;
   ss1.precision(3);
   ss2.precision(3);
   Int_t w = 17;

   ss1 << elEff << "("<< elRec << "/" << elAcc << ")";
   ss2 << piSupp << "("<< piAcc << "/" << piRec << ")";

   ss << std::setw(w) << ss1.str() << std::setw(w) << ss2.str() << std::endl;

   return ss.str();
}

std::string CbmLitQaPrintGenerator::EventDetAccElStatisticsToString(
   boost::property_tree::ptree* pt,
   const std::string& effName,
   const std::string& name)
{
   Double_t acc = pt->get(name+".detAccAcc.acc", -1.);
   Double_t mc = pt->get(name+".detAccAcc.mc", -1.);
   Double_t effAcc = pt->get(name+".detAccAcc.eff", -1.);
   Double_t rec = pt->get(name+".detAccRec.rec", -1.);
  // Double_t mc = pt->get(name+".detAccRec.mc", -1.);
   Double_t effRec = pt->get(name+".detAccRec.eff", -1.);

   std::stringstream ss, ss1, ss2;
   ss.precision(3);
   ss1.precision(3);
   ss2.precision(3);
   Int_t w = 16;

   ss1 << effAcc << "(" << acc << "/" << mc << ")";
   ss2 << effRec << "(" << rec << "/" << mc << ")";

   ss << std::left << std::setw(w) << effName << std::right << std::setw(w) << ss1.str()
         << std::setw(w) << ss2.str() << std::endl;

   return ss.str();
}

std::string CbmLitQaPrintGenerator::PolarAngleEfficiencyToString(
   boost::property_tree::ptree* pt,
   const std::string& name)
{
   Double_t maxAngle = pt->get("MaxAngle", -1.);
   Double_t minAngle = pt->get("MinAngle", -1.);
   Double_t nofBinsAngle = pt->get("NofBinsAngle", -1.);
   Double_t step = (maxAngle - minAngle) / nofBinsAngle;

   if (maxAngle == -1. || minAngle == -1. || nofBinsAngle == -1.){
      return "ERROR PolarAngleEfficiencyToString";
   }

   Int_t w = 17;
   std::stringstream ss;
   for (Int_t i = 0; i < nofBinsAngle; i++) {
      std::stringstream ss0, ss1, ss2, ss3, ss4, ss5, ss6;
      ss0.precision(3);
      ss1.precision(3);
      ss2.precision(3);
      ss3.precision(3);
      ss4.precision(3);
      ss5.precision(3);
      ss6.precision(3);

      std::stringstream ss7;
      ss7 << i*step;
      std::string angle0= ss7.str();
      std::stringstream ss8;
      ss8 << i*step + step;
      std::string angle1= ss8.str();
      ss0 << "(" << angle0 << "-" << angle1 << ")";

      ss1 << pt->get(name+".all.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".all.rec."+angle0 +"_" + angle1, -1.)
          << "/" << pt->get(name+".all.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss2 << pt->get(name+".ref.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".ref.rec."+angle0 +"_" + angle1, -1.) << "/"
          << pt->get(name+".ref.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss3 << pt->get(name+".prim.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".prim.rec."+angle0 +"_" + angle1, -1.)
          << "/" << pt->get(name+".prim.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss4 << pt->get(name+".sec.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".sec.rec."+angle0 +"_" + angle1, -1.)
          << "/" << pt->get(name+".sec.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss5 << pt->get(name+".el.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".el.rec."+angle0 +"_" + angle1, -1.)
          << "/" << pt->get(name+".el.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss6 << pt->get(name+".mu.eff."+angle0 +"_" + angle1, -1.)
          << "(" << pt->get(name+".mu.rec."+angle0 +"_" + angle1, -1.)
          << "/" << pt->get(name+".mu.acc."+angle0 +"_" + angle1, -1.) << ")";

      ss << std::setw(w) << ss0.str() << std::setw(w) << ss1.str()
         << std::setw(w) << ss2.str() << std::setw(w) << ss3.str()
         << std::setw(w) << ss4.str() << std::setw(w) << ss5.str()
         << std::setw(w) << ss6.str() << std::endl;
   }
   return ss.str();
}
