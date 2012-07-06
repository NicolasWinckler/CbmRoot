/**
 * \file CbmLitPropagationQaHistCreator.h
 * \brief Create histograms for propagation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#ifndef CBMLITPROPAGATIONQAHISTCREATOR_H_
#define CBMLITPROPAGATIONQAHISTCREATOR_H_

#include "TObject.h"
class CbmHistManager;
#include <string>
using std::string;

/**
 * \class CbmLitPropagationQaHistCreator
 * \brief Create histograms for propagation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
class CbmLitPropagationQaHistCreator
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitPropagationQaHistCreator();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPropagationQaHistCreator();

   /**
    * \brief Number of QA parameters [0-4]->residuals X, Y, Tx, Ty, Q/p; [5-9]->pulls X, Y, Tx, Ty, Q/p.
    */
   static const Int_t NofQaParameters = 10;

   /**
    * \brief Return histogram name.
    * \param[in] parameterIndex Index of QA parameter [0-4]->residuals X, Y, Tx, Ty, Q/p; [5-9]->pulls X, Y, Tx, Ty, Q/p.
    * \param[in] algorithmIndex Index of algorithm [0]->propagator; [1]->fitter; [2]->smoother.
    * \param[in] planeIndex Index of detector plane.
    * \return Histogram name.
    */
   static string HistName(
         Int_t parameterIndex,
         Int_t algorithmIndex,
         Int_t planeIndex);

   /**
    * \brief Create histograms for tracking QA.
    * \param[in] histManager CbmLitQaHistManager to be filled with created histograms.
    */
   void Create(
         CbmHistManager* histManager);

   /** Setters **/
   void SetFixedBounds(Bool_t fixedBounds) { fFixedBounds = fixedBounds; }
   void SetNofPlanes(Int_t nofPlanes) { fNofPlanes = nofPlanes; }

private:
   CbmHistManager* fHM; // Histogram manager for created histograms
   Bool_t fFixedBounds; // True if use fixed bounds for histograms
   Int_t fNofPlanes; // Number of planes
};

#endif /* CBMLITPROPAGATIONQAHISTCREATOR_H_ */
