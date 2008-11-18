// -----------------------------------------------------------------------
// -----               CbmMvdDigi source file                        -----
// -----              Created 17/04/08  by C. Dritsa                 -----
// -----------------------------------------------------------------------

#include <iostream>
#include "CbmMvdPixelCharge.h"


using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdPixelCharge::CbmMvdPixelCharge(){
    fCharge = -1;
    fMaxChargeContribution = 0;
    fDominatingPointX      = 0;
    fDominatingPointY      = 0;
    fChannelNrX   = 0;
    fChannelNrY   = 0;
    fContributors = 0;
    fTrackCharge  = 0;
}
// -------------------------------------------------------------------------
Bool_t CbmMvdPixelCharge::TestXY(Int_t channelNrX,Int_t channelNrY)
{

    if ( (channelNrX==fChannelNrX) && (channelNrY==fChannelNrY))
    {
	return 1;
    }
    else {
	return 0;
    };
}

// -----   Constructor with parameters   -----------------------------------
CbmMvdPixelCharge::CbmMvdPixelCharge(Int_t charge, Int_t channelNrX, Int_t channelNrY):TObject()
{
    fTrackCharge = charge;

    fMaxChargeContribution = 0;
    fDominatingPointX      = 0;
    fDominatingPointY      = 0;

    fChannelNrX = channelNrX;
    fChannelNrY = channelNrY;

    fContributors = 0;
    fCharge       = 0;

}

// ------- DigestCharge ----------------------------------------------------#

  // Sums up the charge stored in track charge (assuming this is the summed charge from 
  // all segments of a track). Checks if a new track contributed charge to the pixel
  // Checks if the new track is dominant
  
void CbmMvdPixelCharge::DigestCharge(Float_t pointX, Float_t pointY)
{

    if (fTrackCharge>0)
    {
	if (fTrackCharge>fMaxChargeContribution)
	{
	    fMaxChargeContribution = fTrackCharge;
	    fDominatingPointX      = pointX;
	    fDominatingPointY      = pointY;
	}

	fCharge = fCharge+fTrackCharge; // Add charge of the track
	fTrackCharge = 0;
	fContributors = fContributors+1;
    }
}



// -----   Destructor   ----------------------------------------------------
CbmMvdPixelCharge::~CbmMvdPixelCharge(){}
// -------------------------------------------------------------------------

ClassImp(CbmMvdPixelCharge)
