/**
* \file CbmRichRingFitterCOP.h
*
* \brief Here the ring is fitted with theh COP algorithm from A. Ayriyan/G. Ososkov.
*
* \author Alexander Ayriyan, Gennadi Ososkov, Semen Lebedev <s.lebedev@gsi.de>
* \date 2005
**/
#ifndef CBMRICHRINGFITTERCOP
#define CBMRICHRINGFITTERCOP

#include <vector>

class TClonesArray;
class CbmRichRing;
using namespace std;

/**
* \class CbmRichRingFitterCOP
*
* \brief Here the ring is fitted with theh COP algorithm from A. Ayriyan/G. Ososkov.
*
* \author Alexander Ayriyan, Gennadi Ososkov, Semen Lebedev <s.lebedev@gsi.de>
* \date 2005
**/
class CbmRichRingFitterCOP
{
public:
   /**
    * \brief Standard constructor.
    */
   CbmRichRingFitterCOP();

   /**
    * \brief Destructor.
    */
   ~CbmRichRingFitterCOP();

   /**
    * \brief Initialize TClonesArrays for the Rich hits.
    */
   void Init();

   /**
    * \brief Fit ring using hit coordinates from vectors.
    * \param[in,out] ring RICH ring to be fitted.
    * \param[in] hitX Vector of x coordinates of hits.
    * \param[in] hitY Vector of y coordinates of hits.
    */
   void DoFit(
         CbmRichRing *ring,
         const vector<double>& hitX,
         const vector<double>& hitY);

   /**
    * \brief Fit ring.
    * \param[in,out] ring RICH ring to be fitted.
    */
   void DoFit(
         CbmRichRing *ring);

private:
   /**
    * \brief Execute ring fitting algorithm.
    * \param[in,out] ring RICH ring to be fitted.
    */
   void FitRing(
         CbmRichRing* ring);

   /**
    * \brief Calculate chi2 value of fitted ring.
    * \param[in,out] ring Fitted ring.
    */
   void CalcChi2(
         CbmRichRing* ring);

	TClonesArray* fRichHits; // Array of RICH hits

	static const int MAX_NOF_HITS_IN_RING = 400; // maximum possible number of hits in ring
	vector<double> fHitX; // vector of X coordinate
	vector<double> fHitY; // vector of Y coordinate
	int fNofHits;
};

#endif
