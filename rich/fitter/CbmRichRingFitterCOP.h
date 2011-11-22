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
class CbmRichRing;
using namespace std;

class CbmRichRingFitterCOP
{
public:

   CbmRichRingFitterCOP();
   ~CbmRichRingFitterCOP();
   void Init();

   void DoFit1(
         CbmRichRing *pRing,
         const vector<double>& hitX,
         const vector<double>& hitY);

   void DoFit(CbmRichRing *ring);

private:
   void FitRing(CbmRichRing* ring);
   void CalcChi2(CbmRichRing* ring);

	TClonesArray* fHitsArray;

	static const int MAX_NOF_HITS_IN_RING = 200;
	vector<double> fHitX;
	vector<double> fHitY;
	int fNofHits;
};

#endif
