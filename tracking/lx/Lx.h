#ifndef LX_INCLUDED
#define LX_INCLUDED

#include "FairTask.h"
#include "CbmMuchPixelHit.h"
#include "TClonesArray.h"
#include "LxMC.h"
#include "LxCA.h"
#include "LxEff.h"
#include "CbmTrackMatch.h"
#include "TH1.h"
#include "TProfile.h"
#include "CbmStsKFTrackFitter.h"
#include "CbmVertex.h"
#include <fstream>

#ifdef FAST_CODE
#define LX_DYNAMIC_CAST static_cast
#else// FAST_CODE
#define LX_DYNAMIC_CAST dynamic_cast
#endif// FAST_CODE

class LxFinder : public FairTask
{
  friend struct LxEff;
  friend class LxDraw;

public:
  static LxFinder* Instance();
  explicit LxFinder();
  ~LxFinder();
  InitStatus Init();// Inherited virtual.
  void Exec(Option_t* opt);// Inherited virtual.

  void SetGenInvMass(bool value)
  {
    generateInvMass = value;
  }

  void SetGenBackground(bool value)
  {
    generateBackground = value;
  }

  void SetGenChi2(bool value)
  {
    generateChi2 = value;
  }

private:
  void SaveRecoTracks();
  void CalcInvMass();
  void FinishTask();// Inherited virtual.
  void SaveEventTracks();
  void SaveInvMass();
  void SaveBackground();
  void SaveSignalChi2();
  void SaveBackgroundChi2();
  static LxFinder* fInstance;
  TClonesArray* muchPixelHits;
  TClonesArray* listMCTracks;
  TClonesArray* listMuchPts;
  TClonesArray* listMuchClusters;
  TClonesArray* listMuchPixelDigiMatches;
  TClonesArray* listStsTracks;
  TClonesArray* listStsMatches;
  TClonesArray* listStsPts;// STS MC-points array. Used for experiments with STS matching.
  TClonesArray* listRecoTracks;// It is an output array.
  LxEff effCounter;
  static TH1F* massHisto;
  static TH1F* backgroundMassHisto;
  static TH1F* signalChi2Histo;
  static TH1F* backgroundChi2Histo;
  static TProfile* effByMomentumProfile;
  CbmStsKFTrackFitter extFitter;
  CbmVertex* fPrimVtx;
  // These two containers below accumulates respectively all positive and all negative reconstructed tracks for all events.
  std::list<CbmStsTrack> positiveTracks;
  std::list<CbmStsTrack> negativeTracks;
  bool generateInvMass;
  bool generateBackground;
  bool generateChi2;
  std::vector<LxMCPoint> MCPoints;// Points should lay here in the same order as in listMuchPts.
  std::vector<LxMCTrack> MCTracks;// Tracks should lay here in the same order as in listMCTracks.
  std::list<LxStsMCPoint> MCStsPoints;
  std::list<LxStsMCPoint*> MCStsPointsByStations[8];
  LxSpace caSpace;
  std::map<Int_t, std::map<Int_t, int> > particleCounts;
#ifdef MAKE_EFF_CALC
  std::ofstream incomplete_events;// Events where not all tracks are reconstructed.
#endif//MAKE_EFF_CALC
  Int_t eventNumber;
#ifdef CALC_MUCH_DETECTORS_EFF
  Int_t mcPointsCount;
  Int_t mcPointsTriggered;
#endif//CALC_MUCH_DETECTORS_EFF

  ClassDef(LxFinder, 1);
};

#endif//LX_INCLUDED
