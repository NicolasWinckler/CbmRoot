/**
 * \file CbmLitQaBaseReport.h
 *
 * \brief Base class for report generators.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITQABASEREPORT_H_
#define CBMLITQABASEREPORT_H_

#include "CbmDetectorList.h"
#include <boost/property_tree/ptree.hpp>
#include <string>
#include "CbmLitQaBaseReportElement.h"

using namespace std;

/**
 * \class CbmLitQaBaseReport
 *
 * \brief Base class for report generators.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitQaBaseReport {
public:
   /**
    * \brief Constructor.
    */
   CbmLitQaBaseReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaBaseReport();

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
         boost::property_tree::ptree* qa);

protected:

   /**
    * \brief Creates report and writes output to ostream.
    * \param[out] out ostream for output.
    */
   virtual void Create(
      ostream& out) = 0;


   CbmLitQaBaseReportElement* fR;

   bool fIsElectronSetup; // If "electron" setup detected than true
   bool fIsMvd; // If MVD detected than true
   bool fIsSts; // If STS detected than true
   bool fIsRich; // If RICH detected than true
   bool fIsTrd; // If TRD detected than true
   bool fIsMuch; // If MUCH detected than true
   bool fIsTof; // If TOF detected than true

   string fTitle; // report title

};

#endif /* CBMLITQABASEREPORT_H_ */
