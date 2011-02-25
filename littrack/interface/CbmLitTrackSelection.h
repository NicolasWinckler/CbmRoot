#ifndef CBMLITTRACKSELECTION_H_
#define CBMLITTRACKSELECTION_H_

#include "base/CbmLitTool.h"
#include "base/CbmLitTypes.h"

class CbmLitTrackSelection : public CbmLitTool
{
public:
	CbmLitTrackSelection(){};
	virtual ~CbmLitTrackSelection(){};

	virtual LitStatus DoSelect(
			TrackPtrIterator itBegin,
			TrackPtrIterator itEnd) = 0;

	virtual LitStatus DoSelect(
			TrackPtrVector& tracks) = 0;

//	virtual LitStatus DoSelect(
//			TrackBranchPtrIterator itBegin,
//			TrackBranchPtrIterator itEnd) = 0;
//
//	virtual LitStatus DoSelect(
//			TrackBranchPtrVector& tracks) = 0;
};

#endif /*CBMLITTRACKSELECTION_H_*/
