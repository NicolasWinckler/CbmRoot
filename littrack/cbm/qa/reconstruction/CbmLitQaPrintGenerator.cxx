/** CbmLitQaPrintGenerator.cxx
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2011
 * @version 1.0
 **/

#include "qa/reconstruction/CbmLitQaPrintGenerator.h"

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

using namespace std;

void CbmLitQaPrintGenerator::PrintEventStatistics(
   ostream& out,
   boost::property_tree::ptree* pt)
{
   out << "-I- CbmLitReconstructionQaImpl event statistics:" << std::endl;
   out << "Number of events: " << pt->get("hEventNo", -1) << std::endl;

   // Print NOF statistics
   Int_t w = 12;
   out << right;
   out << setfill('_') << setw(8*w) << "_"<< endl;
   out << setfill(' ') << setw(w) << " " << setw(w)
      << "MVD" << setw(w) << "STS"<< setw(w) << "RICH"<< setw(w)
      << "TRD"<< setw(w) << "MUCH pix"<< setw(w) << "MUCH st" << setw(w)
      << "TOF"<< endl;
   out << setfill('_') << setw(8*w) << "_"<< endl;
   out << PrintNofStatisticsToString(pt, "Hits","hNofMvdHits", "hNofStsHits", "hNofRichHits",
         "hNofTrdHits", "hNofMuchPixelHits", "hNofMuchStrawHits", "hNofTofHits");
   out << PrintNofStatisticsToString(pt, "Tracks","", "hNofStsTracks", "hNofRichRings",
         "hNofTrdTracks", "hNofMuchTracks", "hNofMuchTracks", "");
   cout << endl;

   cout << "Number of reconstructed tracks: global=" << pt->get("hNofGlobalTracks", -1) << endl;

   // print reconstruction efficiency without RICH
   string det = "TRD(MUCH)";
   w = 17;
   cout << setfill('-') << setw(w) << "-" << setw(w)
	   << "all" << setw(w) << "reference" << setw(w) << "signal"<< endl;
   cout << setfill(' ');
   cout << left << setw(w) << "STS" << EventEfficiencyStatisticsToString(pt,"hSts3D", "event");
   cout << setw(w) << det << EventEfficiencyStatisticsToString(pt, "hRec3D", "event");
   cout << setw(w) << "TOF matching" << EventEfficiencyStatisticsToString(pt, "hTof3D", "event");
   string s = "STS+" + det;
   cout << setw(w) << s << EventEfficiencyStatisticsToString(pt,"hHalfGlobal3D", "event");
   string s2 = s + "+TOF";
   cout << setw(w) << s2 << EventEfficiencyStatisticsToString(pt, "hGlobal3D", "event");

   // print RICH reconstruction
   cout << right;
   cout << setfill('-') << setw(w) << "-" << setw(w)
	   << "all" << setw(w) << "electron"<< setw(w) << "electron ref" << endl;
   cout << setfill(' ') << left;
   cout << setw(w) << "RICH" << EventEfficiencyStatisticsRichToString(pt, "hRich3D", "event");
   cout << setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(pt, "hStsRich3D", "event");
   cout << setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrd3D", "event");
   cout << setw(w) << "STS+RICH+TRD+TOF" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrdTof3D", "event");

   // detector acceptance efficiency
   w = 16;
   out << "Detector acceptance for primary electrons" << endl;
   out << setfill('_') << setw(3*w) << "_"<< endl;
   out << setfill(' ') << setw(w) << " " << setw(w)
       << right  << "kAcc/MC" << setw(w) << "kRec/MC"<< endl;
   out << setfill('_') << setw(3*w) << "_"<< endl;
   out << EventDetAccElStatisticsToString(pt, "STS", "hStsDetAcc3DEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH","hStsRichDetAcc3DEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH-TRD", "hStsRichTrdDetAcc3DEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH-TRD-TOF", "hStsRichTrdTofDetAcc3DEl");
   out << setfill('_') << setw(3*w) << "_"<< endl;
   out << endl;

   // ghost statistics
   out.precision(3);
   out << "Ghosts per events:";
   out << " STS=" << pt->get("fhStsGhostNh", -1.);
   out << " " << "TRD(MUCH)" << "=" << pt->get("fhRecGhostNh", -1.);
   out << " RICH=" << pt->get("fhRichGhostNh", -1.) << endl;
   out << "after STS-RICH matching: STS=" << pt->get("fhStsGhostRichMatchingNh", -1.);
   out << " RICH=" << pt->get("fhRichGhostStsMatchingNh", -1.) << endl;
   out << "after STS-RICH matching and el identification: RICH=" << pt->get("fhRichGhostElIdNh", -1.) << endl;
}


