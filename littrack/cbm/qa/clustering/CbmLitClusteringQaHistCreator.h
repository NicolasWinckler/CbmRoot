/**
 * \file CbmLitClusteringQaHistCreator.h
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \brief Create histograms for clustering QA.
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQAHISTCREATOR_H_
#define CBMLITCLUSTERINGQAHISTCREATOR_H_

#include <string>
class CbmLitHistManager;
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
         CbmLitHistManager* histManager);

private:

   /**
    *
    */
   void Create1FHist(
         const string& name,
         const string& xTitle,
         const string& yTitle,
         int nofBins,
         float minBin,
         float maxBin);

   CbmLitHistManager* fHM; // Histogram manager for created histograms
};

#endif /* CBMLITCLUSTERINGQAHISTCREATOR_H_ */
