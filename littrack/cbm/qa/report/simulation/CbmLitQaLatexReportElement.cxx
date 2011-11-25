/**
 * \file CbmLitQaLatexReportElement.cxx
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitQaLatexReportElement.h"
#include "utils/CbmLitUtils.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

CbmLitQaLatexReportElement::CbmLitQaLatexReportElement()
{

}

CbmLitQaLatexReportElement::~CbmLitQaLatexReportElement()
{

}

string CbmLitQaLatexReportElement::PrintTableBegin(
      const string& caption,
      const vector<string>& colNames)
{
   stringstream ss;
   ss << "\\begin{table}[h]" << endl;
   ss << "\\centering";
   if (caption != "") ss << "\\caption{" << caption << "}" << endl;
   ss << "\\begin{tabular}{|";
   for (int i = 0; i < colNames.size() + 1; i++){
      ss << "c|";
   }
   ss << "} \\hline" << endl;
   // names start from the second column
   for (int i = 0; i < colNames.size(); i++) {
      ss << "& " << colNames[i];
   }
   ss << "\\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintTableEnd()
{
   return "\\end{tabular} \\end{table}";
}

string CbmLitQaLatexReportElement::PrintEmptyRow(
      int nofCols,
      const string& name)
{
   stringstream ss;
   ss << "\\multicolumn{" << nofCols << "}{|c|}{" << name << "} \\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintRow(
      const string& name1,
      const string& name2,
      const string& name3,
      const string& name4,
      const string& name5,
      const string& name6)
{
   stringstream ss;
   ss << name1 << " & " << name2;
   if (name3 != "") ss << " & " << name3;
   if (name4 != "") ss << " & " << name4;
   if (name5 != "") ss << " & " << name5;
   if (name6 != "") ss << " & " << name6;
   ss << " \\\\ \\hline";
   ss << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintValue(
      const string& hist)
{
   stringstream ss;
   ss << fQa->get(hist, -1.);
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintNofStatistics(
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

   ss << name << " & " << ss1.str() << " & " << ss2.str()
      << " & " << ss3.str() << " & " << ss4.str()
      << " & " << ss5.str() << " & " << ss6.str()
      << " & " << ss7.str();

   ss << " \\\\ \\hline" << endl;

   return ss.str();
}

string CbmLitQaLatexReportElement::PrintHits(
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

   ss << name << " & " << ss1.str() << " & " << ss2.str()
      << " & " << ss3.str() << " & " << ss4.str() << " & "
      << ss5.str() << endl;

   ss << " \\\\ \\hline" << endl;

   return ss.str();
}

string CbmLitQaLatexReportElement::PrintEfficiency(
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

   ss1 << allEff << "("<< allRec << "/" << allAcc << ")";
   ss2 << refEff << "("<< refRec << "/" << refAcc << ")";
   ss3 << primEff << "(" << primRec << "/" << primAcc << ")";
   ss4 << secEff << "(" << secRec << "/" << secAcc << ")";
   ss5 << elEff << "(" << elRec << "/" << elAcc << ")";
   ss6 << muEff << "(" << muRec << "/" << muAcc << ")";

   ss << name << " & " << ss1.str() << " & " << ss2.str()
      << " & " << ss3.str() << " & " << ss4.str()
      << " & " << ss5.str() << " & " << ss6.str() << endl;
   ss << " \\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintEfficiencyRich(
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

   ss1 << allEff << "(" << allRec << "/" << allAcc << ")";
   ss2 << allRefEff << "(" << allRefRec << "/" << allRefAcc << ")";
   ss3 << elEff << "(" << elRec << "/" << elAcc << ")";
   ss4 << elRefEff << "(" << elRefRec << "/" << elRefAcc << ")";
   ss5 << piEff << "(" << piRec << "/" << piAcc << ")";
   ss6 << piRefEff << "(" << piRefRec << "/" << piRefAcc << ")";

   ss << name << " & " << ss1.str() << " & " << ss2.str()
      << " & " << ss3.str() << " & " << ss4.str()
      << " & " << ss5.str() << " & " << ss6.str() << endl;
   ss << " \\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintEfficiencyElId(
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

   ss1 << elEff << "("<< elRec << "/" << elAcc << ")";
   ss2 << piSupp << "("<< piAcc << "/" << piRec << ")";

   ss << name << " & " << ss1.str() << " & " << ss2.str() << endl;
   ss << " \\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintDetAccEl(
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

   ss1 << effAcc << "(" << acc << "/" << mc << ")";
   ss2 << effRec << "(" << rec << "/" << mc << ")";

   ss << name << " & " << ss1.str() << " & " << ss2.str() << endl;
   ss << " \\\\ \\hline" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintPolarAngle(
      const string& hist)
{
   Double_t maxAngle = fQa->get("MaxAngle", -1.);
   Double_t minAngle = fQa->get("MinAngle", -1.);
   Double_t nofBinsAngle = fQa->get("NofBinsAngle", -1.);
   Double_t step = (maxAngle - minAngle) / nofBinsAngle;

   if (maxAngle == -1. || minAngle == -1. || nofBinsAngle == -1.){
      return "ERROR PrintPolarAngle";
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
          << "(" << fQa->get(hist+".all.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".all.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss2 << fQa->get(hist+".ref.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".ref.rec."+angle0 +"_" + angle1, -1.) << "/"
          << fQa->get(hist+".ref.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss3 << fQa->get(hist+".prim.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".prim.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".prim.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss4 << fQa->get(hist+".sec.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".sec.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".sec.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss5 << fQa->get(hist+".el.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".el.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".el.acc."+angle0 +"_" + angle1, -1.) << ")";
      ss6 << fQa->get(hist+".mu.eff."+angle0 +"_" + angle1, -1.)
          << "(" << fQa->get(hist+".mu.rec."+angle0 +"_" + angle1, -1.)
          << "/" << fQa->get(hist+".mu.acc."+angle0 +"_" + angle1, -1.) << ")";

      ss << ss0.str() << " & " << ss1.str()<< " & " << ss2.str()
         << " & " << ss3.str()<< " & " << ss4.str()
         << " & " << ss5.str()<< " & " << ss6.str() << endl;
      ss << " \\\\ \\hline" << endl;
   }

   return ss.str();
}

string CbmLitQaLatexReportElement::PrintImage(
      const string& title,
      const string& file)
{
   stringstream ss;

   ss << "\\begin{figure}[h]" << endl;
   ss << "\\centering" << endl;
   ss << "\\includegraphics[width=7cm]{" << file << ".eps}" << endl;
   ss << "\\caption{" << title << "}" << endl;
   ss << "\\end{figure}" << endl;
   return ss.str();
}

string CbmLitQaLatexReportElement::PrintHead()
{
   string str = "\\documentclass[a4paper,14pt]{article}";
   str += "\\usepackage{amssymb}";
   str += "\\setlength\\oddsidemargin{-2cm}";
   str += "\\setlength\\evensidemargin{-2cm}";
   str += "\\setlength\\textwidth{17cm}";
   str += "\\setlength\\topmargin{0cm}";
   str += "\\usepackage[dvips]{graphicx}";
   str += "\\begin{document}";

   return str;
}


string CbmLitQaLatexReportElement::PrintCloseDocument()
{
   return "\\end{document}";
}