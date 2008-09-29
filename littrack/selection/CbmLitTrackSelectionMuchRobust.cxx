#include "CbmLitTrackSelectionMuchRobust.h"

#include "CbmLitToolFactory.h"
#include "CbmLitTrackSelectionA.h"
#include "CbmLitTrackSelectionD.h"
#include "CbmLitTrackSelectionC.h"
#include "CbmLitTrackSelectionChiSq.h"
#include "CbmLitTrack.h"

CbmLitTrackSelectionMuchRobust::CbmLitTrackSelectionMuchRobust()
{
	CbmLitToolFactory* factory = CbmLitToolFactory::Instance();
	fSelectionRobust = factory->CreateTrackSelection("MuchRobustSelection");
	fSelectionA = new CbmLitTrackSelectionA();
	fSelectionA->Initialize();
	fSelectionD = new CbmLitTrackSelectionD();
	fSelectionD->Initialize();
	fSelectionC = new CbmLitTrackSelectionC();
	fSelectionC->Initialize();
	fSelectionChiSq = new CbmLitTrackSelectionChiSq();
	fSelectionChiSq->Initialize();
}

CbmLitTrackSelectionMuchRobust::~CbmLitTrackSelectionMuchRobust()
{
	if (fSelectionRobust) delete fSelectionRobust;
	if (fSelectionA) delete fSelectionA;
	if (fSelectionD) delete fSelectionD;
	if (fSelectionC) delete fSelectionC;
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
		TrackIterator itBegin,
		TrackIterator itEnd)
{
	if (itBegin == itEnd) return kLITSUCCESS;
	
	((CbmLitTrackSelectionC*)fSelectionC)->SetNofSharedHits(2);
	((CbmLitTrackSelectionD*)fSelectionD)->SetMinNofHits(1);
	((CbmLitTrackSelectionD*)fSelectionD)->SetMinLastPlaneId(10);
	((CbmLitTrackSelectionChiSq*)fSelectionChiSq)->SetMaxChiSq(15.);
	
	for (TrackIterator iTrack = itBegin; iTrack != itEnd; iTrack++) 
		(*iTrack)->SetQuality(kLITGOOD);

	fSelectionD->DoSelect(itBegin, itEnd);
	fSelectionRobust->DoSelect(itBegin, itEnd);
//	fSelectionChiSq->DoSelect(itBegin, itEnd);
//	((CbmLitTrackSelectionD*)fSelectionD)->SetMinNofHits(9);
//	((CbmLitTrackSelectionD*)fSelectionD)->SetMinLastPlaneId(1);
//	fSelectionD->DoSelect(itBegin, itEnd);
	
//	fSelectionC->DoSelect(itBegin, itEnd);
	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuchRobust::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitTrackSelectionMuchRobust)
