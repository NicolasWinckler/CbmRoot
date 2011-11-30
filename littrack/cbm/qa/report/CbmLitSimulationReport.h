/**
 * \file CbmLitSimulationReport.h
 * \brief Base class for simulation reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITSIMULATIONREPORT_H_
#define CBMLITSIMULATIONREPORT_H_

#include "CbmLitReportElement.h"
#include "CbmDetectorList.h"
#include <boost/property_tree/ptree.hpp>
#include <string>

using namespace std;

/**
 * \class CbmLitSimulationReport
 * \brief Base class for simulation reports.
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitSimulationReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitSimulationReport();

   /**
    * \brief Set detector presence to TRUE if QA results for this detector has to be displayed in output.
    * \param[in] detId Id of the detector kMVD, kSTS...
    * \param[in] isDet True if detector is in the setup.
    */
   void SetDetectorPresence(
         DetectorId detId,
         bool isDet);

   /**
    * \brief Set explicitly electron setup of the detector.
    * \param[in] isElectronSetup true if electron setup.
    */
   void SetIsElectronSetup(
         bool isElectronSetup) {
      fIsElectronSetup = isElectronSetup;
   }

   /**
    * \brief Main function which creates report file.
    * \param[in] out Output stream for report file.
    * \param[in] qa Property tree with results.
    */
   void CreateReport(
         ostream& out,
         boost::property_tree::ptree* qa);


   void SetPtreeQa(boost::property_tree::ptree* qa){fQa = qa;}

   void SetPtreeIdeal(boost::property_tree::ptree* ideal){fIdeal = ideal;}

   void SetPtreeCheck(boost::property_tree::ptree* check){fCheck = check;}

   void SetOutputDir(const string& dir){fOutputDir = dir;}

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
   boost::property_tree::ptree* fQa;
   // Property with ideal values
   boost::property_tree::ptree* fIdeal;
   // Property tree with checked results for each study
   boost::property_tree::ptree* fCheck;
   // Name of directory with results
   string fOutputDir;

   // Background colors for error highlighting
   string fErrorColor; // error
   string fWarningColor; // warning
   string fNormalColor; // normal

   // If TRUE than results are highlighted depending on the results of checking procedure
   bool fIsUseChecking;
};

#endif /* CBMLITSIMULATIONREPORT_H_ */
