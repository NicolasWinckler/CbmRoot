/**
 * \file CbmMuchClusteringQaHistCreator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \brief Create histograms for clustering QA.
 * \date 2011
 */

#ifndef CBMMUCHCLUSTERINGQAHISTCREATOR_H_
#define CBMMUCHCLUSTERINGQAHISTCREATOR_H_

#include "TObject.h"
#include "../../../cbm/base/CbmLitDetectorSetup.h"
#include "CbmMuchGeoScheme.h"
#include <string>
class CbmHistManager;
using std::string;

/**
 * \class CbmLitClusteringQaHistCreator
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \brief Create histograms for clustering QA.
 * \date 2011
 */
class CbmMuchClusteringQaHistCreator
{
public:
   /**
    * Constructor.
    */
   CbmMuchClusteringQaHistCreator();

   /**
    * Destructor.
    */
   virtual ~CbmMuchClusteringQaHistCreator();

   /**
    * \brief Create histograms for tracking QA.
    * \param[in] histManager CbmLitQaHistManager to be filled with created histograms.
    */
   void Create(
         CbmHistManager* histManager,
         CbmMuchGeoScheme* muchGeoScheme);

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

   void CreateErrorsByRadiusHistograms(
		DetectorId detId,
	   	const string& detName,
	   	const string& nStation,
	   	const string& nLayer,
	   	const string& xTitle,
	   	Int_t nBins,
	   	Double_t rMax);

   void CreateQualClToPointByRadiusHistograms(
   		DetectorId detId,
   	   	const string& detName,
   	   	const string& nStation,
   	   	const string& nLayer,
   	   	const string& xTitle,
   	   	Int_t nBins,
   	   	Double_t rMax);

   void CreateQualDigisToClByRadiusHistograms(
   		DetectorId detId,
   	   	const string& detName,
   	   	const string& nStation,
   	   	const string& nLayer,
   	   	const string& xTitle,
   	   	Int_t nBins,
   	   	Double_t rMax);

   void CreateErrorsByLayersHistograms(
   		DetectorId detId,
   	   	const string& detName,
   	   	const string& xTitle,
   	   	Int_t nLayers);

   void CreateEfficiencyByLayersHistograms(
      	DetectorId detId,
      	const string& detName,
      	const string& xTitle,
      	Int_t nLayers);

   void CreateQualClToPointByLayersHistograms(
        DetectorId detId,
        const string& detName,
        const string& xTitle,
        Int_t nLayers);

   void CreateQualDigisToClByLayersHistograms(
        DetectorId detId,
        const string& detName,
        const string& xTitle,
        Int_t nLayers);

   CbmMuchGeoScheme* fMuchGeoScheme;

   CbmHistManager* fHM; // Histogram manager for created histograms
   CbmLitDetectorSetup fDet; // For detector setup determination
};

#endif /* CBMMUCHCLUSTERINGQAHISTCREATOR_H_ */
