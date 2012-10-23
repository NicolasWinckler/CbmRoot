/**
 * \file CbmLitReport.h
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMREPORT_H_
#define CBMREPORT_H_

#include "CbmDetectorList.h"
#include <string>
class CbmReportElement;
using std::string;
using std::vector;

/**
 * \enumeration ReportType
 * \brief Enumeration defines different report types.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
enum ReportType {kHtmlReport, kLatexReport, kTextReport};

/**
 * \class CbmReport
 * \brief Base class for reports.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmReport {
public:
   /**
    * \brief Constructor.
    */
   CbmReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmReport();

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
    * \brief Create concrete CbmReportElement instance based on report type.
    * \param[in] reportType Type of the report to be produced.
    */
   void CreateReportElement(
         ReportType reportType);

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

   /**
    * \brief Returns list of image names without extension and without path in specified directory.
    * \param[in] dir Directory name.
    * \param[in] pattern File name pattern.
    */
   vector<string> GetImages(
		   const string& dir,
		   const string& pattern) const;

   string fTitle; // Title of report
   string fAuthor; // Author of report
//   string fDate; // Date when report was generated

   // Background colors for error highlighting
   string fErrorColor; // error
   string fWarningColor; // warning
   string fNormalColor; // normal

   // If TRUE than results are highlighted depending on the results of checking procedure
   bool fIsUseChecking;

   CbmReportElement* fR; // Report element tool

private:

//   CbmReport(const CbmReport&);
//   CbmReport operator=(const CbmReport&);
};

#endif /* CBMREPORT_H_ */