void CbmLitQaPrintGenerator::PrintFinalStatistics(
   ostream& out,
   boost::property_tree::ptree* pt)
{
   //fPt = pt;
   out << "-I- CbmLitReconstructionQaImpl final statistics:" << endl;
   out << "Number of events: " << pt->get("hEventNo", -1) << endl;

   // Print NOF statistics
   Int_t w = 12;
   out << right;
   out << setfill('_') << setw(8*w) << "_"<< endl;
   out << setfill(' ') << setw(w) << " " << setw(w) << "MVD" << setw(w) << "STS"<< setw(w) << "RICH"<< setw(w)
      << "TRD"<< setw(w) << "MUCH pix"<< setw(w) << "MUCH st" << setw(w) << "TOF"<< endl;

   out << setfill('_') << setw(8*w) << "_"<< endl;
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
   out << endl;
   out << "Number of global tracks per event: "<<(Int_t)pt->get("hNofGlobalTracks", -1.)<< endl;
   out << "Number of track projections in RICH: " <<(Int_t)pt->get("hNofRichProjections", -1.) << endl;

   //print hits histos statistics (nof all, true, fake hits in track/ring)
   w = 10;
   out << setfill('_') << setw(6*w) << "_"<< endl;
   out << setfill(' ') << setw(w) << " " << setw(w) << "all" << setw(w) << "true" << setw(w) << "fake"
      << setw(w) << "true/all"<< setw(w) << "fake/all" << endl;
   out << setfill('_') << setw(6*w) << "_"<< endl;
   out << setfill(' ');
   out << PrintHitsStatisticsToString(pt, "hMvdTrackHits", "MVD");
   out << PrintHitsStatisticsToString(pt, "hStsTrackHits", "STS");
   out << PrintHitsStatisticsToString(pt, "hTrdTrackHits", "TRD");
   out << PrintHitsStatisticsToString(pt, "hMuchTrackHits", "MUCH");
   out << PrintHitsStatisticsToString(pt, "hRichRingHits", "RICH");

   // print reconstruction efficiency without RICH
   w = 17;
   out << setfill('_') << setw(7*w) << "_"<< endl;
   out << setfill(' ') << setw(w) << " " << setw(w) << "all" << setw(w) << "reference"
      << setw(w) << "primary" << setw(w) << "secondary"<< setw(w) << "electron"<< setw(w) << "muon" << endl;
   out << setfill('_') << setw(7*17) << "_"<< endl;

   out << setfill(' ');
   out << left << setw(w) << "STS" << EventEfficiencyStatisticsToString(pt, "hSts3D");
   out << setw(w) << "TRD(MUCH)" << EventEfficiencyStatisticsToString(pt, "hRec3D");
   out << setw(w) << "TOF matching" << EventEfficiencyStatisticsToString(pt, "hTof3D");

   out << setfill('-') << left;
   out << setw(7*17) << "Normalization STS+TRD(MUCH)" << endl;
   out << setfill(' ') << left;
   out << setw(w) << "STS" << EventEfficiencyStatisticsToString(pt, "hSts3DNormHalfGlobal");
   string s = "STS+TRD(MUCH)";
   out << setw(w) << s << EventEfficiencyStatisticsToString(pt, "hHalfGlobal3D");

   out << setfill('-') << left;
   out << setw(7*w) << "Normalization STS+TRD(MUCH)+TOF" << endl;
   out << setfill(' ') << left;
   out << setw(w) << "STS" << EventEfficiencyStatisticsToString(pt, "hSts3DNormGlobal");
   out << setw(w) << s << EventEfficiencyStatisticsToString(pt, "hHalfGlobal3DNormGlobal");
   string s2 = s + "+TOF";
   out << setw(w) << s2 << EventEfficiencyStatisticsToString(pt, "hGlobal3D");
   out << setfill('_') << setw(7*w) << "_"<< endl;

   // print RICH reconstruction efficiency
   out << right;
   out << setfill('_') << setw(7*w) << "_"<< endl;
   out << setfill(' ') << setw(w) << " " << setw(w) << "all" << setw(w) << "all reference" << setw(w)
      << "electron" << setw(w) << "electron ref" << setw(w) << "pion"<< setw(w) << "pion ref" << endl;
   out << setfill('_') << setw(7*17) << "_"<< endl;

   out << setfill(' ') << left;
   out << setw(w) << "RICH" << EventEfficiencyStatisticsRichToString(pt, "hRich3D");

   out << setfill('-') << left;
   out << setw(7*w) << "Normalization STS+RICH " << endl;
   out << setfill(' ') << left;
   out << setw(w) << "STS" << EventEfficiencyStatisticsRichToString(pt, "hSts3DNormStsRich");
   out << setw(w) << "STS+RICH no match" << EventEfficiencyStatisticsRichToString(pt, "hStsRichNoMatching3D");
   out << setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(pt, "hStsRich3D");

   out << setfill('-') << left;
   out << setw(7*w) << "Normalization STS+RICH+TRD " << endl;
   out << setfill(' ') << left;
   out << setw(w) << "STS" << EventEfficiencyStatisticsRichToString(pt, "hSts3DNormStsRichTrd");
   out << setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(pt, "hStsRich3DNormStsRichTrd");
   out << setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrd3D");

   out << setfill('-') << left;
   out << setw(7*w) << "Normalization STS+RICH+TRD+TOF " << endl;
   out << setfill(' ') << left;
   out << setw(w) << "STS" << EventEfficiencyStatisticsRichToString(pt, "hSts3DNormStsRichTrdTof");
   out << setw(w) << "STS+RICH" << EventEfficiencyStatisticsRichToString(pt, "hStsRich3DNormStsRichTrdTof");
   out << setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrd3DNormStsRichTrdTof");
   out << setw(w) << "STS+RICH+TRD+TOF" << EventEfficiencyStatisticsRichToString(pt, "hStsRichTrdTof3D");
   out << setfill('_') << setw(7*17) << "_"<< endl;

   // print electron identification statistics
   out << setfill('_') << setw(3*w) << "_"<< endl;
   out << "Electron Identification" << endl;
   out << setfill('_') << setw(3*w) << "_"<< endl;
   out << setfill(' ') << right << setw(w) << " " << setw(w) << "efficiency" << setw(w) << "pion supp."<< endl;
   out << setfill('_') << setw(3*w) << "_"<< endl;

   out << setfill('-') << left;
   out << setw(3*w) << "Normalization STS+TRD" << endl;
   out << setfill(' ') << left;
   out << setw(w) << "STS+TRD" << EventEfficiencyStatisticsElIdToString(pt, "hStsTrd3DElId");

   out << setfill('-') << left;
   out << setw(3*w) << "Normalization STS+TRD+TOF" << endl;
   out << setfill(' ') << left;
   out << setw(w) << "STS+TRD" << EventEfficiencyStatisticsElIdToString(pt, "hStsTrd3DElIdNormStsTrdTof");
   out << setw(w) << "STS+TRD+TOF" << EventEfficiencyStatisticsElIdToString(pt, "hStsTrdTof3DElId");

   out << setfill('-') << left;
   out << setw(3*w) << "Normalization STS+RICH " << endl;
   out << setfill(' ') << left;
   out << setw(w) << "STS+RICH" << EventEfficiencyStatisticsElIdToString(pt, "hStsRich3DElId");

   out << setfill('-') << left;
   out << setw(3*w) << "Normalization STS+RICH+TRD " << endl;
   out << setfill(' ') << left;
   out << setw(w) << "STS+RICH" << EventEfficiencyStatisticsElIdToString(pt, "hStsRich3DElIdNormStsRichTrd");
   out << setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichTrd3DElId");

   out << setfill('-') << left;
   out << setw(3*w) << "Normalization STS+RICH+TRD+TOF " << endl;
   out << setfill(' ') << left;
   out << setw(w) << "STS+RICH" << EventEfficiencyStatisticsElIdToString(pt, "hStsRich3DElIdNormStsRichTrdTof");
   out << setw(w) << "STS+RICH+TRD" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichTrd3DElIdNormStsRichTrdTof");
   out << setw(w) << "STS+RICH+TRD+TOF" << EventEfficiencyStatisticsElIdToString(pt, "hStsRichTrdTof3DElId");
   out << setfill('_') << setw(3*w) << "_"<< endl;

   // detector acceptance efficiency
   w = 16;
   out << "Detector acceptance for primary electrons" << endl;
   out << setfill('_') << setw(3*w) << "_"<< endl;
   out << setfill(' ') << setw(w) << " " << setw(w) << right  << "kAcc/MC" << setw(w) << "kRec/MC"<< endl;
   out << setfill('_') << setw(3*w) << "_"<< endl;

   out << EventDetAccElStatisticsToString(pt, "STS", "hStsDetAcc3DEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH","hStsRichDetAcc3DEl");
   out << EventDetAccElStatisticsToString(pt, "STS-TRD", "hStsTrdDetAcc3DEl");
   out << EventDetAccElStatisticsToString(pt, "STS-TOF", "hStsTofDetAcc3DEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH-TRD", "hStsRichTrdDetAcc3DEl");
   out << EventDetAccElStatisticsToString(pt, "STS-RICH-TRD-TOF", "hStsRichTrdTofDetAcc3DEl");
   out << EventDetAccElStatisticsToString(pt, "STS-TRD-TOF", "hStsTrdTofDetAcc3DEl");
   out << setfill('_') << setw(3*w) << "_"<< endl;
   out << endl;

   // ghost statistics
   out.precision(3);
   out << "Ghosts per events:";
   out << " STS=" << pt->get("fhStsGhostNh", -1.);
   out << " " << "TRD(MUCH)" << "=" << pt->get("fhRecGhostNh", -1.);
   out << " RICH=" << pt->get("fhRichGhostNh", -1.) << endl;
   out << "after STS-RICH matching: STS=" << pt->get("fhStsGhostRichMatchingNh", -1.);
   out << " RICH=" << pt->get("fhRichGhostStsMatchingNh", -1.) << endl;
   out << "after STS-RICH matching and el identification: RICH=" << pt->get("fhRichGhostElIdNh", -1.) << endl;

   // STS quality numbers
   out << "Chi2 to primary vertex: mean = " << pt->get("fhStsChiprim.mean", -1.)
         << " RMS = " << pt->get("fhStsChiprim.rms", -1.) << endl;
   out << "Momentum resolution: mean = " << pt->get("fhStsMomresVsMom.mean", -1.)
         << " RMS = " << pt->get("fhStsMomresVsMom.rms", -1.) << endl;
   //track length
   out << "Track length 100%*(MC-Rec)/MC: mean = " << pt->get("fhTrackLength.mean", -1.)
         << " RMS = " << pt->get("fhTrackLength.rms", -1.) << endl;

   // Tracking efficiency vs. polar angle
   out << "Tracking efficiency in dependence on polar angle:" << endl;
   w = 17;
   out << setfill('_') << setw(7*w) << "_"<< endl;
   out << setfill(' ') << setw(w) << " " << setw(w)
      << "all" << setw(w) << "reference"
      << setw(w) << "primary"<< setw(w) << "secondary"
      << setw(w) << "electron"<< setw(w) << "muon" << endl;
   out << setfill('_') << setw(7*w) << "_"<< endl;

   out << setfill('-') << left << setw(7*w) << "STS" << endl;
   out << setfill(' ') << left << PolarAngleEfficiencyToString(pt, "hStsAngle");
   out << setfill('-') << left << setw(7*w) << "TRD(MUCH)" << endl;
   out << setfill(' ') << left << PolarAngleEfficiencyToString(pt, "hRecAngle");
   out << setfill('-') << left << setw(7*w) << "TOF" << endl;
   out << setfill(' ') << left << PolarAngleEfficiencyToString(pt, "hTofAngle");
   out << setfill('_') << setw(7*w) << "_" << endl;
}

