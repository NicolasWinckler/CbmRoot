/**
 * \file CbmLitQaReconstructionReport.h
 *
 * \brief Creates HTML page for sudy of the reconstruction performance.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CbmLitQaReconstructionReport_H_
#define CbmLitQaReconstructionReport_H_

#include "CbmLitQaBaseReport.h"

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

using namespace std;
/**
 * \class CbmLitQaReconstructionReport
 *
 * \brief Creates HTML page for study of the reconstruction performance.
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
class CbmLitQaReconstructionReport : public CbmLitQaBaseReport
{
public:
   /**
    * \brief Constructor.
    * \param[in] type type of the report. "latex" or "html"
    */
   CbmLitQaReconstructionReport(
         const string& type);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaReconstructionReport();

protected:

   /**
    * \brief Creates HTML page and writes output to ostream.
    * \param[out] out ostream for output.
    */
   void Create(
      std::ostream& out);
};

#endif /* CbmLitQaReconstructionReport_H_ */
