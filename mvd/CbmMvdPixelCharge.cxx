// -----------------------------------------------------------------------
// -----               CbmMvdDigi source file                        -----
// -----              Created 17/04/08  by C. Dritsa                 -----
// -----------------------------------------------------------------------

#include <iostream>
#include "CbmMvdPixelCharge.h"


using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
CbmMvdPixelCharge::CbmMvdPixelCharge()
  : TObject(),
    fCharge(-1.),
    fMaxChargeContribution(0.),
    fDominatingPointX(0.),
    fDominatingPointY(0.),
    fContributors(0),
    fChannelNrX(0),
    fChannelNrY(0),
    fTrackCharge(0.),
    fTrackId(-1),
    fPointId(-1)
{
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
CbmMvdPixelCharge::CbmMvdPixelCharge(Float_t charge, Int_t channelNrX, Int_t channelNrY, Int_t pointId, Int_t trackId)
  : TObject(),
    fCharge(charge),
    fMaxChargeContribution(0.),
    fDominatingPointX(0.),
    fDominatingPointY(0.),
    fContributors(0),
    fChannelNrX(channelNrX),
    fChannelNrY(channelNrY),
    fTrackCharge(0.),
    fTrackId(trackId),
    fPointId(pointId)
{
}

// ------- DigestCharge ----------------------------------------------------#

  // Sums up the charge stored in track charge (assuming this is the summed charge from 
  // all segments of a track). Checks if a new track contributed charge to the pixel
  // Checks if the new track is dominant
  
void CbmMvdPixelCharge::DigestCharge(Float_t pointX, Float_t pointY, Int_t pointId, Int_t trackId)
{

    if (fTrackCharge>0)
    {
	if (fTrackCharge>fMaxChargeContribution)
	{
	    fMaxChargeContribution = fTrackCharge;
	    fDominatingPointX      = pointX;
	    fDominatingPointY      = pointY;
	    fTrackId                 = trackId;
	    fPointId		   = pointId;
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
