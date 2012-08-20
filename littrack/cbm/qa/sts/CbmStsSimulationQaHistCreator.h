/**
 * \file CbmStsSimulationQaHistCreator.h
 * \author Volker Friese <v.friese@gsi.de>
 * \brief Create histograms for STS simulation QA.
 * \date 2012
 */

#ifndef CBMSTSSIMULATIONQAHISTCREATOR_H_
#define CBMSTSSIMULATIONQAHISTCREATOR_H_

#include "TObject.h"
#include <string>
class CbmHistManager;
using std::string;

/**
 * \class CbmStsSimulationQaHistCreator
 * \author Volker Friese <v.friese@gsi.de>
 * \brief Create histograms for STS simulation QA.
 * \date 2012
 */
class CbmStsSimulationQaHistCreator
{
public:
   /**
    * Constructor.
    */
   CbmStsSimulationQaHistCreator();

   /**
    * Destructor.
    */
   virtual ~CbmStsSimulationQaHistCreator();

   /**
    * \brief Create histograms for STS simulation QA.
    * \param[in] histManager CbmHistManager to be filled with created histograms.
    */
   void Create(
         CbmHistManager* histManager);

private:

   /**
    *
    */
   void CreateH1F(
         const string& name,
         const string& xTitle,
         const string& yTitle,
         Int_t nofBins,
         Double_t minBin,
         Double_t maxBin);

   CbmHistManager* fHM; ///< Histogram manager for created histograms
};

#endif /* CBMSTSSIMULATIONQAHISTCREATOR_H_ */
