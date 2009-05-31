/** CbmMuchTrackFinderIdeal
 * @author A.Lebedev <Andrey.Lebedev@gsi.de>
 * @since 2007
 **
 ** Ideal MUCH track finder. Finds tracks using MC information.
 ** Finds only signal muon tracks, i.e. PDG +/- 13 and MotherId = -1
 **/

#ifndef CBMMUCHTRACKFINDERIDEAL_H_
#define CBMMUCHTRACKFINDERIDEAL_H_ 1

#include "CbmMuchTrackFinder.h"

class TClonesArray;
class CbmMCTrack;
class FairMCPoint;
class CbmMuchTrack;

class CbmMuchTrackFinderIdeal : public CbmMuchTrackFinder
{
public:
	CbmMuchTrackFinderIdeal();
	virtual ~CbmMuchTrackFinderIdeal();

	void Init();

	Int_t DoFind(TClonesArray* hitArray,
			TClonesArray* trackArray);

private:
	void SetTrackParam(
			const CbmMCTrack* mcTrack,
			const FairMCPoint* mcPoint,
			CbmMuchTrack* track);

	TClonesArray* fMcTracks;
	TClonesArray* fMcPoints;
	TClonesArray* fMuchPixelHits;
	TClonesArray* fMuchTracks;
	TClonesArray* fMuchDigiMatches;
	TClonesArray* fClusters;

	Int_t fVerbose;
	Int_t fEvents;

	ClassDef(CbmMuchTrackFinderIdeal,1);
};

#endif
