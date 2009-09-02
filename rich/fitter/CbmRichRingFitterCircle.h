/******************************************************************************
*  $Id: CbmRichRingFitterCircle.h,v 1.2 2006/07/17 14:06:25 hoehne Exp $
*
*  Class  : CbmRichRingFitterCircle
*  Description: This is the header of a particular fittng class.
*               Here the ring is fitted with euqation of a cicle.
*
*  Author : Supriya Das  (Algorithm from F77 subroutine of S.Sadovsky)
*  E-mail : S.Das@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitterCircle.h,v $
*  Revision 1.2  2006/07/17 14:06:25  hoehne
*  ring radius correction added, see P. Stolpovsky, CBM simulation meeting 14.7.2006
*
*  Revision 1.1  2006/01/19 11:49:34  hoehne
*  initial version: implementation of circle fit to ring
*
*
*******************************************************************************/
#ifndef CBM_RICH_RING_FITTER_CIRCLE
#define CBM_RICH_RING_FITTER_CIRCLE 1

#include "CbmRichRingFitterImpl.h"

class CbmRichRingFitterCircle : public CbmRichRingFitterImpl
{

   public:

   /** Default constructor **/
   CbmRichRingFitterCircle();

   /** Standard constructor **/
   CbmRichRingFitterCircle(Int_t verbose, Double_t correction);

   /** Destructor **/
   virtual ~CbmRichRingFitterCircle();


   /** Ring Fitting algorithm **/
   void DoFit(CbmRichRing* ring);

   private:

   /** Verbosity level **/
   Int_t fVerbose;
  /** Correction: 1-yes; 0-no; **/
   Double_t fCorrection;

   ClassDef(CbmRichRingFitterCircle,1);
   
};

#endif
