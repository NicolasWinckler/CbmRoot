/**
 * \file CbmLitTrackingQaStudyReport.h
 * \brief Creates study report for reconstruction.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITTRACKINGQASTUDYREPORT_H_
#define CBMLITTRACKINGQASTUDYREPORT_H_

#include "../report/CbmLitStudyReport.h"
#include <string>
#include "TSystem.h"
using std::string;

/**
 * \class CbmLitTrackingQaStudyReport
 * \brief Creates study report for reconstruction.
 *
 * Generates report from a number of results of different
 * simulations and reconstructions. Creates comparison tables.
 * Very useful for studies since all numbers are automatically
 * put in the comparison tables.
 *
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 *
 */
class CbmLitTrackingQaStudyReport : public CbmLitStudyReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitTrackingQaStudyReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitTrackingQaStudyReport();

protected:
    /**
    * \brief Inherited from CbmLitStudyReport.
    */
   void Create(
         ostream& out);

   /**
     * \brief Inherited from CbmLitSimulationReport.
     */
    virtual string GetQaFileName() const {
       return "tracking_qa.json";
    }

    /**
     * \brief Inherited from CbmLitSimulationReport.
     */
    virtual string GetIdealFileName() const {
       return string(gSystem->Getenv("VMCWORKDIR")) + ("/littrack/cbm/qa/tracking/tracking_qa_ideal.json");
    }

    /**
     * \brief Inherited from CbmLitSimulationReport.
     */
    virtual string GetCheckFileName() const {
       return "tracking_qa_check.json";
    }

   /**
    * \brief Print one row in a table.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return String with table row.
    */
   virtual string PrintRow(
      const string& property,
      const string& name);

   /**
    * \brief Print one row with efficiencies in a table.
    * \param[in] property Name of the property in property tree.
    * \param[in] name Name of the row.
    * \return String with table row.
    */
   virtual string PrintRowEff(
         const string& property,
         const string& name);

   /**
    * \brief Print table of images with reconstruction performance.
    * \param[in] title Title of the image.
    * \param[in] file Name of the image file.
    * \return String with table which contains images.
    */
   virtual string PrintImageTable(
         const string& title,
         const string& file);

   /**
    * \brief
    */
   virtual string PrintValue(
         int studyId,
         const string& valueTitle);
};

#endif /* CBMLITTRACKINGQASTUDYREPORT_H_ */
