#include "CbmLitTrackSelectionMuchRobust.h"

#include "CbmLitToolFactory.h"
#include "CbmLitTrackSelectionShortTracks.h"
#include "CbmLitTrackSelectionD.h"
#include "CbmLitTrackSelectionSharedHits.h"
#include "CbmLitTrackSelectionChiSq.h"
#include "CbmLitTrack.h"

CbmLitTrackSelectionMuchRobust::CbmLitTrackSelectionMuchRobust()
{
//	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
//	fSelectionRobust = factory->CreateTrackSelection("MuchRobustSelection");
//	fSelectionA = new CbmLitTrackSelectionA();
//	fSelectionA->Initialize();
//	fSelectionD = new CbmLitTrackSelectionD();
//	fSelectionD->Initialize();
//	fSelectionC = new CbmLitTrackSelectionC();
//	fSelectionC->Initialize();
//	fSelectionChiSq = new CbmLitTrackSelectionChiSq();
//	fSelectionChiSq->Initialize();
}

CbmLitTrackSelectionMuchRobust::~CbmLitTrackSelectionMuchRobust()
{
//	if (fSelectionRobust) delete fSelectionRobust;
//	if (fSelectionA) delete fSelectionA;
//	if (fSelectionD) delete fSelectionD;
//	if (fSelectionC) delete fSelectionC;
}

LitStatus CbmLitTrackSelectionMuchRobust::Initialize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuchRobust::Finalize()
{
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuchRobust::DoSelect(
		TrackPtrIterator itBegin,
		TrackPtrIterator itEnd)
{
//	if (itBegin == itEnd) return kLITSUCCESS;
//
//	((CbmLitTrackSelectionC*)fSelectionC)->SetNofSharedHits(2);
//	((CbmLitTrackSelectionD*)fSelectionD)->SetMinNofHits(1);
//	((CbmLitTrackSelectionD*)fSelectionD)->SetMinLastPlaneId(10);
//	((CbmLitTrackSelectionChiSq*)fSelectionChiSq)->SetMaxChiSq(15.);
//
//	for (TrackPtrIterator iTrack = itBegin; iTrack != itEnd; iTrack++)
//		(*iTrack)->SetQuality(kLITGOOD);
//
//	fSelectionD->DoSelect(itBegin, itEnd);
//	fSelectionRobust->DoSelect(itBegin, itEnd);
//	fSelectionChiSq->DoSelect(itBegin, itEnd);
//	((CbmLitTrackSelectionD*)fSelectionD)->SetMinNofHits(9);
//	((CbmLitTrackSelectionD*)fSelectionD)->SetMinLastPlaneId(1);
//	fSelectionD->DoSelect(itBegin, itEnd);

//	fSelectionC->DoSelect(itBegin, itEnd);

	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuchRobust::DoSelect(
		TrackPtrVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}
