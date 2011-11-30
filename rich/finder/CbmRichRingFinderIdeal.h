/**
* $Id: CbmRichRingFinderIdeal.h,v 1.2 2006/01/26 09:49:40 hoehne Exp $
*
*  Class  : CbmRichRingFinderIdeal
*  Description: Ideal ring finder in the RICH for simulated event.
*
*  Author : Supriya Das
*  E-mail : S.Das@gsi.de
*/

#ifndef CBM_RICH_RING_FINDER_IDEAL
#define CBM_RICH_RING_FINDER_IDEAL 1

#include "CbmRichRingFinder.h"

class TClonesArray;

class CbmRichRingFinderIdeal : public CbmRichRingFinder
{
public:
   /** Default constructor **/
   CbmRichRingFinderIdeal();

   /** Standard constructor **/
   CbmRichRingFinderIdeal(
         Int_t verbose);

   /** Destructor **/
   virtual ~CbmRichRingFinderIdeal();

   /** Initialisation **/
   virtual void Init();

  /** Ring finding algorithm
   ** This just reads MC truth (MCTracks and MCPoints), creates
   ** one RichRing for each MCTrack and attaches the hits according
   ** to the MCTrack of the corresponding MCPoint
   *@param hitArray   Array of rich hits
   *@param projArray  Array of projected tracks (not needed here)
   *@param ringArray  Array of CbmRichRing
   **
   *@value Number of tracks created
   **/
   virtual Int_t DoFind(
       TClonesArray* hitArray,
       TClonesArray* projArray,
		 TClonesArray* ringArray);


private:
   TClonesArray* fMCRichPointArray; // CbmRichPoint array
   TClonesArray* fMCTrackArray; // CbmMCTrackArray

   Int_t fVerbose; // Verbosity level

   ClassDef(CbmRichRingFinderIdeal,1);
};

#endif
