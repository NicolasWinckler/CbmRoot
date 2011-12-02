/**
 * \file CbmLitFieldQaReport.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitFieldQaReport.h"
#include "../report/CbmLitReportElement.h"
#include "../../../std/utils/CbmLitUtils.h"
#include <assert.h>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>
using boost::assign::list_of;
using boost::assign::push_back;
using lit::ToString;

CbmLitFieldQaReport::CbmLitFieldQaReport(
      LitReportType reportType) : CbmLitSimulationReport(reportType)
{

}

CbmLitFieldQaReport::~CbmLitFieldQaReport()
{

}

void CbmLitFieldQaReport::Create(
   std::ostream& out,
   boost::property_tree::ptree* qa,
   boost::property_tree::ptree* ideal,
   boost::property_tree::ptree* check)
{
   fQa = qa;
   fIdeal = ideal;
   fCheck = check;

   out.precision(3);
   out << fR->DocumentBegin() << std::endl;
   out << fR->Title(0, "Magnetic field QA") << std::endl;
   out << PrintSummaryTable();

   // List all images
   out << fR->Title(1, "List of images");
   boost::property_tree::ptree pt = fQa->get_child("images.png");
   for (boost::property_tree::ptree::const_iterator it = pt.begin(); it != pt.end(); it++) {
      boost::property_tree::ptree::value_type v = *it;
      std::string imageName = pt.get(v.first, "");
      out << fR->Image(imageName, imageName);
   }

   out << fR->DocumentEnd();
}

std::string CbmLitFieldQaReport::PrintSummaryTable()
{
   std::stringstream ss;

   int nofSlices = fQa->get("NofSlices", -1.);
   assert(nofSlices > 0);
   int nofPolynoms = fQa->get("NofPolynoms", -1.);
   assert(nofPolynoms > 0);

   const Int_t nerr = 4; // number of errors [absMean, absRMS, relMean, relRMS]
   std::vector<string> colNames = list_of("").repeat(nerr, "Grid");
   for (int i = 0; i < nofPolynoms; i++) {
      std::string pol = ToString<int>(i);
      int degree = fQa->get("slice0.polynomial" + pol + ".degree", -1.);
      assert(degree > 0);
      std::string colName = "Polynomial" + ToString<int>(degree);
      push_back(colNames).repeat(nerr, colName);
   }
   ss << fR->TableBegin("Summary table", colNames);

   std::string vnames[4] = {"BX", "BY", "BZ", "MOD"};
   for (Int_t iSlice = 0; iSlice < nofSlices; iSlice++) {
      std::string slice = "slice" + ToString<Int_t>(iSlice);
      std::string sliceZ = ToString<float>(fQa->get(slice + ".Z", -1));
      int nSpanCols = 1 + (nofPolynoms + 1) * nerr;
      ss << fR->TableEmptyRow(nSpanCols, slice + " Z=" + sliceZ + " cm");
      std::string prefix = slice + ".grid";
      for (Int_t v = 0; v < 4; v++) {
         std::vector<std::string> row;
         row.push_back(vnames[v]);
         row.push_back(ToString<float>(fQa->get(prefix + ".err." + vnames[v] + ".abs.mean", -1.)));
         row.push_back(ToString<float>(fQa->get(prefix + ".err." + vnames[v] + ".abs.rms", -1.)));
         row.push_back(ToString<float>(fQa->get(prefix + ".err." + vnames[v] + ".rel.mean", -1.)));
         row.push_back(ToString<float>(fQa->get(prefix + ".err." + vnames[v] + ".rel.rms", -1.)));

         for (Int_t iPolynom = 0; iPolynom < nofPolynoms; iPolynom++) {
            std::string prefix = slice + ".polynomial" + ToString<Int_t>(iPolynom);
            row.push_back(ToString<float>(fQa->get(prefix + ".err." + vnames[v] + ".abs.mean", -1.)));
            row.push_back(ToString<float>(fQa->get(prefix + ".err." + vnames[v] + ".abs.rms", -1.)));
            row.push_back(ToString<float>(fQa->get(prefix + ".err." + vnames[v] + ".rel.mean", -1.)));
            row.push_back(ToString<float>(fQa->get(prefix + ".err." + vnames[v] + ".rel.rms", -1.)));
         }
         fR->TableRow(row);
      }
   }

   return ss.str();
}
