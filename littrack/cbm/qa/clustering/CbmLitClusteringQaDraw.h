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
class TH1;
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

   /**
    * \brief Draw histograms.
    * \param[in] histManager Pointer to histogram manager.
    * \param[in] outputDir Output directory for images. If not specified images are not saved.
    */
   void Draw(
         CbmLitHistManager* histManager,
         const string& outputDir = "");

private:

   void DrawHistogramsByPattern(
         const string& histNamePattern);

   CbmLitHistManager* fHM; // Pointer to histogram manager
   string fOutputDir; // Output directory for images
};

#endif /* CBMLITCLUSTERINGQADRAW_H_ */