string CbmLitQaPrintGenerator::PrintNofStatisticsToString(
   boost::property_tree::ptree* pt,
   const string& name,
   const string& mvd,
   const string& sts,
   const string& rich,
   const string& trd,
   const string& muchP,
   const string& muchS,
   const string& tof)
{
   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6, ss7;
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

   ss << setw(w) << name <<
         setw(w) << ss1.str() << setw(w) << ss2.str() <<
         setw(w) << ss3.str() << setw(w) << ss4.str() <<
         setw(w) << ss5.str() << setw(w) << ss6.str() <<
         setw(w) << ss7.str() <<endl;

   return ss.str();
}

string CbmLitQaPrintGenerator::PrintHitsStatisticsToString(
   boost::property_tree::ptree* pt,
   const string& hist,
   const string& name)
{
   Double_t all = pt->get(hist+".all", -1.);
   Double_t trueh = pt->get(hist+".true", -1.);
   Double_t fakeh = pt->get(hist+".fake", -1.);
   Double_t toa = pt->get(hist+".trueOverAll", -1.);
   Double_t foa = pt->get(hist+".fakeOverAll", -1.);

   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
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

   ss << setw(w) << name << setw(w) << ss1.str() <<
         setw(w) << ss2.str() << setw(w) << ss3.str() <<
         setw(w) << ss4.str() << setw(w) << ss5.str() << endl;

   return ss.str();
}

