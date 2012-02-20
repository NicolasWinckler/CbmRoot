/**
 * \file CbmLitPropagationQaDraw.h
 * \brief Draw histograms for propagation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
#ifndef CBMLITPROPAGATIONQADRAW_H_
#define CBMLITPROPAGATIONQADRAW_H_

#include "TObject.h"
#include <string>
class CbmLitHistManager;
using std::string;

/**
 * \class CbmLitPropagationQaDraw
 * \brief Draw histograms for propagation QA.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2012
 */
class CbmLitPropagationQaDraw
{
public:
   /**
    * \brief Constructor.
    */
   CbmLitPropagationQaDraw();

   /**
    * \brief Destructor.
    */
   virtual ~CbmLitPropagationQaDraw();

   /**
    * \brief Draw histograms.
    * \param[in] histManager Pointer to histogram manager.
    * \param[in] outputDir Output directory for images. If not specified images are not saved.
    */
   void Draw(
         CbmLitHistManager* histManager,
         const string& outputDir = "");

   /** Setters **/
   void SetNofPlanes(Int_t nofPlanes) { fNofPlanes = nofPlanes; }

private:
   /**
    * \brief Draw histograms for specified algorithm and detector plane.
    * \param[in] algorithmIndex Index of algorithm.
    * \param[in] planeIndex Index of plane.
    */
   void DrawHistos(
      Int_t algorithmIndex,
      Int_t planeIndex);

   CbmLitHistManager* fHM; // Histogram manager
   string fOutputDir; // Output directory for images
   Int_t fNofPlanes; // Number of planes
};

#endif /* CBMLITPROPAGATIONQADRAW_H_ */
