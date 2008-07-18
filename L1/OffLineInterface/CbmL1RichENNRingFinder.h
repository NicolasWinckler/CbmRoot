/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  Standalone RICH ring finder based on the Elastic Neural Net
 *
 *====================================================================
 */

#ifndef _CBM_L1_RICH_ENN_RING_FINDER_H_
#define _CBM_L1_RICH_ENN_RING_FINDER_H_

#include "CbmRichRingFinder.h"

#include <vector>

class ENNHit;
class ENNRing;


class CbmL1RichENNRingFinder :public CbmRichRingFinder
{

  struct ENNHit {
    int index;
    double x, y;               // coordinates
    double quality;               // quality of the best ring with this hit
    // variables for local search:
    double lx, ly, lr2;        // local coordinates
    double S0, S1, S2, S3, S4; // coefficients for calculation of E
    double C, Cx, Cy;          // coefficients for the parameter space
    bool on_ring;              // is the hit close to the current ring
  };
 
  struct ENNRing {
    bool on;                   // is the ring selected?
    double x, y, r;            // parameters
    double chi2;               // chi^2
    std::vector<ENNHit*> Hits;      // pointers to ring hits
    // variables for the selection procedure:
    int NHits;                 // number of ring hits
    int NOwn;                  // number of its own hits
    bool skip;                 // skip the ring during selection
  };
  
  static bool CompareENNHits( const ENNHit &h1, const ENNHit &h2 ){
    return ( h1.x < h2.x );
  }

  void ENNRingFinder( std::vector<ENNHit> &Hits, std::vector<ENNRing> &Rings,
		      double HitSize = 1., int MinRingHits = 5, 
		      double RMin = 2., double RMax = 6. );

 public:
  
  /** Standard constructor **/
  CbmL1RichENNRingFinder( Int_t verbose = 0 );

   /** Destructor **/
  ~CbmL1RichENNRingFinder();

  /** Initialisation **/
  void Init();

  /** Ring finding algorithm
   *
   *@param hitArray   Array of rich hits
   *@param projArray  Array of projected tracks (not needed here)
   *@param ringArray  Array of CbmRichRing
   **
   *@value Number of tracks created
   **/
  Int_t DoFind(TClonesArray* hitArray, TClonesArray* projArray,
	       TClonesArray* ringArray);

 private:

  Double_t fRecoTime;
  Int_t fNEvents;

 public:
  
  ClassDef(CbmL1RichENNRingFinder,1);
};

#endif
