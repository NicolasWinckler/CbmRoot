/**
 * \file CbmLitFieldQaHTMLGenerator.cxx
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitFieldQaHTMLGenerator.h"
#include "../std/utils/CbmLitUtils.h"

CbmLitFieldQaHTMLGenerator::CbmLitFieldQaHTMLGenerator()
{

}

CbmLitFieldQaHTMLGenerator::~CbmLitFieldQaHTMLGenerator()
{

}

void CbmLitFieldQaHTMLGenerator::Create(
   std::ostream& out,
   const boost::property_tree::ptree* qa,
   const boost::property_tree::ptree* ideal,
   const boost::property_tree::ptree* check)
{
   fQa = qa;
   fIdeal = ideal;
   fCheck = check;

   int row = 0;

   out.precision(3);
   out << "<html><body>" << HtmlHeadString();

   out << "<h1>Magnetic field QA</h1>" << std::endl;

   out << PrintSummaryTable();

   // List all images
   out << "<h2>" << "List of images" << "</h2>";
   boost::property_tree::ptree pt = fQa->get_child("images.png");
   for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++) {
      boost::property_tree::ptree::value_type v = *it;
      std::string imageName = pt.get(v.first, "");
      out << "<a href=\"" << imageName << "\">" << imageName << "</a></br>";
   }

   out <<  "</body></html>";
}

std::string CbmLitFieldQaHTMLGenerator::PrintSummaryTable()
{
   std::stringstream ss;
   ss << "<h2>" << "Summary table" << "</h2>";
   ss << "<table id=\"efficiency\" >";

   int nofSlices = fQa->get("NofSlices", -1.);
   int nofPolynoms = fQa->get("NofPolynoms", -1.);

   int nerr = 4; // Number of errors for each component (abs mean, abs rms, rel mean, rel rms)
   ss << "<tr><th></th><th colspan=\"" << nerr << "\">Grid</th>";
   for (int i = 0; i < nofPolynoms; i++) {
      std::string pol = lit::ToString<Int_t>(i);
      int degree = fQa->get("slice0.polynomial" + pol + ".degree", -1.);
      ss << "<th colspan=\"" << nerr << "\">Polynomial " << degree << "</th>";
   }
   ss << "</tr>" << std::endl;

   std::string vnames[4] = {"BX", "BY", "BZ", "MOD"};
   std::string absMeanPoly[nofPolynoms], absRmsPoly[nofPolynoms],
         relMeanPoly[nofPolynoms], relRmsPoly[nofPolynoms];
   for (Int_t iSlice = 0; iSlice < nofSlices; iSlice++) {
      std::string slice = "slice" + lit::ToString<Int_t>(iSlice);
      std::string sliceZ = lit::ToString<float>(fQa->get(slice + ".Z", -1));
      int nSpanCols = 1 + (nofPolynoms + 1) * nerr;
      ss << PrintEmptyRow(0, nSpanCols, slice + " Z=" + sliceZ + " cm");
      std::string name = slice + ".grid";
      for (Int_t v = 0; v < 4; v++) {
         std::string absMean = name + ".err." + vnames[v] + ".abs.mean";
         std::string absRms = name + ".err." + vnames[v] + ".abs.rms";
         std::string relMean = name + ".err." + vnames[v] + ".rel.mean";
         std::string relRms = name + ".err." + vnames[v] + ".rel.rms";

         for (Int_t iPolynom = 0; iPolynom < nofPolynoms; iPolynom++) {
            std::string name = slice + ".polynomial" + lit::ToString<Int_t>(iPolynom);
            absMeanPoly[iPolynom] = name + ".err." + vnames[v] + ".abs.mean";
            absRmsPoly[iPolynom] = name + ".err." + vnames[v] + ".abs.rms";
            relMeanPoly[iPolynom] = name + ".err." + vnames[v] + ".rel.mean";
            relRmsPoly[iPolynom] = name + ".err." + vnames[v] + ".rel.rms";
         }
         ss << "<tr>";
         ss << "<td>" << vnames[v] << "</td>";
         ss << "<td>" << HtmlPrintValue("", absMean, "small") << "</td>"
               << "<td>" << HtmlPrintValue("", absRms, "small") << "</td>"
               << "<td>" << HtmlPrintValue("", relMean, "small") << "%</td>"
               << "<td>" << HtmlPrintValue("", relRms, "small") << "%</td>";
         for (Int_t iPolynom = 0; iPolynom < nofPolynoms; iPolynom++) {
            ss << "<td>" << HtmlPrintValue("", absMeanPoly[iPolynom], "small") << "</td>"
                  << "<td>" << HtmlPrintValue("", absRmsPoly[iPolynom], "small") << "</td>"
                  << "<td>" << HtmlPrintValue("", relMeanPoly[iPolynom], "small") << "%</td>"
                  << "<td>" << HtmlPrintValue("", relRmsPoly[iPolynom], "small") << "%</td>";
         }
         ss << "</tr>";
      }
   }

   ss << "</table>" << std::endl;
   return ss.str();
}

std::string CbmLitFieldQaHTMLGenerator::PrintEmptyRow(
      int row,
      int ncols,
      const std::string& name)
{
   std::stringstream ss;
   if ((row % 2) == 0) ss << "<tr>"; else ss << "<tr class = \"alt\">";
   ss << "<td colspan=\"" << ncols << "\">" << name << "</td></tr>" << std::endl;
   return ss.str();
}
