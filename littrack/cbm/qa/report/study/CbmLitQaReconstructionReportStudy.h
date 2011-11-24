/**
 * \file CbmLitQaReconstructionReportStudy.h
 *
 * \brief Creates HTML page for sudy of the reconstruction performance.
 *
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CbmLitQaReconstructionReportStudy_H_
#define CbmLitQaReconstructionReportStudy_H_

#include "CbmLitQaBaseReportStudy.h"

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>

using namespace std;
/**
 * \class CbmLitQaReconstructionReportStudy
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
class CbmLitQaReconstructionReportStudy : public CbmLitQaBaseReportStudy
{
public:
   /**
    * \brief Constructor.
    * \param[in] type type of the report. "latex" or "html"
    */
   CbmLitQaReconstructionReportStudy(
         const string& type);

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitQaReconstructionReportStudy();

protected:

   /**
    * \brief Creates HTML page and writes output to ostream.
    * \param[out] out ostream for output.
    */
   void Create(
      ostream& out);
};

#endif /* CbmLitQaReconstructionReportStudy_H_ */
