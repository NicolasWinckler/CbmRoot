// -------------------------------------------------------------------------
// -----                        CbmTrack source file                   -----
// -----                  Created 29/11/07  by V. Friese               -----
// -----                  Modified 26/05/09  by A. Lebedev             -----
// -------------------------------------------------------------------------

#include "CbmTrack.h"

#include <iostream>
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
CbmTrack::CbmTrack() :
    fHitIndex(),
    fHitType(),
    fPidHypo(0),
    fParamFirst(),
    fParamLast(),
    fFlag(0),
    fChiSq(0.),
    fNDF(0),
    fPreviousTrackId(-1),
    fLinks(NULL)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmTrack::~CbmTrack()
{
}
// -------------------------------------------------------------------------

// -----   Public method AddHit   ------------------------------------------
void CbmTrack::AddHit(
		Int_t index,
		HitType type)
{
	fHitIndex.push_back(index);//std::make_pair(type, index));
	fHitType.push_back(type);
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void CbmTrack::Print() const
{
	cout << "CbmTrack: nof hits=" << fHitIndex.size() << ", chiSq=" << fChiSq
		<< ", NDF=" << fNDF << ", pidHypo=" << fPidHypo
		<< ", previousTrackId=" << fPreviousTrackId << ", flag=" << fFlag << endl;
//	fParamFirst.Print();
//	fParamLast.Print();
}
// -------------------------------------------------------------------------

ClassImp(CbmTrack);
