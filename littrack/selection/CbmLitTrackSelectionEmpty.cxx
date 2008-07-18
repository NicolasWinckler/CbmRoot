
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
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionEmpty::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}
ClassImp(CbmLitTrackSelectionEmpty)