string CbmLitQaPrintGenerator::EventEfficiencyStatisticsToString(
   boost::property_tree::ptree* pt,
   const string& name,
   const string& opt)
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

   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
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
	   ss << setw(w) << ss1.str() << setw(w) << ss2.str() <<
			   setw(w) << ss3.str() << setw(w) << ss4.str() <<
			   setw(w) << ss5.str() << setw(w) << ss6.str() << endl;
   } else if (opt == "event"){
	   ss << setw(w) << ss1.str() << setw(w) << ss2.str();
	   if (pt->get("IsElectronSetup", true)){
		   ss << setw(w) << ss5.str() << endl;
	   }else {
		   ss << setw(w) << ss6.str() << endl;
	   }
   }

   return ss.str();
}

string CbmLitQaPrintGenerator::EventEfficiencyStatisticsRichToString(
   boost::property_tree::ptree* pt,
   const string& name,
   const string& opt)
{
   Double_t allRec = pt->get(name+".All.rec", -1.);
   Double_t allAcc = pt->get(name+".All.acc", -1.);
   Double_t allEff = pt->get(name+".All.eff", -1.);
   Double_t allRefRec = pt->get(name+".AllRef.rec", -1.);
   Double_t allRefAcc = pt->get(name+".AllRef.acc", -1.);
   Double_t allRefEff = pt->get(name+".AllRef.eff", -1.);
   Double_t elRec = pt->get(name+".El.rec", -1.);
   Double_t elAcc = pt->get(name+".El.acc", -1.);
   Double_t elEff = pt->get(name+".El.eff", -1.);
   Double_t elRefRec = pt->get(name+".ElRef.rec", -1.);
   Double_t elRefAcc = pt->get(name+".ElRef.acc", -1.);
   Double_t elRefEff = pt->get(name+".ElRef.eff", -1.);
   Double_t piRec = pt->get(name+".Pi.rec", -1.);
   Double_t piAcc = pt->get(name+".Pi.acc", -1.);
   Double_t piEff = pt->get(name+".Pi.eff", -1.);
   Double_t piRefRec = pt->get(name+".PiRef.rec", -1.);
   Double_t piRefAcc = pt->get(name+".PiRef.acc", -1.);
   Double_t piRefEff = pt->get(name+".PiRef.eff", -1.);

   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
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
	   ss << setw(w) << ss1.str() << setw(w) << ss3.str()
			   << setw(w) << ss4.str() << endl;
   } else if (opt == "final") {
	   ss << setw(w) << ss1.str() << setw(w) << ss2.str()
	      << setw(w) << ss3.str() << setw(w) << ss4.str()
	      << setw(w) << ss5.str() << setw(w) << ss6.str() << endl;
   }

   return ss.str();
}

