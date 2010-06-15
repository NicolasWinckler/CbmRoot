/*
*  Class  : CbmRichRingSelectImpl
*  Description : Abstract base class for concrete RICH ring selection algorithm:
*                to be run after ring-track assign for fake-ring rejection
*
*  Author : Semen Lebedev
*  E-mail : S.Lebedev@gsi.de
*/


#ifndef CBM_RICH_RING_SELECT_IMPL
#define CBM_RICH_RING_SELECT_IMPL 1

#include <vector>

class TClonesArray;
class CbmRichRingLight;
class CbmRichRing;

class CbmRichRingSelectImpl
{
public:
	int kMAX_NOF_HITS;

	 CbmRichRingSelectImpl() { };
	 ~CbmRichRingSelectImpl() { };

	 void Init();

	 int GetNofHitsOnRing(CbmRichRing* ring);
	 int GetNofHitsOnRingCircle(CbmRichRing* ring);
	 float GetAngle(CbmRichRing* ring);
protected:
    std::vector<float> fAlpha;
    std::vector<float> fPhi;
	TClonesArray* fHitsArray;
};

#endif
