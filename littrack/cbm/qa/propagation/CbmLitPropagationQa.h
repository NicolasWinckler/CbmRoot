/**
 * \file CbmLitPropagationQa.h
 * \brief Track propagation and track fit QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 **/

#ifndef CBMLITPROPAGATIONQA_H_
#define CBMLITPROPAGATIONQA_H_ 1

#include "base/CbmLitDetectorSetup.h"

#include "FairTask.h"

class CbmLitHistManager;
class CbmLitPropagationQaCalculator;

/**
 * \file CbmLitPropagationQa
 * \brief Track propagation and track fit QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2007
 *
 **/
class CbmLitPropagationQa : public FairTask
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitPropagationQa();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPropagationQa();

   /**
    * \brief Inherited from FairTask.
    */
   virtual InitStatus Init();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Exec(
      Option_t* opt);

   /**
    * \brief Inherited from FairTask.
    */
   virtual void Finish();

   /**
    * \brief Inherited from FairTask.
    */
   virtual void SetParContainers();

   /** Setters **/
   void SetFixedBounds(Bool_t fixedBounds) { fFixedBounds = fixedBounds; }
   void SetOutputDir(const string& outputDir) { fOutputDir = outputDir; }
   void SetNofPlanes(Int_t nofPlanes) { fNofPlanes = nofPlanes; }

private:

   /**
    * \brief Create final simulation report for this propagation QA run.
    * \param[in] title Report title.
    * \param[in] resultDirectory Directory with results.
    */
   void CreateSimulationReport(
         const string& title,
         const string& resultDirectory);

   CbmLitHistManager* fHM; // Histogram manager
   string fOutputDir; // Output directory for image files
   Bool_t fFixedBounds; // If true than fixed bounds are used for histograms
   Int_t fNofPlanes; // Number of planes in detector
   CbmLitPropagationQaCalculator* fPropagationQa; // Propagation QA tool

   ClassDef(CbmLitPropagationQa, 1);
};

#endif
