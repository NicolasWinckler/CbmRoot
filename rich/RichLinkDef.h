// $Id: RichLinkDef.h,v 1.16 2006/09/13 14:56:13 hoehne Exp $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class CbmRich+;
#pragma link C++ class CbmRichHitProducer+;
#pragma link C++ class CbmGeoRich+;
#pragma link C++ class CbmRichMatchRings+;
#pragma link C++ class CbmRichContFact;
#pragma link C++ class CbmGeoRichPar;
#pragma link C++ class CbmRichTrainAnnSelect;
#pragma link C++ class CbmRichTrainAnnElectrons;

//reconstruction
#pragma link C++ class CbmRichReconstruction+;

//qa
#pragma link C++ class CbmRichTestSim+;
#pragma link C++ class CbmRichTestHits+;
#pragma link C++ class CbmRichGeoTest+;
#pragma link C++ class CbmRichRingFitterQa+;
//#pragma link C++ class CbmRichParallelQa+;

//tracks
#pragma link C++ class CbmRichProjectionProducer+;
#pragma link C++ class CbmRichTrackExtrapolationIdeal+;
#pragma link C++ class CbmRichTrackExtrapolationMirrorIdeal+;
#pragma link C++ class CbmRichExtrapolateTracks+;
#pragma link C++ class CbmRichRingTrackAssignClosestD+;
#pragma link C++ class CbmRichRingTrackAssignIdeal+;
#pragma link C++ class CbmRichAssignTrack+;

//#pragma link C++ class LhcbRingFinder;

#pragma link C++ class CbmRichProt+;
#pragma link C++ class CbmRichProtHitProducer+;
#pragma link C++ class CbmRichProtPrepareExtrapolation+;
#pragma link C++ class CbmRichProtProjectionProducer+;
#pragma link C++ class CbmRichProtRingFinderHough+;
#pragma link C++ class CbmRichProtRingFinderHoughImpl+;

#endif

