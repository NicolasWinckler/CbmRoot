/******************************************************************************
*  $Id: CbmRichRingFitter.h,v 1.2 2006/09/13 14:57:07 hoehne Exp $
*
*  Class  : CbmRichRingFitter
*  Description: Abstract base class for concrete Rich Ring fitting algorithms.
*               Each derived class must implement the method DoFit and fill
*               the parameters fRadius, fCentreX, fCentreY.
*
*  Author : Supriya Das
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitter.h,v $
*  Revision 1.2  2006/09/13 14:57:07  hoehne
*  task for calculating Chi2 of ring fit added
*
*  Revision 1.1  2006/01/19 11:33:12  hoehne
*  initial version: base class for RingFitters
*
*
*******************************************************************************/
#ifndef CBM_RICH_RING_FITTER
#define CBM_RICH_RING_FITTER 1
#include "TClonesArray.h"
#include "TObject.h"
/*#include "CbmStsTrackParam.h"*/

class CbmRichRing;


class CbmRichRingFitter : public TObject
{

 public:

  /** Default constructor **/
  CbmRichRingFitter() { };


  /** Destructor **/
  virtual ~CbmRichRingFitter() { };


  /** Virtual method Init. If needed, to be implemented in the
   ** concrete class. Else no action.
   **/
  virtual void Init();


  /** Abstract method DoFit. To be implemented in the concrete class.
   ** Task: Make a fit to the hits attached to the track by the track
   ** finder. Fill the track parameter member variables.
   **
   *@param pTrack      Pointer to CbmStsTrack
   */
  virtual void DoFit(CbmRichRing* pRing) = 0;

  /** Set verbosity
   *@param verbose   Verbosity level
   **/
  void SetVerbose(Int_t verbose) { fVerbose = verbose; };

 protected:
 
 /** Method CalcChi2. Implemented in the base class for
       Rich Ring fitting algorithms.
       Task: Calculate chi-square for reconstracted ring.
    **/
   virtual void CalcChi2(CbmRichRing* pRing);

   Int_t   fNHits;
   Float_t fChi2;
   Float_t fRadius;
   Float_t fCentreX;
   Float_t fCentreY;

   Int_t fVerbose;      // Verbosity level
	
   TClonesArray* fHitsArray;
  ClassDef(CbmRichRingFitter,1);

};

#endif
