/******************************************************************************
*  $Id: CbmRichRingFitterRobustCOP.h,v 1.2 2006/07/17 14:06:25 hoehne Exp $
*
*  Class  : CbmRichRingFitterRobustCOP
*  Description: This is the header of a particular fitting class.
*               Here the ring is fitted with the RobustCOP algorithm from A. Ayriyan/ G. Ososkov
*
*  Algorithm:                     Alexander Ayriyan 10.08.2005, Gennadi Ososkov
*  Adoption to new Fitter Class : Claudia Hoehne
*  E-mail : C.Hoehne@gsi.de
*
*******************************************************************************
*  $Log: CbmRichRingFitterRobustCOP.h,v $
*  Revision 1.2  2006/07/17 14:06:25  hoehne
*  ring radius correction added, see P. Stolpovsky, CBM simulation meeting 14.7.2006
*
*  Revision 1.1  2006/01/25 13:32:33  hoehne
*  initial version: ring fitting routines, so far implemented in CbmRichLightSpot
*
*  
*
*******************************************************************************/
#ifndef CBM_RICH_RING_FITTER_RobustCOP
#define CBM_RICH_RING_FITTER_RobustCOP 1

#include "CbmRichRingFitterImpl.h"

class CbmRichRingFitterRobustCOP : public CbmRichRingFitterImpl
{

   public:

   /** Default constructor **/
   CbmRichRingFitterRobustCOP();

   /** Standard constructor **/
   CbmRichRingFitterRobustCOP(Int_t verbose,Double_t correction);

   /** Destructor **/
   virtual ~CbmRichRingFitterRobustCOP();

   /** Ring Fitting algorithm **/
   void DoFit(CbmRichRing* ring);

   private:

   /** Verbosity level **/
   Int_t fVerbose;
  Double_t fCorrection;
  ClassDef(CbmRichRingFitterRobustCOP,1);
};

#endif
