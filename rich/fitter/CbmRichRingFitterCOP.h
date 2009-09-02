/******************************************************************************
*  $Id: CbmRichRingFitterCOP.h,v 1.2 2006/07/17 14:06:25 hoehne Exp $
*
*  Class  : CbmRichRingFitterCOP
*  Description: This is the header of a particular fitting class.
*               Here the ring is fitted with theh COP algorithm from A. Ayriyan/ G. Ososkov
*
*  Algorithm:                     Alexander Ayriyan 10.08.2005, Gennadi Ososkov
*  Adoption to new Fitter Class : Claudia Hoehne
*  E-mail : C.Hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitterCOP.h,v $
*  Revision 1.2  2006/07/17 14:06:25  hoehne
*  ring radius correction added, see P. Stolpovsky, CBM simulation meeting 14.7.2006
*
*  Revision 1.1  2006/01/19 11:50:31  hoehne
*  initial version: implementation of COP fit to ring
*
*
*******************************************************************************/
#ifndef CBM_RICH_RING_FITTER_COP
#define CBM_RICH_RING_FITTER_COP 1

#include "CbmRichRingFitterImpl.h"

class CbmRichRingFitterCOP : public CbmRichRingFitterImpl
{

   public:

   /** Default constructor **/
   CbmRichRingFitterCOP();

   /** Standard constructor **/
   CbmRichRingFitterCOP(Int_t verbose, Double_t correction);

   /** Destructor **/
   virtual ~CbmRichRingFitterCOP();

   /** Ring Fitting algorithm **/
   void DoFit(CbmRichRing* ring);

   private:

   /** Verbosity level **/
   Int_t fVerbose;
  Double_t fCorrection;

  ClassDef(CbmRichRingFitterCOP,1);
};

#endif
