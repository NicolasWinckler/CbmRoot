#include "CbmLitTrackSelectionMuchRobust.h"

#include "CbmLitToolFactory.h"
#include "CbmLitTrackSelectionA.h"
#include "CbmLitTrackSelectionD.h"
#include "CbmLitTrackSelectionC.h"
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
	
//	fSelectionC->Properties().SetProperty("fNofSharedHits", 2);
//	fSelectionD->Properties().SetProperty("fMinNofHits", 1);
//	fSelectionD->Properties().SetProperty("fMinLastPlaneId", 9);	
	
	for (TrackIterator iTrack = itBegin; iTrack != itEnd; iTrack++) 
		(*iTrack)->SetFlag(0);

	//fSelectionA->DoSelect(itBegin, itEnd);
	fSelectionD->DoSelect(itBegin, itEnd);
	fSelectionRobust->DoSelect(itBegin, itEnd);
	//fSelectionC->DoSelect(itBegin, itEnd);
	
	return kLITSUCCESS;
}

LitStatus CbmLitTrackSelectionMuchRobust::DoSelect(
		TrackVector& tracks)
{
	return DoSelect(tracks.begin(), tracks.end());
}

ClassImp(CbmLitTrackSelectionMuchRobust)
