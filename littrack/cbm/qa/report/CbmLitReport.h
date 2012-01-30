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
    * \brief Constructor.
    */
   CbmLitReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitReport();

   /**
    * \brief Check if the property with name name exists in the QA property tree.
    * \param[in] name Name of property.
    * \return True if property exists, otherwise return false.
    */
   virtual bool PropertyExists(
         const std::string& name) const = 0;

   /* Setters */
   void SetTitle(const string& title) { fTitle = title; }
   void SetAuthor(const string& author) { fAuthor = author; }
   void SetErrorColor(const string& color) { fErrorColor = color; }
   void SetWarningColor(const string& color){ fWarningColor = color; }
   void SetNormalColor(const string& color){ fNormalColor = color; }
   void SetIsUseChecking(bool isCheck){ fIsUseChecking = isCheck; }

protected:
   /**
    * \brief Create concrete CbmLitReportElement instance based on report type.
    * \param[in] reportType Type of the report to be produced.
    */
   void CreateReportElement(
         LitReportType reportType);

   /**
    * \brief Delete report element. Normally should be called at the end of Create function.
    */
   void DeleteReportElement();

   /**
    * \brief Pure virtual function has to return JSON file name with QA results.
    * \return JSON file name.
    */
   virtual string GetQaFileName() const = 0;

   /**
    * \brief Pure virtual function has to return JSON file name with ideal results.
    * \return JSON file name.
    */
   virtual string GetIdealFileName() const = 0;

   /**
    * \brief Pure virtual function has to return JSON file name with checked results.
    * \return JSON file name.
    */
   virtual string GetCheckFileName() const = 0;

   string fTitle; // Title of report
   string fAuthor; // Author of report
//   string fDate; // Date when report was generated

   // Background colors for error highlighting
   string fErrorColor; // error
   string fWarningColor; // warning
   string fNormalColor; // normal

   // If TRUE than results are highlighted depending on the results of checking procedure
   bool fIsUseChecking;

   CbmLitReportElement* fR; // Report element tool
};

#endif /* CBMLITREPORT_H_ */
