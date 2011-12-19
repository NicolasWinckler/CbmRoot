/**
 * \file CbmLitClusteringQaDraw.h
 * \brief Draw histograms for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMLITCLUSTERINGQADRAW_H_
#define CBMLITCLUSTERINGQADRAW_H_

#include <string>
using std::string;
class TH1F;
class CbmLitHistManager;

/**
 * \class CbmLitClusteringQaDraw.h
 * \brief Draw histograms for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmLitClusteringQaDraw
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitClusteringQaDraw();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitClusteringQaDraw();

private:
   void DrawHitsStationHisto(
         const std::string& name,
         TH1F* hist);

   void DrawHitsStationHistos();

   CbmLitHistManager* fHM;
};

#endif /* CBMLITCLUSTERINGQADRAW_H_ */
