/**
* \file CbmRichConverter.h
*
* \brief Convert internal data classes to cbmroot common data classes.
*
* \author Semen Lebedev
* \date 2012
**/
#ifndef CBM_RICH_CONVERTER
#define CBM_RICH_CONVERTER

#include "CbmRichRingLight.h"
#include "CbmRichRing.h"
#include "CbmRichHit.h"
#include "FairRootManager.h"
#include "TClonesArray.h"

#include <iostream>
#include <vector>

using std::vector;
using std::cout;
using std::endl;

/**
* \class CbmRichConverter
*
* \brief Convert internal data classes to cbmroot common data classes.
*
* \author Semen Lebedev
* \date 2012
**/

class CbmRichConverter
{
public:
   /**
    * \brief Copy hits from CbmRichRing to CbmRichRingLight.
    * \param[in] ring1 CbmRichRing from which hits are copied
    * \param[out] ring2 RICH ring to which hits are added.
    */
   static void CopyHitsToRingLight(
         const CbmRichRing* ring1,
         CbmRichRingLight* ring2)
   {
      if (NULL == fRichHits) {
         Init();
      }
      if (NULL == fRichHits) return;
      int nofHits = ring1->GetNofHits();
      for (int i = 0; i < nofHits; i++){
         CbmRichHit* hit = (CbmRichHit*) fRichHits->At(i);
         if (NULL == hit) continue;
         CbmRichHitLight hl(hit->GetX(), hit->GetY());
         ring2->AddHit(hl);
      }
   }

   /**
    * \brief Copy hits coordinates from vectors to CbmRichRingLight.
    * \param[in] hitX Vector of X coordinates.
    * \param[in] hitY Vector of Y coordinates.
    * \param[out] ring RICH ring to which hits are added.
    */
   static void CopyHitsToRingLight(
         const vector<double>& hitX,
         const vector<double>& hitY,
         CbmRichRingLight* ring)
   {
      int nofHits = hitX.size();
      for (int i = 0; i < nofHits; i++){
         CbmRichHitLight hl(hitX[i], hitY[i]);
         ring->AddHit(hl);
      }
   }

   /**
    * \brief Copy parameters from CbmRichRingLight to CbmRichRing.
    * \param[in] ring1 CbmRichRingLight from which parameters are copied.
    * \param[out] ring2 CbmRichRing to which parameters are copied.
    */
   static void CopyParamsToRing(
         const CbmRichRingLight* ring1,
         CbmRichRing* ring2)
   {
      int i = 99;
      int r = 8;
      i = r+i;
   }

   static TClonesArray* fRichHits;

private:
   /**
    * \brief Initialize array of RICH hits.
    */
   static void Init()
   {
      FairRootManager* ioman = FairRootManager::Instance();
      if ( NULL == ioman) {
         cout << "-E- CbmRichConverter::Init, RootManager not instantised!" << endl;
         return;
      }
      fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
      if ( NULL == fRichHits) {
         cout << "-W- CbmRichConverter::Init, No RichHit array" << endl;
      }
   }
};



#endif
