
#include "CbmLitTrackSelectionEmpty.h"

CbmLitTrackSelectionEmpty::CbmLitTrackSelectionEmpty()
{

}

CbmLitTrackSelectionEmpty::~CbmLitTrackSelectionEmpty()
{
	
}

LitStatus CbmLitTrackSelectionEmpty::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionEmpty::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionEmpty::DoSelect(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionEmpty::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}
ClassImp(CbmLitTrackSelectionEmpty)
