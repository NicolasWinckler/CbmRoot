/**
 * \file CbmLitClusteringQaHistCreator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \brief Create histograms for clustering QA.
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQAHISTCREATOR_H_
#define CBMLITCLUSTERINGQAHISTCREATOR_H_

#include "TObject.h"
#include "cbm/base/CbmLitDetectorSetup.h"
#include <string>
class CbmHistManager;
using std::string;

/**
 * \class CbmLitClusteringQaHistCreator
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \brief Create histograms for clustering QA.
 * \date 2011
 */
class CbmLitClusteringQaHistCreator
{
public:
   /**
    * Constructor.
    */
   CbmLitClusteringQaHistCreator();

   /**
    * Destructor.
    */
   virtual ~CbmLitClusteringQaHistCreator();

   /**
    * \brief Create histograms for tracking QA.
    * \param[in] histManager CbmLitQaHistManager to be filled with created histograms.
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

   /**
    *
    */
   void CreateNofObjectsHistograms(
   		DetectorId detId,
   		const string& detName);

   /**
    *
    */
   void CreateNofObjectsHistograms(
   		DetectorId detId,
   		const string& detName,
   		const string& parameter,
   		const string& xTitle);

   CbmHistManager* fHM; // Histogram manager for created histograms
   CbmLitDetectorSetup fDet; // For detector setup determination
};

#endif /* CBMLITCLUSTERINGQAHISTCREATOR_H_ */
