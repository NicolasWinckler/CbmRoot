/**
 * \file CbmStsSimulationQaDraw.h
 * \brief Draw histograms for STS simulation QA.
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */

#ifndef CBMSTSSIMULATIONQADRAW_H_
#define CBMSTSSIMULATIONQADRAW_H_

#include <string>
using std::string;
class TH1;
class CbmHistManager;

/**
 * \class CbmStsSimulationQaDraw
 * \brief Draw histograms for STS simulation QA.
 * \author Volker Friese <v.friese@gsi.de>
 * \date 2012
 */
class CbmStsSimulationQaDraw
{
public:
   /**
    * \brief Constructor.
    */
   CbmStsSimulationQaDraw();

   /**
    * \brief Destructor.
    */
   virtual ~CbmStsSimulationQaDraw();

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

#endif /* CBMSTSSIMULATIONQADRAW_H_ */
