/**
 * \file CbmRichGeoTestStudyReport.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2012
 */
#include "CbmRichGeoTestStudyReport.h"
#include "CbmReportElement.h"

#include "TSystem.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/assign/list_of.hpp>
using boost::assign::list_of;
using std::endl;
using std::stringstream;

CbmRichGeoTestStudyReport::CbmRichGeoTestStudyReport()
{
}

CbmRichGeoTestStudyReport::~CbmRichGeoTestStudyReport()
{
}

void CbmRichGeoTestStudyReport::Create(
      ostream& out)
{
   out.precision(3);
   out << fR->DocumentBegin();
   out << fR->Title(0, fTitle);

   out << fR->TableBegin("Efficiencies", list_of(string("")).range(fStudyNames));
   out << PrintRow("acc_mean", "ACC");
   out << PrintRow("circle_fit_eff_mean", "Circle fit");
   out << PrintRow("ellipse_fit_eff_mean", "Ellipse fit");
   out << fR->TableEnd() << endl;

   out << fR->TableBegin("Nof point and hits in ring", list_of(string("")).range(fStudyNames));
   out << PrintRow("nof_points_in_ring_mean", "Nof points, mean");
   out << PrintRow("nof_points_in_ring_rms", "Nof points, rms");
   out << PrintRow("nof_hits_in_ring_mean", "Nof hits, mean");
   out << PrintRow("nof_hits_in_ring_rms", "Nof hits, rms");
   out << fR->TableEnd() << endl;

   out << fR->TableBegin("Point fit", list_of(string("")).range(fStudyNames));
   out << PrintRow("points_fit_a_axis_mean", "A axis, mean");
   out << PrintRow("points_fit_a_axis_rms", "A axis, rms");
   out << PrintRow("points_fit_b_axis_mean", "B axis, mean");
   out << PrintRow("points_fit_b_axis_rms", "B axis, rms");
   out << PrintRow("points_fit_boa_mean", "B/A, mean");
   out << PrintRow("points_fit_boa_rms", "B/A, rms");
   out << PrintRow("points_fit_r_mean", "Radius, mean");
   out << PrintRow("points_fit_r_rms", "Radius, mean");
   out << fR->TableEnd() << endl;

   out << fR->TableBegin("Hit fit", list_of(string("")).range(fStudyNames));
   out << PrintRow("hits_fit_a_axis_mean", "A axis, mean");
   out << PrintRow("hits_fit_a_axis_rms", "A axis, rms");
   out << PrintRow("hits_fit_b_axis_mean", "B axis, mean");
   out << PrintRow("hits_fit_b_axis_rms", "B axis, rms");
   out << PrintRow("hits_fit_boa_mean", "B/A, mean");
   out << PrintRow("hits_fit_boa_rms", "B/A, rms");
   out << PrintRow("hits_fit_r_mean", "Radius, mean");
   out << PrintRow("hits_fit_r_rms", "Radius, mean");
   out << fR->TableEnd() << endl;

   out << fR->TableBegin("Point fit vs. hit fit. Ellipse.", list_of(string("")).range(fStudyNames));
   out << PrintRow("diff_ellipse_da_mean", "dA, mean");
   out << PrintRow("diff_ellipse_da_rms", "dA, rms");
   out << PrintRow("diff_ellipse_db_mean", "dB, mean");
   out << PrintRow("diff_ellipse_db_rms", "dB, rms");
   out << PrintRow("diff_ellipse_dx_mean", "dX, mean");
   out << PrintRow("diff_ellipse_dx_rms", "dX, rms");
   out << PrintRow("diff_ellipse_dy_mean", "dY, mean");
   out << PrintRow("diff_ellipse_dy_rms", "dY, rms");
   out << fR->TableEnd() << endl;

   out << fR->TableBegin("Point fit vs. hit fit. Circle.", list_of(string("")).range(fStudyNames));
   out << PrintRow("diff_circle_dr_mean", "dR, mean");
   out << PrintRow("diff_circle_dr_rms", "dR, rms");
   out << PrintRow("diff_circle_dx_mean", "dX, mean");
   out << PrintRow("diff_circle_dx_rms", "dX, rms");
   out << PrintRow("diff_circle_dy_mean", "dY, mean");
   out << PrintRow("diff_circle_dy_rms", "dY, rms");
   out << fR->TableEnd() << endl;

   out <<  fR->DocumentEnd();
}

string CbmRichGeoTestStudyReport::PrintRow(
      const string& property,
      const string& name)
{
   vector<string> n(fStudyNames.size(), "");
   for (int i = 0; i < fStudyNames.size(); i++) {
      n[i] = PrintValue(i, property);
   }
   return fR->TableRow(list_of(name).range(n));
}

string CbmRichGeoTestStudyReport::PrintValue(
      int studyId,
      const string& valueName)
{
   stringstream ss;
   ss.precision(3);
   ss << fQa[studyId].get(valueName, -1.);
   return ss.str();
}
