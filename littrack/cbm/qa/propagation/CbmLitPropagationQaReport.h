/**
 * \file CbmLitPropagationQaReport.h
 * \brief Create report for propagation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#ifndef CBMLITPROPAGATIONQAREPORT_H_
#define CBMLITPROPAGATIONQAREPORT_H_

#include "CbmSimulationReport.h"

/**
 * \class CbmLitPropagationQaReport
 * \brief Create report for propagation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
class CbmLitPropagationQaReport : public CbmSimulationReport
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitPropagationQaReport();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPropagationQaReport();

   /** Setters **/
   void SetNofPlanes(Int_t nofPlanes) { fNofPlanes = nofPlanes; }

protected:
   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Create();

   /**
    * \brief Inherited from CbmSimulationReport.
    */
   virtual void Draw();

   string PrintResAndPullRow(
           const string& rowName,
           Int_t algorithmIndex,
           Int_t planeIndex,
           const string& propertyName);

   string PrintTable(
         const string& title,
         Int_t algorithmIndex,
         const string& propertyName);

   Int_t fNofPlanes; // Number of planes
};

#endif /* CBMLITPROPAGATIONQAREPORT_H_ */
