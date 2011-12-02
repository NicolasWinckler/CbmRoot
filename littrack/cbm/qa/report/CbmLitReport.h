/**
 * \file CbmLitReport.h
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITREPORT_H_
#define CBMLITREPORT_H_

#include "CbmDetectorList.h"
#include <string>
class CbmLitReportElement;
using std::string;

/**
 * \enumeration LitReportType
 * \brief Enumeration defines different report types.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
enum LitReportType {kLitHtml, kLitLatex, kLitText};

/**
 * \class CbmLitReport
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitReport {
public:
   /**
    * \brief Constructor with report type.
    * \param[in] reportType Type of the report to be produced.
    */
   CbmLitReport(LitReportType reportType);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitReport();

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

   /* Setters */
   void SetTitle(const string& title) { fTitle = title; }
   void SetAuthor(const string& author) { fAuthor = author; }
   void SetErrorColor(const string& color) { fErrorColor = color; }
   void SetWarningColor(const string& color){ fWarningColor = color; }
   void SetNormalColor(const string& color){ fNormalColor = color; }
   void SetIsUseChecking(bool isCheck){ fIsUseChecking = isCheck; }

protected:
   string fTitle; // Title of report
   string fAuthor; // Author of report
//   string fDate; // Date when report was generated

   bool fIsElectronSetup; // If "electron" setup detected than true
   bool fIsMvd; // If MVD detected than true
   bool fIsSts; // If STS detected than true
   bool fIsRich; // If RICH detected than true
   bool fIsTrd; // If TRD detected than true
   bool fIsMuch; // If MUCH detected than true
   bool fIsTof; // If TOF detected than true

   // Background colors for error highlighting
   string fErrorColor; // error
   string fWarningColor; // warning
   string fNormalColor; // normal

   // If TRUE than results are highlighted depending on the results of checking procedure
   bool fIsUseChecking;

   CbmLitReportElement* fR; // Report element tool
};

#endif /* CBMLITREPORT_H_ */