string CbmLitQaPrintGenerator::EventEfficiencyStatisticsElIdToString(
   boost::property_tree::ptree* pt,
   const string& name,
   const string& opt)
{
   Double_t elRec = pt->get(name+".el.rec", -1.);
   Double_t elAcc = pt->get(name+".el.acc", -1.);
   Double_t elEff = pt->get(name+".el.eff", -1.);
   Double_t piRec = pt->get(name+".pi.rec", -1.);
   Double_t piAcc = pt->get(name+".pi.acc", -1.);
   Double_t piSupp = pt->get(name+".pi.supp", -1.);

   stringstream ss, ss1, ss2;
   ss1.precision(3);
   ss2.precision(3);
   Int_t w = 17;

   ss1 << elEff << "("<< elRec << "/" << elAcc << ")";
   ss2 << piSupp << "("<< piAcc << "/" << piRec << ")";

   ss << setw(w) << ss1.str() << setw(w) << ss2.str() << endl;

   return ss.str();
}

string CbmLitQaPrintGenerator::EventDetAccElStatisticsToString(
   boost::property_tree::ptree* pt,
   const string& effName,
   const string& name)
{
   Double_t acc = pt->get(name+".detAccAcc.acc", -1.);
   Double_t mc = pt->get(name+".detAccAcc.mc", -1.);
   Double_t effAcc = pt->get(name+".detAccAcc.eff", -1.);
   Double_t rec = pt->get(name+".detAccRec.rec", -1.);
  // Double_t mc = pt->get(name+".detAccRec.mc", -1.);
   Double_t effRec = pt->get(name+".detAccRec.eff", -1.);

   stringstream ss, ss1, ss2;
   ss.precision(3);
   ss1.precision(3);
   ss2.precision(3);
   Int_t w = 16;

   ss1 << effAcc << "(" << acc << "/" << mc << ")";
   ss2 << effRec << "(" << rec << "/" << mc << ")";

   ss << left << setw(w) << effName << right << setw(w) << ss1.str()
         << setw(w) << ss2.str() << endl;

   return ss.str();
}

