/******************************************************************************
*  Class  : CbmRichRingFitterCOP
*  Description: This is the header of a particular fitting class.
*               Here the ring is fitted with theh COP algorithm from A. Ayriyan/ G. Ososkov
*
*  Algorithm:  Alexander Ayriyan, Gennadi Ososkov (10.08.2005)
*  E-mail : C.Hoehne@gsi.de
*******************************************************************************/
#ifndef CBM_RICH_RING_FITTER_COP
#define CBM_RICH_RING_FITTER_COP 1

#include <vector>

class TClonesArray;
class CbmRichRingLight;
class CbmRichRing;

class CbmRichRingFitterCOP
{
public:

   CbmRichRingFitterCOP();
   ~CbmRichRingFitterCOP();
   void Init();

   void DoFit(CbmRichRingLight *ring);
   void DoFit(CbmRichRing *ring);

private:
   void FitRing(CbmRichRingLight* ring);
   void CalcChi2(CbmRichRingLight* ring);

	TClonesArray* fHitsArray;

	static const int MAX_NOF_HITS_IN_RING = 200;
	std::vector<float> fHitX;
	std::vector<float> fHitY;
	int fNofHits;
};

#endif
