/**
 * \file CbmLitStudyReport.h
 * \brief Base class for study reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITSTUDYREPORT_H_
#define CBMLITSTUDYREPORT_H_

#include "CbmDetectorList.h"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include "CbmLitReportElement.h"

using namespace std;

/**
 * \class CbmLitStudyReport
 * \brief Base class for study reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitStudyReport();

   /**
    * \brief Set detector presence to TRUE if QA results for this detector has to be displayed in output.
    *
    * \param[in] detId Id of the detector kMVD, kSTS...
    * \param[in] isDet True if detector is in the setup.
    */
   void SetDetectorPresence(
         DetectorId detId,
         bool isDet);

   /**
    * \brief Set explicitly electron setup of the detector.
    *
    * \param[in] isElectronSetup true if electron setup.
    */
   void SetIsElectronSetup(
         bool isElectronSetup) {
      fIsElectronSetup = isElectronSetup;
   }

   /**
    * \brief Main function which creates report file.
    *
    * \param[in] out Output stream for report file.
    * \param[in] title Title of the page.
    * \param[in] results List of directories with results.
    * \param[in] names Name of the studies.
    */
   void CreateReport(
         ostream& out,
         const string& title,
         const vector<string>& results,
         const vector<string>& names);

   void SetPtreeQa(const vector<boost::property_tree::ptree>& qa){fQa = qa;}

   void SetPtreeIdeal(boost::property_tree::ptree* ideal){fIdeal = ideal;}

   void SetPtreeCheck(const vector<boost::property_tree::ptree>& check){fCheck = check;}

   void SetStudyNames(const vector<string>& names){fStudyNames = names;}

   void SetStudyResults(const vector<string>& results){fStudyResults = results;}

   void SetErrorColor(const string& color){fErrorColor = color;}

   void SetWarningColor(const string& color){fWarningColor = color;}

   void SetNormalColor(const string& color){fNormalColor = color;}

   void SetIsUseChecking(bool isCheck){fIsUseChecking = isCheck;}

protected:

   /**
    * \brief Creates report and writes output to ostream.
    * \param[out] out ostream for output.
    */
   virtual void Create(
      ostream& out) = 0;

   CbmLitReportElement* fR;

   bool fIsElectronSetup; // If "electron" setup detected than true
   bool fIsMvd; // If MVD detected than true
   bool fIsSts; // If STS detected than true
   bool fIsRich; // If RICH detected than true
   bool fIsTrd; // If TRD detected than true
   bool fIsMuch; // If MUCH detected than true
   bool fIsTof; // If TOF detected than true

   string fTitle; // report title

   // Property tree of Qa results for each study
   vector<boost::property_tree::ptree> fQa;
   // Property with ideal values
   boost::property_tree::ptree* fIdeal;
   // Property tree with checked results for each study
   vector<boost::property_tree::ptree> fCheck;
   // Names of studies
   vector<string> fStudyNames;
   // Names of directories with study results
   vector<string> fStudyResults;

   // Background colors for error highlighting
   string fErrorColor; // error
   string fWarningColor; // warning
   string fNormalColor; // normal

   // If TRUE than results are highlighted depending on the results of checking procedure
   bool fIsUseChecking;
};

#endif /* CBMLITSTUDYREPORT_H_ */
