/**
 * \file CbmLitQaHtmlReportElement.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaHtmlReportElement.h"
#include "utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

CbmLitQaHtmlReportElement::CbmLitQaHtmlReportElement()
{
}

CbmLitQaHtmlReportElement::~CbmLitQaHtmlReportElement()
{

}

string CbmLitQaHtmlReportElement::PrintTableBegin(
      const string& caption,
      const vector<string>& colNames)
{

   stringstream ss;
   ss << "<h2>" << caption << "</h2>";
   ss << "<table id=\"efficiency\" >" << endl;
   ss << "<tr><th></th>";
   for (int i = 0; i < colNames.size(); i++) {
      ss << "<th>" << colNames[i] << "</th>";
   }
   ss << "</tr>" << endl;
   return ss.str();
}



string CbmLitQaHtmlReportElement::PrintTableEnd()
{
   return "</table>";
}

string CbmLitQaHtmlReportElement::PrintEmptyRow(
      int nofCols,
      const string& name)
{

   stringstream ss;
   ss << "<td colspan=\"" << nofCols << "\">" << name << "</td></tr>" << endl;
   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintRow(
      const string& name1,
      const string& name2,
      const string& name3,
      const string& name4,
      const string& name5,
      const string& name6)
{
   stringstream ss;
   ss << "<tr>";
   ss << "<td>" << name1 << "</td>";
   ss << "<td>" << name2 << "</td>";
   if (name3 != "") ss << "<td>" << name3 << "</td>";
   if (name4 != "") ss << "<td>" << name4 << "</td>";
   if (name5 != "") ss << "<td>" << name5 << "</td>";
   if (name6 != "") ss << "<td>" << name6 << "</td>";

   ss << "</tr>" << endl;
   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintValue(
      const string& hist)
{
   stringstream ss;
   ss << fQa->get(hist, -1.);
   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintNofStatistics(
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

   if (mvd != "") { ss1 << (Int_t)fQa->get(mvd, -1.); }
      else { ss1 << "-"; }
   if (sts != "") { ss2 << (Int_t)fQa->get(sts, -1.); }
      else { ss2 << "-"; }
   if (rich != "") { ss3 << (Int_t)fQa->get(rich, -1.); }
      else { ss3 << "-"; }
   if (trd != "") { ss4 << (Int_t)fQa->get(trd, -1.); }
      else { ss4 << "-"; }
   if (muchP != "") { ss5 << (Int_t)fQa->get(muchP, -1.); }
      else { ss5 << "-"; }
   if (muchS != "") { ss6 << (Int_t)fQa->get(muchS, -1.); }
      else { ss6 << "-"; }
   if (tof!= "") { ss7 << (Int_t)fQa->get(tof, -1.); }
      else { ss7 << "-"; }

   ss << "<tr>";
   ss << "<td>" << name << "</td>";
   ss << "<td>" << ss1.str() << "</td>";
   ss << "<td>" << ss2.str() << "</td>";
   ss << "<td>" << ss3.str() << "</td>";
   ss << "<td>" << ss4.str() << "</td>";
   ss << "<td>" << ss5.str() << "</td>";
   ss << "<td>" << ss6.str() << "</td>";
   ss << "<td>" << ss7.str() << "</td>";
   ss << "</tr>" << endl;

   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintHits(
        const string& name,
        const string& hist)
{
   Double_t all = fQa->get(hist+".all", -1.);
   Double_t trueh = fQa->get(hist+".true", -1.);
   Double_t fakeh = fQa->get(hist+".fake", -1.);
   Double_t toa = fQa->get(hist+".trueOverAll", -1.);
   Double_t foa = fQa->get(hist+".fakeOverAll", -1.);

   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);

   if (all == -1.) ss1 << "-"; else ss1 << all;
   if (trueh == -1.) ss2 << "-"; else ss2 << trueh;
   if (fakeh == -1.) ss3 << "-"; else ss3 << fakeh;
   if (toa == -1.) ss4 << "-"; else ss4 << 100. * toa;
   if (foa == -1.) ss5 << "-"; else ss5 << 100. * foa;

   ss << "<tr>";
   ss << "<td>" << name << "</td>";
   ss << "<td>" << ss1.str() << "</td>";
   ss << "<td>" << ss2.str() << "</td>";
   ss << "<td>" << ss3.str() << "</td>";
   ss << "<td>" << ss4.str() << "</td>";
   ss << "<td>" << ss5.str() << "</td>";
   ss << "</tr>" << endl;

   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintEfficiency(
     const string& name,
     const string& hist)
{
   Double_t allRec = fQa->get(hist+".all.rec", -1.);
   Double_t allAcc = fQa->get(hist+".all.acc", -1.);
   Double_t allEff = fQa->get(hist+".all.eff", -1.);
   Double_t refRec = fQa->get(hist+".ref.rec", -1.);
   Double_t refAcc = fQa->get(hist+".ref.acc", -1.);
   Double_t refEff = fQa->get(hist+".ref.eff", -1.);
   Double_t primRec = fQa->get(hist+".prim.rec", -1.);
   Double_t primAcc = fQa->get(hist+".prim.acc", -1.);
   Double_t primEff = fQa->get(hist+".prim.eff", -1.);
   Double_t secRec = fQa->get(hist+".sec.rec", -1.);
   Double_t secAcc = fQa->get(hist+".sec.acc", -1.);
   Double_t secEff = fQa->get(hist+".sec.eff", -1.);
   Double_t muRec = fQa->get(hist+".mu.rec", -1.);
   Double_t muAcc = fQa->get(hist+".mu.acc", -1.);
   Double_t muEff = fQa->get(hist+".mu.eff", -1.);
   Double_t elRec = fQa->get(hist+".el.rec", -1.);
   Double_t elAcc = fQa->get(hist+".el.acc", -1.);
   Double_t elEff = fQa->get(hist+".el.eff", -1.);

   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);
   ss6.precision(3);

   ss1 << allEff << "<small>("<< allRec << "/" << allAcc << ")</small>";
   ss2 << refEff << "<small>("<< refRec << "/" << refAcc << ")</small>";
   ss3 << primEff << "<small>(" << primRec << "/" << primAcc << ")</small>";
   ss4 << secEff << "<small>(" << secRec << "/" << secAcc << ")</small>";
   ss5 << elEff << "<small>(" << elRec << "/" << elAcc << ")</small>";
   ss6 << muEff << "<small>(" << muRec << "/" << muAcc << ")</small>";

   ss << "<tr>";
   ss << "<td>" << name << "</td>";
   ss << "<td>" << ss1.str() << "</td>";
   ss << "<td>" << ss2.str() << "</td>";
   ss << "<td>" << ss3.str() << "</td>";
   ss << "<td>" << ss4.str() << "</td>";
   ss << "<td>" << ss5.str() << "</td>";
   ss << "<td>" << ss6.str() << "</td>";
   ss << "</tr>" << endl;

   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintEfficiencyRich(
      const string& name,
      const string& hist)
{
   Double_t allRec = fQa->get(hist+".All.rec", -1.);
   Double_t allAcc = fQa->get(hist+".All.acc", -1.);
   Double_t allEff = fQa->get(hist+".All.eff", -1.);
   Double_t allRefRec = fQa->get(hist+".AllRef.rec", -1.);
   Double_t allRefAcc = fQa->get(hist+".AllRef.acc", -1.);
   Double_t allRefEff = fQa->get(hist+".AllRef.eff", -1.);
   Double_t elRec = fQa->get(hist+".El.rec", -1.);
   Double_t elAcc = fQa->get(hist+".El.acc", -1.);
   Double_t elEff = fQa->get(hist+".El.eff", -1.);
   Double_t elRefRec = fQa->get(hist+".ElRef.rec", -1.);
   Double_t elRefAcc = fQa->get(hist+".ElRef.acc", -1.);
   Double_t elRefEff = fQa->get(hist+".ElRef.eff", -1.);
   Double_t piRec = fQa->get(hist+".Pi.rec", -1.);
   Double_t piAcc = fQa->get(hist+".Pi.acc", -1.);
   Double_t piEff = fQa->get(hist+".Pi.eff", -1.);
   Double_t piRefRec = fQa->get(hist+".PiRef.rec", -1.);
   Double_t piRefAcc = fQa->get(hist+".PiRef.acc", -1.);
   Double_t piRefEff = fQa->get(hist+".PiRef.eff", -1.);

   stringstream ss, ss1, ss2, ss3, ss4, ss5, ss6;
   ss1.precision(3);
   ss2.precision(3);
   ss3.precision(3);
   ss4.precision(3);
   ss5.precision(3);
   ss6.precision(3);

   ss1 << allEff << "<small>(" << allRec << "/" << allAcc << ")</small>";
   ss2 << allRefEff << "<small>(" << allRefRec << "/" << allRefAcc << ")</small>";
   ss3 << elEff << "<small>(" << elRec << "/" << elAcc << ")</small>";
   ss4 << elRefEff << "<small>(" << elRefRec << "/" << elRefAcc << ")</small>";
   ss5 << piEff << "<small>(" << piRec << "/" << piAcc << ")</small>";
   ss6 << piRefEff << "<small>(" << piRefRec << "/" << piRefAcc << ")</small>";

   ss << "<tr>";
   ss << "<td>" << name << "</td>";
   ss << "<td>" << ss1.str() << "</td>";
   ss << "<td>" << ss2.str() << "</td>";
   ss << "<td>" << ss3.str() << "</td>";
   ss << "<td>" << ss4.str() << "</td>";
   ss << "<td>" << ss5.str() << "</td>";
   ss << "<td>" << ss6.str() << "</td>";
   ss << "</tr>" << endl;

   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintEfficiencyElId(
      const string& name,
      const string& hist)
{
   Double_t elRec = fQa->get(hist+".el.rec", -1.);
   Double_t elAcc = fQa->get(hist+".el.acc", -1.);
   Double_t elEff = fQa->get(hist+".el.eff", -1.);
   Double_t piRec = fQa->get(hist+".pi.rec", -1.);
   Double_t piAcc = fQa->get(hist+".pi.acc", -1.);
   Double_t piSupp = fQa->get(hist+".pi.supp", -1.);

   stringstream ss, ss1, ss2;
   ss1.precision(3);
   ss2.precision(3);

   ss1 << elEff << "<small>("<< elRec << "/" << elAcc << ")</small>";
   ss2 << piSupp << "<small>("<< piAcc << "/" << piRec << ")</small>";

   ss << "<tr>";
   ss << "<td>" << name << "</td>";
   ss << "<td>" << ss1.str() << "</td>";
   ss << "<td>" << ss2.str() << "</td>";
   ss << "</tr>" << endl;

   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintDetAccEl(
      const string& name,
      const string& hist)
{
   Double_t acc = fQa->get(hist+".detAccAcc.acc", -1.);
   Double_t mc = fQa->get(hist+".detAccAcc.mc", -1.);
   Double_t effAcc = fQa->get(hist+".detAccAcc.eff", -1.);
   Double_t rec = fQa->get(hist+".detAccRec.rec", -1.);
  // Double_t mc = pt->get(hist+".detAccRec.mc", -1.);
   Double_t effRec = fQa->get(hist+".detAccRec.eff", -1.);

   stringstream ss, ss1, ss2;
   ss.precision(3);
   ss1.precision(3);
   ss2.precision(3);

   ss1 << effAcc << "<small>(" << acc << "/" << mc << ")</small>";
   ss2 << effRec << "<small>(" << rec << "/" << mc << ")</small>";
   ss << "<tr>";
   ss << "<td>" << name << "</td>";
   ss << "<td>" << ss1.str() << "</td>";
   ss << "<td>" << ss2.str() << "</td>";
   ss << "</tr>" << endl;

   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintPolarAngle(
      const string& hist)
{
   Double_t maxAngle = fQa->get("MaxAngle", -1.);
   Double_t minAngle = fQa->get("MinAngle", -1.);
   Double_t nofBinsAngle = fQa->get("NofBinsAngle", -1.);
   Double_t step = (maxAngle - minAngle) / nofBinsAngle;

   if (maxAngle == -1. || minAngle == -1. || nofBinsAngle == -1.){
      return "ERROR PolarAngleEfficiencyToString";
   }

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

      ss1 << fQa->get(hist+".all.eff."+angle0 +"_" + angle1, -1.)
          << "<small>(" << fQa->get(hist+".all.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".all.acc."+angle0 +"_" + angle1, -1.) << ")</small>";
      ss2 << fQa->get(hist+".ref.eff."+angle0 +"_" + angle1, -1.)
          << "<small>(" << fQa->get(hist+".ref.rec."+angle0 +"_" + angle1, -1.) << "/"
          << fQa->get(hist+".ref.acc."+angle0 +"_" + angle1, -1.) << ")</small>";
      ss3 << fQa->get(hist+".prim.eff."+angle0 +"_" + angle1, -1.)
          << "<small>(" << fQa->get(hist+".prim.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".prim.acc."+angle0 +"_" + angle1, -1.) << ")</small>";
      ss4 << fQa->get(hist+".sec.eff."+angle0 +"_" + angle1, -1.)
          << "<small>(" << fQa->get(hist+".sec.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".sec.acc."+angle0 +"_" + angle1, -1.) << ")</small>";
      ss5 << fQa->get(hist+".el.eff."+angle0 +"_" + angle1, -1.)
          << "<small>(" << fQa->get(hist+".el.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".el.acc."+angle0 +"_" + angle1, -1.) << ")</small>";
      ss6 << fQa->get(hist+".mu.eff."+angle0 +"_" + angle1, -1.)
          << "<small>(" << fQa->get(hist+".mu.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".mu.acc."+angle0 +"_" + angle1, -1.) << ")</small>";

      ss << "<tr>";
      ss << "<td>" << ss0.str() << "</td>";
      ss << "<td>" << ss1.str() << "</td>";
      ss << "<td>" << ss2.str() << "</td>";
      ss << "<td>" << ss3.str() << "</td>";
      ss << "<td>" << ss4.str() << "</td>";
      ss << "<td>" << ss5.str() << "</td>";
      ss << "<td>" << ss6.str() << "</td>";
      ss << "</tr>" << endl;

   }
   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintImage(
      const string& title,
      const string& file)
{
   stringstream ss;
   ss << "<h3>" << title << "</h3>";
   ss << "<img src=\"" << file << ".png\" alt=\"" << title << "\" />";

   return ss.str();
}

string CbmLitQaHtmlReportElement::PrintHead()
{
   string str = "<html><body><head><style type=\"text/css\">";
   str += "#efficiency";
   str += "{";
   str += "font-family:Verdana, Arial, Helvetica, sans-serif;";
//   str += "width:100%;";
   str += "border-collapse:collapse;";
   str += "}";
   str += "#efficiency td, #efficiency th";
   str += "{";
   str += "font-size:1em;";
   str += "border:1px solid #98bf21;";
   str += "padding:3px 7px 2px 7px;";
   str += "}";
   str += "#efficiency th";
   str += "{";
   str += "font-size:1.1em;";
   str += "text-align:left;";
   str += "padding-top:5px;";
   str += "padding-bottom:4px;";
   str += "background-color:#A7C942;";
   str += "color:#ffffff;";
   str += "}";
   str += "#efficiency tr.alt td";
   str += "{";
   str += "color:#000000;";
   str += "background-color:#EAF2D3;";
   str += "}";
   str += "</style></head>";
   return str;
}

string CbmLitQaHtmlReportElement::PrintCloseDocument()
{
   return "</body></html>";
}