string CbmLitQaPrintGenerator::PolarAngleEfficiencyToString(
   boost::property_tree::ptree* pt,
   const string& name)
{
   Double_t maxAngle = pt->get("MaxAngle", -1.);
   Double_t minAngle = pt->get("MinAngle", -1.);
   Double_t nofBinsAngle = pt->get("NofBinsAngle", -1.);
   Double_t step = (maxAngle - minAngle) / nofBinsAngle;

   if (maxAngle == -1. || minAngle == -1. || nofBinsAngle == -1.){
      return "ERROR PolarAngleEfficiencyToString";
   }

   Int_t w = 17;
   stringstream ss;
   for (Int_t i = 0; i < nofBinsAngle; i++) {
      stringstream ss0, ss1, ss2, ss3, ss4, ss5, ss6;
      ss0.precision(3);
      ss1.precision(3);
      ss2.precision(3);
      ss3.precision(3);
      ss4.precision(3);
      ss5.precision(3);
      ss6.precision(3);

      stringstream ss7;
      ss7 << i*step;
      string angle0= ss7.str();
      stringstream ss8;
      ss8 << i*step + step;
      string angle1= ss8.str();
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

      ss << setw(w) << ss0.str() << setw(w) << ss1.str()
         << setw(w) << ss2.str() << setw(w) << ss3.str()
         << setw(w) << ss4.str() << setw(w) << ss5.str()
         << setw(w) << ss6.str() << endl;
   }
   return ss.str();
}
