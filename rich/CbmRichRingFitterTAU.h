/******************************************************************************
*  $Id: CbmRichRingFitterTAU.h,v 1.3 2006/09/13 14:55:11 hoehne Exp $
*
*  Class  : CbmRichRingFitterTAU
*  Description: This is the header of a particular fitting class.
*               Here the ring is fitted with the TAU algorithm from A. Ayriyan/ G. Ososkov
*
*  Algorithm:                     Alexander Ayriyan 10.08.2005, Gennadi Ososkov
*  Adoption to new Fitter Class : Claudia Hoehne
*  E-mail : C.Hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitterTAU.h,v $
*  Revision 1.3  2006/09/13 14:55:11  hoehne
*  new optimal weight function added for improved fitting
*
*  Revision 1.2  2006/07/17 14:06:25  hoehne
*  ring radius correction added, see P. Stolpovsky, CBM simulation meeting 14.7.2006
*
*  Revision 1.1  2006/01/25 13:33:19  hoehne
*  initial version: ring fitting routines so far implemented in CbmRichLightSpot
*
*  
*
*******************************************************************************/
#ifndef CBM_RICH_RING_FITTER_TAU
#define CBM_RICH_RING_FITTER_TAU 1

#include "CbmRichRingFitter.h"

class CbmRichRingFitterTAU : public CbmRichRingFitter
{

   public:

   /** Default constructor **/
   CbmRichRingFitterTAU();

   /** Standard constructor **/
   CbmRichRingFitterTAU(Int_t verbose,Double_t correction,Int_t robust);

   /** Destructor **/
   virtual ~CbmRichRingFitterTAU();


   /** Ring Fitting algorithm **/
   void DoFit(CbmRichRing* ring);

   private:

   /** Verbosity level **/
   Int_t fRobust;
   Int_t fVerbose;
  Double_t fCorrection;
  ClassDef(CbmRichRingFitterTAU,1);
};

#endif
