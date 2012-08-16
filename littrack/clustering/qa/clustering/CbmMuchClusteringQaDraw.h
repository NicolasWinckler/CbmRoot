/**
 * \file CbmMuchClusteringQaDraw.h
 * \brief Draw histograms for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */

#ifndef CBMMUCHCLUSTERINGQADRAW_H_
#define CBMMUCHCLUSTERINGQADRAW_H_

#include <string>
using std::string;
class TH1;
class CbmHistManager;

/**
 * \class CbmLitClusteringQaDraw.h
 * \brief Draw histograms for clustering QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
class CbmMuchClusteringQaDraw
{
public:
   /**
    * \brief Constructor.
    */
   CbmMuchClusteringQaDraw();

   /**
    * \brief Destructor.
    */
   virtual ~CbmMuchClusteringQaDraw();

   /**
    * \brief Draw histograms.
    * \param[in] histManager Pointer to histogram manager.
    * \param[in] outputDir Output directory for images. If not specified images are not saved.
    */
   void Draw(
         CbmHistManager* histManager,
         const string& outputDir = "");

private:

   void DrawHistogramsByPattern(
         const string& histNamePattern);

   CbmHistManager* fHM; // Pointer to histogram manager
   string fOutputDir; // Output directory for images
};

#endif /* CBMMUCHCLUSTERINGQADRAW_H_ */
