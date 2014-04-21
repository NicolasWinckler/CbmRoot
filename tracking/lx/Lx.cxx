#include <sys/time.h>
#include <iostream>
#include "Lx.h"
#include "CbmMuchGeoScheme.h"
#include "LxDraw.h"
#include "CbmMCTrack.h"
#include "TDatabasePDG.h"
#include "CbmMuchPoint.h"
#include "CbmStsPoint.h"
#include "TGeoManager.h"
#include "CbmMuchCluster.h"
#include "CbmMuchDigiMatch.h"
#include "CbmMuchTrack.h"
#include "CbmKFTrack.h"
#include "CbmKFParticle.h"

ClassImp(LxFinder)

using namespace std;

LxFinder* LxFinder::fInstance = 0;
TH1F* LxFinder::massHisto = 0;
TH1F* LxFinder::backgroundMassHisto = 0;
TH1F* LxFinder::signalChi2Histo = 0;
TH1F* LxFinder::backgroundChi2Histo = 0;
TProfile* LxFinder::effByMomentumProfile = 0;

#ifdef MAKE_ANGLE_HISTOS
TH1F* angleXHistos[6];
TH1F* angleYHistos[6];
TH1F* tanXHistos[6];
TH1F* tanYHistos[6];
#endif//MAKE_ANGLE_HISTOS

#ifdef MAKE_BRAKE_ANGLE_HISTOS
TH1F* breakTanXHistos[6];
TH1F* breakAngleXHistos[6];
TH1F* breakTanYHistos[6];
TH1F* breakAngleYHistos[6];
#endif//MAKE_BRAKE_ANGLE_HISTOS

#ifdef MAKE_DISPERSE_HISTOS
TH1F* disperse01XHistos[6];
TH1F* disperse01YHistos[6];
TH1F* disperse02XHistos[6];
TH1F* disperse02YHistos[6];
#endif//MAKE_DISPERSE_HISTOS

#ifdef MAKE_MU_PLUS_MU_MINUS_HISTOS
TH1F* muPlusAngleXHisto;
TH1F* muPlusAngleYHisto;
TH1F* muMinusAngleXHisto;
TH1F* muMinusAngleYHisto;
#endif//MAKE_MU_PLUS_MU_MINUS_HISTOS

#ifdef MAKE_DIFF_FIT_HISTOS
TH1F* stsXDiffFitHisto;
TH1F* stsYDiffFitHisto;
TH1F* stsTXDiffFitHisto;
TH1F* stsTYDiffFitHisto;
#endif//MAKE_DIFF_FIT_HISTOS

#ifdef MAKE_DIFF_FIRST_HISTOS
TH1F* stsXDiffFirstHisto[8];
TH1F* stsYDiffFirstHisto[8];
TH1F* stsTXDiffFirstHisto[8];
TH1F* stsTYDiffFirstHisto[8];
#endif//MAKE_DIFF_FIRST_HISTOS

#ifdef MAKE_FIT_COEFFS_HISTOS
TH1F* bXSigHisto;
TH1F* bYSigHisto;
TH1F* bXBackHisto;
TH1F* bYBackHisto;
#endif//MAKE_FIT_COEFFS_HISTOS

#ifdef MAKE_LAST_STS_NEAREST_Y
TH1F* rightLastStsYHisto;
#endif//MAKE_LAST_STS_NEAREST_Y

LxFinder* LxFinder::Instance()
{
  return fInstance;
}

LxFinder::LxFinder() : muchPixelHits(0), listMCTracks(0), listMuchPts(0), listMuchClusters(0),
    listMuchPixelDigiMatches(0), listStsTracks(0), listStsMatches(0), listStsPts(0), listRecoTracks(0),
    effCounter(*this), fPrimVtx(0), generateInvMass(false), generateBackground(false),
    generateChi2(false), eventNumber(1)
#ifdef CALC_MUCH_DETECTORS_EFF
    , mcPointsCount(0), mcPointsTriggered(0)
#endif//CALC_MUCH_DETECTORS_EFF
{
  fInstance = this;
}

LxFinder::~LxFinder()
{
}

InitStatus LxFinder::Init()
{
  static Int_t nTimes = 1;
  cout << "LxFinder::Init() called at " << nTimes++ << " time" << endl;

  FairRootManager *fManager = FairRootManager::Instance();

  muchPixelHits = LX_DYNAMIC_CAST<TClonesArray*> (fManager->GetObject("MuchPixelHit"));
  listMCTracks = LX_DYNAMIC_CAST<TClonesArray*> (fManager->GetObject("MCTrack"));
  listMuchPts = LX_DYNAMIC_CAST<TClonesArray*> (fManager->GetObject("MuchPoint"));
  listMuchClusters = LX_DYNAMIC_CAST<TClonesArray*> (fManager->GetObject("MuchCluster"));
  listMuchPixelDigiMatches = LX_DYNAMIC_CAST<TClonesArray*> (fManager->GetObject("MuchDigiMatch"));
  listStsTracks = LX_DYNAMIC_CAST<TClonesArray*> (fManager->GetObject("StsTrack"));
  listStsMatches = LX_DYNAMIC_CAST<TClonesArray*> (fManager->GetObject("StsTrackMatch"));
  listStsPts = LX_DYNAMIC_CAST<TClonesArray*> (fManager->GetObject("StsPoint"));
  //fPrimVtx = LX_DYNAMIC_CAST<CbmVertex*> (fManager->GetObject("PrimaryVertex"));
  fPrimVtx = new CbmVertex;

  // Read Z-positions of MUCH station layers and save them in LxLayer objects.

  for (int i = 0; i < LXSTATIONS; ++i)
  {
    for (int j = 0; j < LXLAYERS; ++j)
    {
      TString muchStLrPath = Form("/cave_1/much_0/muchstation%02i_0/muchstation%02ilayer%01i_0",
          i + 1, i + 1, j + 1);
      gGeoManager->cd(muchStLrPath.Data());
      Double_t localCoords[3] = {0., 0., 0.};
      Double_t globalCoords[3];
      gGeoManager->LocalToMaster(localCoords, globalCoords);
      caSpace.stations[i]->layers[j]->zCoord = globalCoords[2];
      //cout << "MUCH station " << i + 1 << " layer " << j + 1 << " Z-coord=" << globalCoords[2] << endl;
    }

    caSpace.stations[i]->zCoord = caSpace.stations[i]->layers[1]->zCoord;
  }

  // Create an output array.
  listRecoTracks = new TClonesArray("CbmMuchTrack", 100);
  fManager->Register("MuchTrack", "Much", listRecoTracks, kTRUE);

  if (generateInvMass)
    massHisto = new TH1F("jpsi_mass", "jpsi_mass", 100, 2., 4.);

  if (generateBackground)
    backgroundMassHisto = new TH1F("background_mass", "background_mass", 400, 2., 4.);

  if (generateChi2)
  {
    signalChi2Histo = new TH1F("signal_chi2", "signal_chi2", 100, 0., 20.);
    backgroundChi2Histo = new TH1F("background_chi2", "background_chi2", 100, 0., 20.);
  }

#ifdef MAKE_HISTOS
  char histoName[128];

  for (int i = 0; i < 6; ++i)
  {
#ifdef MAKE_ANGLE_HISTOS
    sprintf(histoName, "anglesX_%d", i);
    angleXHistos[i] = new TH1F(histoName, histoName, 200, -20., 20.);
    angleXHistos[i]->StatOverflows();
    sprintf(histoName, "anglesY_%d", i);
    angleYHistos[i] = new TH1F(histoName, histoName, 200, -20., 20.);
    angleYHistos[i]->StatOverflows();
    sprintf(histoName, "tangentsX_%d", i);
    tanXHistos[i] = new TH1F(histoName, histoName, 200, -0.4, 0.4);
    tanXHistos[i]->StatOverflows();
    sprintf(histoName, "tangentsY_%d", i);
    tanYHistos[i] = new TH1F(histoName, histoName, 200, -0.4, 0.4);
    tanYHistos[i]->StatOverflows();
#endif//MAKE_ANGLE_HISTOS

#ifdef MAKE_BRAKE_ANGLE_HISTOS
    sprintf(histoName, "breakTangentsX_%d", i);
    breakTanXHistos[i] = new TH1F(histoName, histoName, 200, -0.5, 0.5);
    breakTanXHistos[i]->StatOverflows();
    sprintf(histoName, "breakAnglesX_%d", i);
    breakAngleXHistos[i] = new TH1F(histoName, histoName, 200, -7., 7.);
    breakAngleXHistos[i]->StatOverflows();
    sprintf(histoName, "breakTangentsY_%d", i);
    breakTanYHistos[i] = new TH1F(histoName, histoName, 200, -0.5, 0.5);
    breakTanYHistos[i]->StatOverflows();
    sprintf(histoName, "breakAnglesY_%d", i);
    breakAngleYHistos[i] = new TH1F(histoName, histoName, 200, -7., 7.);
    breakAngleYHistos[i]->StatOverflows();
#endif//MAKE_BRAKE_ANGLE_HISTOS

#ifdef MAKE_DISPERSE_HISTOS
    sprintf(histoName, "disperse01X_%d", i);
    disperse01XHistos[i] = new TH1F(histoName, histoName, 200, -10., 10.);
    disperse01XHistos[i]->StatOverflows();
    sprintf(histoName, "disperse01Y_%d", i);
    disperse01YHistos[i] = new TH1F(histoName, histoName, 200, -10., 10.);
    disperse01YHistos[i]->StatOverflows();
    sprintf(histoName, "disperse02X_%d", i);
    disperse02XHistos[i] = new TH1F(histoName, histoName, 200, -10., 10.);
    disperse02XHistos[i]->StatOverflows();
    sprintf(histoName, "disperse02Y_%d", i);
    disperse02YHistos[i] = new TH1F(histoName, histoName, 200, -10., 10.);
    disperse02YHistos[i]->StatOverflows();
#endif//MAKE_DISPERSE_HISTOS
  }

#ifdef MAKE_MU_PLUS_MU_MINUS_HISTOS
  muPlusAngleXHisto = new TH1F("anglesX", "anglesX", 200, -60., 60.);
  muPlusAngleXHisto->StatOverflows();
  muPlusAngleYHisto = new TH1F("anglesY", "anglesY", 200, -60., 60.);
  muPlusAngleYHisto->StatOverflows();
  muMinusAngleXHisto = new TH1F("anglesX", "anglesX", 200, -60., 60.);
  muMinusAngleXHisto->StatOverflows();
  muMinusAngleYHisto = new TH1F("anglesY", "anglesY", 200, -60., 60.);
  muMinusAngleYHisto->StatOverflows();
#endif//MAKE_MU_PLUS_MU_MINUS_HISTOS
#endif//MAKE_HISTOS

#ifdef MAKE_DIFF_FIT_HISTOS
  stsXDiffFitHisto = new TH1F("STS X diff fit", "STS X diff fit", 200, -5., 5.);
  stsXDiffFitHisto->StatOverflows();
  stsYDiffFitHisto = new TH1F("STS Y diff fit", "STS Y diff fit", 200, -5., 5.);
  stsYDiffFitHisto->StatOverflows();
  stsTXDiffFitHisto = new TH1F("STS TX diff fit", "STS TX diff fit", 200, -10., 10.);
  stsTXDiffFitHisto->StatOverflows();
  stsTYDiffFitHisto = new TH1F("STS TY diff fit", "STS TY diff fit", 200, -10., 10.);
  stsTYDiffFitHisto->StatOverflows();
#endif//MAKE_DIFF_FIT_HISTOS

#ifdef MAKE_DIFF_FIRST_HISTOS
  for (int i = 0; i < 8; ++i)
  {
    sprintf(histoName, "STS X diff first: %d", i);
    stsXDiffFirstHisto[i] = new TH1F(histoName, histoName, 200, -10., 10.);
    stsXDiffFirstHisto[i]->StatOverflows();
    sprintf(histoName, "STS Y diff first: %d", i);
    stsYDiffFirstHisto[i] = new TH1F(histoName, histoName, 200, -10., 10.);
    stsYDiffFirstHisto[i]->StatOverflows();
    sprintf(histoName, "STS TX diff first: %d", i);
    stsTXDiffFirstHisto[i] = new TH1F(histoName, histoName, 200, -0.15, 0.15);
    stsTXDiffFirstHisto[i]->StatOverflows();
    sprintf(histoName, "STS TY diff first: %d", i);
    stsTYDiffFirstHisto[i] = new TH1F(histoName, histoName, 200, -0.15, 0.15);
    stsTYDiffFirstHisto[i]->StatOverflows();
  }
#endif//MAKE_DIFF_FIRST_HISTOS

#ifdef MAKE_FIT_COEFFS_HISTOS
  bXSigHisto = new TH1F("bX signal", "bX signal", 200, -20., 20.);
  bXSigHisto->StatOverflows();
  bYSigHisto = new TH1F("bY signal", "bY signal", 200, -20., 20.);
  bYSigHisto->StatOverflows();

  bXBackHisto = new TH1F("bX background", "bX background", 200, -20., 20.);
  bXBackHisto->StatOverflows();
  bYBackHisto = new TH1F("bY background", "bY background", 200, -20., 20.);
  bYBackHisto->StatOverflows();
#endif//MAKE_FIT_COEFFS_HISTOS

#ifdef MAKE_LAST_STS_NEAREST_Y
  rightLastStsYHisto = new TH1F("Right last STS Y", "Right last STS Y", 2, -1.0, 2.0);
#endif//MAKE_LAST_STS_NEAREST_Y

  effByMomentumProfile = new TProfile("Reconstruction effectivity by momentum", "Reconstruction effectivity by momentum",
      100, 0.0, 50.0, 0.0, 100.0);

#ifdef MAKE_EFF_CALC
  incomplete_events.open("incomplete_events.txt");
#endif//MAKE_EFF_CALC

  return kSUCCESS;
}

static Int_t nTimes = 1;

void LxFinder::Exec(Option_t* opt)
{
  cout << "LxFinder::Exec() called at " << nTimes++ << " time" << endl;
  timeval bTime, eTime;
  int exeDuration;
  gettimeofday(&bTime, 0);

  // Clean all previously created data.
  MCTracks.clear();
  MCPoints.clear();
  MCStsPoints.clear();

  for (int i = 0; i < 8; ++i)
    MCStsPointsByStations[i].clear();

  caSpace.Clear();
  listRecoTracks->Clear();

  // Read MC tracks
  Int_t nEnt = listMCTracks->GetEntries();
  cout << "There are: " << nEnt << " of MC tracks" << endl;
  LxMCTrack mcTrack;

  MCTracks.reserve(nEnt);
  Int_t* root2lxmctrackmap = new Int_t[nEnt];
  Int_t mapCnt = 0;

  for (int i = 0; i < nEnt; ++i)
  {
    CbmMCTrack* mct = LX_DYNAMIC_CAST<CbmMCTrack*> (listMCTracks->At(i));
    mcTrack.p = mct->GetP();
    Int_t pdgCode = mct->GetPdgCode();

    if (abs(pdgCode) >= 10000)
    {
      root2lxmctrackmap[i] = -1;
      continue;
    }

    root2lxmctrackmap[i] = mapCnt++;

    mcTrack.q = TDatabasePDG::Instance()->GetParticle(pdgCode)->Charge() / 3.0;
    mcTrack.x = mct->GetStartX();
    mcTrack.y = mct->GetStartY();
    mcTrack.z = mct->GetStartZ();
    mcTrack.px = mct->GetPx();
    mcTrack.py = mct->GetPy();
    mcTrack.pz = mct->GetPz();
    mcTrack.mother_ID = mct->GetMotherId();
    mcTrack.fUniqueID = mct->GetUniqueID();
    mcTrack.pdg = pdgCode;
    mcTrack.externalTrack = 0;
    MCTracks.push_back(mcTrack);
  }

  nEnt = listStsPts->GetEntries();
  cout << "There are: " << nEnt << " of STS MC points" << endl;

  for (int i = 0; i < nEnt; ++i)
  {
    TVector3 xyzI, PI, xyzO, PO;
    CbmStsPoint* stsPt = LX_DYNAMIC_CAST<CbmStsPoint*> (listStsPts->At(i));

    if (0 == stsPt)
      continue;

    LxStsMCPoint stsMCPoint;
    stsPt->Position(xyzI);
    stsPt->Momentum(PI);
    stsPt->PositionOut(xyzO);
    stsPt->MomentumOut(PO);
    TVector3 xyz = .5 * (xyzI + xyzO);
    TVector3 P = .5 * (PI + PO);
    stsMCPoint.x  = xyz.X();
    stsMCPoint.y  = xyz.Y();
    stsMCPoint.z  = xyz.Z();
    stsMCPoint.px = P.X();
    stsMCPoint.py = P.Y();
    stsMCPoint.pz = P.Z();
    stsMCPoint.p = sqrt(fabs(stsMCPoint.px * stsMCPoint.px + stsMCPoint.py * stsMCPoint.py + stsMCPoint.pz * stsMCPoint.pz));
    stsMCPoint.stationNumber = (stsMCPoint.z - 28) / 10;
    Int_t trackId = root2lxmctrackmap[stsPt->GetTrackID()];

    if (-1 != trackId)
    {
      stsMCPoint.mcTrack = &MCTracks[trackId];
      MCStsPoints.push_back(stsMCPoint);
      MCTracks[trackId].stsPoints[stsMCPoint.stationNumber].push_back(&MCStsPoints.back());
      MCStsPointsByStations[stsMCPoint.stationNumber].push_back(&MCStsPoints.back());
    }
  }

  nEnt = listMuchPts->GetEntries();
  cout << "There are: " << nEnt << " of MUCH MC points" << endl;
  LxMCPoint mcPoint;

  MCPoints.reserve(nEnt);
  Int_t* root2lxmcpointmap = new Int_t[nEnt];// Unfortunately we have to use this map because in the loop
                                             // below some iterations can not to produce a point.
  mapCnt = 0;
  Int_t mcPtsCount = nEnt;

  for (int i = 0; i < nEnt; ++i)
  {
    TVector3 xyzI, PI, xyzO, PO;
    /*CbmStsPoint* stsPt = LX_DYNAMIC_CAST<CbmStsPoint*> (listStsPts->At(i));

    if (stsPt)
    {
      LxStsMCPoint stsMCPoint;
      stsPt->Position(xyzI);
      stsPt->Momentum(PI);
      stsPt->PositionOut(xyzO);
      stsPt->MomentumOut(PO);
      TVector3 xyz = .5 * (xyzI + xyzO);
      TVector3 P = .5 * (PI + PO);
      stsMCPoint.x  = xyz.X();
      stsMCPoint.y  = xyz.Y();
      stsMCPoint.z  = xyz.Z();
      stsMCPoint.px = P.X();
      stsMCPoint.py = P.Y();
      stsMCPoint.pz = P.Z();
      stsMCPoint.p = sqrt(fabs(stsMCPoint.px * stsMCPoint.px + stsMCPoint.py * stsMCPoint.py + stsMCPoint.pz * stsMCPoint.pz));
      stsMCPoint.stationNumber = (stsMCPoint.z - 28) / 10;
      Int_t trackId = root2lxmctrackmap[stsPt->GetTrackID()];

      if (-1 != trackId)
      {
        stsMCPoint.mcTrack = &MCTracks[trackId];
        MCStsPoints.push_back(stsMCPoint);
        MCTracks[trackId].stsPoints[stsMCPoint.stationNumber].push_back(&MCStsPoints.back());
        MCStsPointsByStations[stsMCPoint.stationNumber].push_back(&MCStsPoints.back());
      }
    }*/

    CbmMuchPoint* pt = LX_DYNAMIC_CAST<CbmMuchPoint*> (listMuchPts->At(i));

    if (!pt)
    {
      root2lxmcpointmap[i] = -1;
      continue;
    }

    Int_t trackId = root2lxmctrackmap[pt->GetTrackID()];

    if (-1 == trackId)
    {
      root2lxmcpointmap[i] = -1;
      continue;
    }

    root2lxmcpointmap[i] = mapCnt++;

    pt->Position(xyzI);
    pt->Momentum(PI);
    pt->PositionOut(xyzO);
    pt->MomentumOut(PO);
    TVector3 xyz = .5 * (xyzI + xyzO);
    TVector3 P = .5 * (PI + PO);
    mcPoint.x  = xyz.X();
    mcPoint.y  = xyz.Y();
    mcPoint.z  = xyz.Z();
    mcPoint.px = P.X();
    mcPoint.py = P.Y();
    mcPoint.pz = P.Z();
    mcPoint.p = sqrt(fabs(mcPoint.px * mcPoint.px + mcPoint.py * mcPoint.py + mcPoint.pz * mcPoint.pz));
    mcPoint.stationNumber = CbmMuchGeoScheme::GetStationIndex(pt->GetDetectorId());
    mcPoint.layerNumber = CbmMuchGeoScheme::GetLayerIndex(pt->GetDetectorId());
    MCPoints.push_back(mcPoint);
    Int_t ptId = root2lxmcpointmap[i];

#ifdef MAKE_HISTOS
    Double_t trackPt2 = MCTracks[trackId].px * MCTracks[trackId].px + MCTracks[trackId].py * MCTracks[trackId].py;
#endif//MAKE_HISTOS

#ifdef MAKE_MU_PLUS_MU_MINUS_HISTOS
    if ((13 == MCTracks[trackId].pdg || -13 == MCTracks[trackId].pdg) && MCTracks[trackId].mother_ID < 0 && MCTracks[trackId].p >= 3 && trackPt2 >= 1)
    {
      Double_t tx = mcPoint.px / mcPoint.pz;
      Double_t angleX = atan(tx) * 180 / 3.1415926535;
      Double_t ty = mcPoint.py / mcPoint.pz;
      Double_t angleY = atan(ty) * 180 / 3.1415926535;

      if (-13 == MCTracks[trackId].pdg)
      {
        muPlusAngleXHisto->Fill(angleX);
        muPlusAngleYHisto->Fill(angleY);
      }
      else
      {
        muMinusAngleXHisto->Fill(angleX);
        muMinusAngleYHisto->Fill(angleY);
      }
    }
#endif//MAKE_MU_PLUS_MU_MINUS_HISTOS

    MCTracks[trackId].Points.push_back(&MCPoints[ptId]);
  }

#ifdef MAKE_HISTOS
  // Build angle (tangent) breaks distribution <
  for (vector<LxMCTrack>::iterator i = MCTracks.begin(); i != MCTracks.end(); ++i)
  {
    LxMCTrack& track = *i;

    if ((13 != track.pdg && -13 == track.pdg) || track.mother_ID >= 0)
      continue;

    if (track.p < 3)
      continue;

    Double_t pt2 = track.px * track.px + track.py * track.py;

    if (pt2 < 1)
      continue;

    LxMCPoint* points[LXSTATIONS][LXLAYERS];
    memset(points, 0, sizeof(points));

    for (vector<LxMCPoint*>::iterator j = track.Points.begin(); j != track.Points.end(); ++j)
    {
      LxMCPoint* point = *j;
      points[point->stationNumber][point->layerNumber] = point;
    }

    for (int j = 0; j < LXSTATIONS; ++j)
    {
#ifdef MAKE_ANGLE_HISTOS
      if (j < LXSTATIONS - 1)
      {
        LxMCPoint* point0 = points[j][1];
        LxMCPoint* point1 = points[j + 1][1];

        if (0 != point0 && 0 != point1)
        {
          Double_t diffZ = point0->z - point1->z;
          Double_t tx1 = point1->x / point1->z;
          Double_t ty1 = point1->y / point1->z;
          Double_t tx = (point0->x - point1->x) / diffZ;
          Double_t ty = (point0->y - point1->y) / diffZ;

          tanXHistos[point1->stationNumber]->Fill(tx - tx1);
          Double_t angleX = atan(tx - tx1) * 180 / 3.1415926535;
          angleXHistos[point1->stationNumber]->Fill(angleX);

          tanYHistos[point1->stationNumber]->Fill(ty - ty1);
          Double_t angleY = atan(ty - ty1) * 180 / 3.1415926535;
          angleYHistos[point1->stationNumber]->Fill(angleY);
        }
      }
#endif//MAKE_ANGLE_HISTOS

#ifdef MAKE_BRAKE_ANGLE_HISTOS
      if (j < LXSTATIONS - 2)
      {
        LxMCPoint* point0 = points[j][1];
        LxMCPoint* point1 = points[j + 1][1];
        LxMCPoint* point2 = points[j + 2][1];

        if (0 != point0 && 0 != point1 && 0 != point2)
        {
          Double_t tanX1 = (point1->x - point0->x) / (point1->z - point0->z);
          Double_t tanX2 = (point2->x - point1->x) / (point2->z - point1->z);
          Double_t tanX = tanX2 - tanX1;
          breakTanXHistos[j + 1]->Fill(tanX);
          Double_t angleX = atan(tanX) * 180 / 3.1415926535;
          breakAngleXHistos[j + 1]->Fill(angleX);
          Double_t tanY1 = (point1->y - point0->y) / (point1->z - point0->z);
          Double_t tanY2 = (point2->y - point1->y) / (point2->z - point1->z);
          Double_t tanY = tanY2 - tanY1;
          breakTanYHistos[j + 1]->Fill(tanY);
          Double_t angleY = atan(tanY) * 180 / 3.1415926535;
          breakAngleYHistos[j + 1]->Fill(angleY);
        }
      }
#endif//MAKE_BRAKE_ANGLE_HISTOS

#ifdef MAKE_DISPERSE_HISTOS
      if (0 != points[j][0] && 0 != points[j][1])
      {
        Double_t tx = points[j][0]->x / points[j][0]->z;
        Double_t ty = points[j][0]->y / points[j][0]->z;
        Double_t diffZ = points[j][1]->z - points[j][0]->z;
        Double_t diffX = points[j][1]->x - points[j][0]->x - tx * diffZ;
        Double_t diffY = points[j][1]->y - points[j][0]->y - ty * diffZ;
        disperse01XHistos[j]->Fill(diffX);
        disperse01YHistos[j]->Fill(diffY);
      }

      if (0 != points[j][1] && 0 != points[j][2])
      {
        Double_t tx = points[j][2]->x / points[j][2]->z;
        Double_t ty = points[j][2]->y / points[j][2]->z;
        Double_t diffZ = points[j][1]->z - points[j][2]->z;
        Double_t diffX = points[j][1]->x - points[j][2]->x - tx * diffZ;
        Double_t diffY = points[j][1]->y - points[j][2]->y - ty * diffZ;
        disperse01XHistos[j]->Fill(diffX);
        disperse01YHistos[j]->Fill(diffY);
      }

      if (0 != points[j][0] && 0 != points[j][2])
      {
        Double_t tx = points[j][0]->x / points[j][0]->z;
        Double_t ty = points[j][0]->y / points[j][0]->z;
        Double_t diffZ = points[j][2]->z - points[j][0]->z;
        Double_t diffX = points[j][2]->x - points[j][0]->x - tx * diffZ;
        Double_t diffY = points[j][2]->y - points[j][0]->y - ty * diffZ;
        disperse02XHistos[j]->Fill(diffX);
        disperse02YHistos[j]->Fill(diffY);
      }
#endif//MAKE_DISPERSE_HISTOS
    }//for (int j = 0; j < LXSTATIONS; ++j)
  }//for (vector<LxMCTrack>::iterator i = MCTracks.begin(); i != MCTracks.end(); ++i)
  // > angle (tangent) breaks distribution
#endif//MAKE_HISTOS

  // Read MUCH pixel hits.
  nEnt = muchPixelHits->GetEntries();
  cout << "There are: " << nEnt << " of MUCH pixel hits" << endl;

  for (int i = 0; i < nEnt; ++i)
  {
    CbmMuchPixelHit* mh = LX_DYNAMIC_CAST<CbmMuchPixelHit*> (muchPixelHits->At(i));

    Int_t stationNumber = CbmMuchGeoScheme::GetStationIndex(mh->GetAddress());
    Int_t layerNumber = CbmMuchGeoScheme::GetLayerIndex(mh->GetAddress());
    TVector3 pos, err;
    mh->Position(pos);
    mh->PositionError(err);
    //cout << "HIT Z-coord=" << pos.Z() << endl;
    //cout << "stNumber=" << stNumber << ", Z=" << pos.Z() << ", errX=" << err.X() << ", errY=" << err.Y() << ", errZ=" << err.Z() << endl;
    //cout << "stNumber=" << stNumber << ", Z=" << pos.Z() << ", X=" << pos.X() << ", errX=" << err.X() << ", Y=" << pos.Y() << ", errY=" << err.Y() << endl;
    //muchStations[stNumber].push_back(mh);

    // Create an internal Lx representation of a hit-point.
    //LxPoint* lxPoint = new LxPoint(pos.X(), pos.Y(), pos.Z(), err.X(), err.Y(), err.Z(),
        //caSpace.stations[stationNumber]->layers[layerNumber]);
    //lxPoint->hitId = i;// Need this when serialize reconstructed tracks.
    // And store it in the respective layer on the respective station.
    //caSpace.stations[stationNumber]->layers[layerNumber]->points.push_back(lxPoint);
    Double_t x = pos.X();
    Double_t y = pos.Y();
    Double_t z = pos.Z();

    if (x != x || y != y || z != z)// Test for NaN
      continue;

    LxPoint* lxPoint = caSpace.AddPoint(stationNumber, layerNumber, i, x, y, z, err.X(), err.Y(), err.Z());

    Int_t clusterId = mh->GetRefId();
    CbmMuchCluster* cluster = LX_DYNAMIC_CAST<CbmMuchCluster*> (listMuchClusters->At(clusterId));
    Int_t nDigis = cluster->GetNofDigis();

    for (int j = 0; j < nDigis; ++j)
    {
      //Migration CbmMuchDigiMatch* digiMatch = LX_DYNAMIC_CAST<CbmMuchDigiMatch*> (listMuchPixelDigiMatches->At(cluster->GetDigiIndex(j)));
      CbmMuchDigiMatch* digiMatch = LX_DYNAMIC_CAST<CbmMuchDigiMatch*> (listMuchPixelDigiMatches->At(cluster->GetDigi(j)));
      //Migration Int_t nMCs = digiMatch->GetNPoints();
      //Int_t nMCs = digiMatch->GetNoPrimaryElectrons();
      Int_t nMCs = digiMatch->GetNofLinks();

      for (int k = 0; k < nMCs; ++k)
      {
        //Migration Int_t mcIndex = digiMatch->GetRefIndex(k);
        //Int_t mcIndex = digiMatch->GetRefIndexPerPrimaryElectron(k);
        //Int_t mcIndex = digiMatch->GetLink(k).GetIndex();
        const CbmLink& lnk = digiMatch->GetLink(k);
        Int_t mcIndex = lnk.GetIndex();

        if (mcIndex >= mcPtsCount)
          continue;

        Int_t mcIndexMapped = root2lxmcpointmap[mcIndex];

        if (-1 == mcIndexMapped)
          continue;

        MCPoints[mcIndexMapped].lxPoints.push_back(lxPoint);
      }
    }
  }

  /*cout << "Numbers of hits:" << endl;

  for (int i = 0; i < LXSTATIONS; ++i)
  {
    cout << "On station" << i << ":" << endl;

    for (int j = 0; j < LXLAYERS; ++j)
      cout << "On layer" << j << ": " << caSpace.stations[i]->layers[j]->points.size() << endl;
  }*/

#ifdef CALC_MUCH_DETECTORS_EFF
  mcPointsCount += MCPoints.size();

  for (vector<LxMCPoint>::iterator i = MCPoints.begin(); i != MCPoints.end(); ++i)
  {
    if (i->lxPoints.size() > 0)
      ++mcPointsTriggered;
  }

  Double_t detectorEff = 100 * mcPointsTriggered;
  detectorEff /= mcPointsCount;
  cout << "MUCH detector effectivity: " << detectorEff << "( " << mcPointsTriggered << " / " << mcPointsCount << " )" << endl;
#endif//CALC_MUCH_DETECTORS_EFF

  // At last start invocation of CA procedures.
  gettimeofday(&eTime, 0);
  exeDuration = (eTime.tv_sec - bTime.tv_sec) * 1000000 + eTime.tv_usec - bTime.tv_usec;
  cout << "Execution duration 1 was: " << exeDuration << endl;
  bTime.tv_sec = eTime.tv_sec;
  bTime.tv_usec = eTime.tv_usec;
  caSpace.RestoreMiddlePoints();
  caSpace.BuildRays();
  caSpace.ConnectNeighbours();
  caSpace.Reconstruct();
  caSpace.FitTracks();
  gettimeofday(&eTime, 0);
  exeDuration = (eTime.tv_sec - bTime.tv_sec) * 1000000 + eTime.tv_usec - bTime.tv_usec;
  cout << "Execution duration 2 was: " << exeDuration << endl;

  /////////////////////////////////////////////////////////////////////////////////
  // Join MUCH-tracks with STS-tracks.
  /////////////////////////////////////////////////////////////////////////////////

  // STS-tracks fulfilling the limitations peculiar for muons are picked up in the loop below.
  nEnt = listStsTracks->GetEntries();

  for (int i = 0; i < nEnt; ++i)
  {
    CbmStsTrack* stsTrack = LX_DYNAMIC_CAST<CbmStsTrack*> (listStsTracks->At(i));

    CbmStsTrack aTrack = *stsTrack;
    extFitter.DoFit(&aTrack, 13);
    Double_t chi2Prim = extFitter.GetChiToVertex(&aTrack, fPrimVtx);
    FairTrackParam params;
    extFitter.Extrapolate(&aTrack, fPrimVtx->GetZ(), &params);
    Double_t p = 1 / params.GetQp();
    Double_t p2 = p * p;

    if (p2 < 9)
      continue;

    Double_t tx2 = params.GetTx() * params.GetTx();
    Double_t ty2 = params.GetTy() * params.GetTy();
    Double_t pt2 = p2 * (tx2 + ty2) / (1 + tx2 + ty2);

    if (pt2 < 1)
      continue;

    LxExtTrack extTrack;
    extTrack.track = stsTrack;
    extTrack.extId = i;

    CbmTrackMatch* match = LX_DYNAMIC_CAST<CbmTrackMatch*> (listStsMatches->At(i));
    Int_t numberOfHits = match->GetNofTrueHits() + match->GetNofWrongHits() + match->GetNofFakeHits();

    if (match->GetNofTrueHits() >= 0.7 * numberOfHits)
    {
      Int_t mcTrackId = match->GetMCTrackId();
      Int_t mappedId = root2lxmctrackmap[mcTrackId];

      if (-1 != mappedId)
      {
        MCTracks[mappedId].externalTrack = stsTrack;
        extTrack.mcTrack = &MCTracks[mappedId];
      }
    }

    caSpace.extTracks.push_back(extTrack);
  }

  // Join reconstructed tracks with external tracks.
  caSpace.JoinExtTracks();

  // Measure a reconstruction effectivity.
  Double_t eff = effCounter.CalcRecoEff(true);// Parameter instructs whether join with external tracks or not.
#ifdef CALC_LINK_WITH_STS_EFF
  eff = effCounter.CalcLinkEff();
#endif//CALC_LINK_WITH_STS_EFF

  // Draw some result.
  //static LxDraw drawer;

  //drawer.ClearView();
  //drawer.DrawMuch();
  //drawer.SaveCanvas("Geometry_");

  //drawer.ClearView();
  //drawer.DrawInputHits();
  //drawer.SaveCanvas("Hits_");
  //drawer.Ask();

  /*drawer.ClearView();
  drawer.DrawInputHits();
  drawer.DrawMCTracks();
  drawer.SaveCanvas("MC_");
  drawer.Ask();*/

  //drawer.ClearView();
  //drawer.DrawInputHits();
  //drawer.DrawRays();
  //drawer.Ask();

  /*drawer.ClearView();
  drawer.DrawInputHits();
  drawer.DrawRecoTracks();
  drawer.SaveCanvas("Reco_");
  drawer.Ask();*/

  //drawer.ClearView();
  //drawer.DrawInputHits();
  //drawer.DrawRecoTracks();
  //drawer.DrawExtTracks();
  //drawer.Ask();

  if (generateInvMass)
    CalcInvMass();

  if (generateBackground)
    SaveEventTracks();

  // Finally save reconstructed tracks.
  SaveRecoTracks();

  // Hopefully we don't need these two maps any more.
  delete[] root2lxmctrackmap;
  delete[] root2lxmcpointmap;
  ++eventNumber;
}

void LxFinder::SaveRecoTracks()
{
  Int_t trackNo = listRecoTracks->GetEntriesFast();

  cout << "LxFinder::SaveRecoTracks() there are: " << caSpace.tracks.size() << " reconstructed tracks" << endl;

  for (list<LxTrack*>::iterator i = caSpace.tracks.begin(); i != caSpace.tracks.end(); ++i)
  {
    LxTrack* recoTrack = *i;
    CbmMuchTrack muchTrack;

    for (int j = 0; j < LXSTATIONS * LXLAYERS; ++j)
    {
      LxPoint* pPoint = recoTrack->points[j];

      if (0 != pPoint)
        muchTrack.AddHit(pPoint->hitId, kMUCHPIXELHIT);
    }

    muchTrack.SetChiSq(recoTrack->chi2);
    muchTrack.SetNDF(4 * recoTrack->length);// Probably need to calculate it more accurately.

    FairTrackParam parFirst;

    if (0 != recoTrack->externalTrack.track)
    {
      muchTrack.SetPreviousTrackId(recoTrack->externalTrack.extId);
      parFirst = *recoTrack->externalTrack.track->GetParamLast();
    }

    muchTrack.SetFlag(0);// Good track.

    FairTrackParam parLast(parFirst);
    muchTrack.SetParamFirst(&parFirst);
    muchTrack.SetParamLast(&parLast);

    new ((*listRecoTracks)[trackNo++]) CbmMuchTrack(muchTrack);
  }

  cout << "LxFinder::SaveRecoTracks() there are: " << listRecoTracks->GetEntriesFast() << " tracks stored" << endl;
}

void LxFinder::CalcInvMass()
{
  for (list<LxTrack*>::iterator i = caSpace.tracks.begin(); i != caSpace.tracks.end(); ++i)
  {
    LxTrack* firstTrack = *i;
    LxMCTrack* mcTrack1 = firstTrack->mcTrack;
    LxMCTrack* mcTrack2 = firstTrack->externalTrack.mcTrack;

    if (mcTrack1)
    {
      Int_t pdg1 = mcTrack1 ? mcTrack1->pdg : 10000;
      map<Int_t, map<Int_t, int> >::iterator j = particleCounts.find(pdg1);

      if (j == particleCounts.end())
        j = particleCounts.insert(pair<Int_t, map<Int_t, int> > (pdg1, map<Int_t, int> ())).first;

      Int_t pdg2 = mcTrack2 ? mcTrack2->pdg : 10000;

      map<Int_t, int>::iterator k = j->second.find(pdg2);

      if (k != j->second.end())
        ++(k->second);
      else
        j->second.insert(pair<Int_t, int> (pdg2, 1));
    }

    if (0 == firstTrack->externalTrack.track)
      continue;

    CbmStsTrack t1 = *firstTrack->externalTrack.track;

    // Add entry to the respective chi2 histogram if needed. This can be used for placing chi2 cuts determining J/psi.
    // pdg == 13 | -13 -- it is a muon.
    // mother_ID < 0 -- it is a primary particle. URQMD doesn't produce primary muons. So it from PLUTO <=> is J/psi.

    if (generateChi2)
    {
      Double_t normalizedChi2 = firstTrack->chi2 / (firstTrack->length * 4);// length * 4 == NDF.

      if (mcTrack2 && (mcTrack2->pdg == 13 || mcTrack2->pdg == -13) && mcTrack2->mother_ID < 0)
        signalChi2Histo->Fill(normalizedChi2);
      else
        backgroundChi2Histo->Fill(normalizedChi2);
    }

    if (!mcTrack2 || (mcTrack2->pdg != 13 && mcTrack2->pdg != -13) || mcTrack2->mother_ID >= 0)
      continue;

    extFitter.DoFit(&t1, 13);
    Double_t chi2Prim = extFitter.GetChiToVertex(&t1, fPrimVtx);
    FairTrackParam t1param;
    extFitter.Extrapolate(&t1, fPrimVtx->GetZ(), &t1param);

    if (t1param.GetQp() <= 0)
      continue;

    Double_t p1 = 1 / t1param.GetQp();
    Double_t tx12 = t1param.GetTx() * t1param.GetTx();
    Double_t ty12 = t1param.GetTy() * t1param.GetTy();
    Double_t pt12 = p1 * p1 * (tx12 + ty12) / (1 + tx12 + ty12);

    if (pt12 < 1)
      continue;

    CbmKFTrack muPlus(t1);

    list<LxTrack*>::iterator j = i;
    ++j;

    for (; j != caSpace.tracks.end(); ++j)
    {
      LxTrack* secondTrack = *j;

      if (0 == secondTrack->externalTrack.track)
        continue;

      LxMCTrack* mcSecondTrack = secondTrack->externalTrack.mcTrack;

      if (!mcSecondTrack || (mcSecondTrack->pdg != 13 && mcSecondTrack->pdg != -13) || mcSecondTrack->mother_ID >= 0)
        continue;

      CbmStsTrack t2 = *secondTrack->externalTrack.track;
      extFitter.DoFit(&t2, 13);
      chi2Prim = extFitter.GetChiToVertex(&t2, fPrimVtx);
      FairTrackParam t2param;
      extFitter.Extrapolate(&t2, fPrimVtx->GetZ(), &t2param);

      if (t2param.GetQp() >= 0)
        continue;

      Double_t p2 = 1 / t2param.GetQp();
      Double_t tx22 = t2param.GetTx() * t2param.GetTx();
      Double_t ty22 = t2param.GetTy() * t2param.GetTy();
      Double_t pt22 = p2 * p2 * (tx22 + ty22) / (1 + tx22 + ty22);

      if (pt22 < 1)
        continue;

      CbmKFTrack muMinus(t2);
      vector<CbmKFTrackInterface*> kfData;
      kfData.push_back(&muPlus);
      kfData.push_back(&muMinus);
      CbmKFParticle DiMu;
      DiMu.Construct(kfData, 0);
      DiMu.TransportToDecayVertex();
      Double_t m, merr;
      DiMu.GetMass(m, merr);

      massHisto->Fill(m);
    }
  }
}

//#ifdef MAKE_HISTOS
// It also deletes the histogram.
static void SaveHisto(TH1F* histo, const char* name)
{
  TFile fh(name, "RECREATE");
  histo->Write();
  fh.Close();
  delete histo;
}
//#endif//MAKE_HISTOS

void LxFinder::FinishTask()
{
  if (generateInvMass)
    SaveInvMass();

  if (generateBackground)
    SaveBackground();

  /*if (generateChi2)
  {
    SaveSignalChi2();
    SaveBackgroundChi2();
  }

  cout << "Statistics on particles found:" << endl;

  for (map<Int_t, map<Int_t, int> >::iterator i = particleCounts.begin(); i != particleCounts.end(); ++i)
  {
    string muchParticle = (10000 <= i->first || -10000 >= i->first) ? "Undefined MUCH particle" : TDatabasePDG::Instance()->GetParticle(i->first)->GetName();
    cout << "For MUCH particle [ " << muchParticle << " ] ( PDG = " << i->first << " ) found STS joints to:" << endl;

    for (map<Int_t, int>::iterator j = i->second.begin(); j != i->second.end(); ++j)
    {
      string stsParticle = (10000 <= j->first || -10000 >= j->first) ? "Undefined STS particle" : TDatabasePDG::Instance()->GetParticle(j->first)->GetName();
      cout << "\t" << j->second << " [ " << stsParticle << " ] ( PDG = " << j->first << " )" << endl;
    }
  }*/
  TFile* curFile = TFile::CurrentFile();
#ifdef MAKE_HISTOS
  char histoFileName[128];

  for (int i = 0; i < 6; ++i)
  {
#ifdef MAKE_ANGLE_HISTOS
    sprintf(histoFileName, "anglesX_histo_%d.root", i);
    SaveHisto(angleXHistos[i], histoFileName);

    sprintf(histoFileName, "anglesY_histo_%d.root", i);
    SaveHisto(angleYHistos[i], histoFileName);

    sprintf(histoFileName, "tangentsX_histo_%d.root", i);
    SaveHisto(tanXHistos[i], histoFileName);

    sprintf(histoFileName, "tangentsY_histo_%d.root", i);
    SaveHisto(tanYHistos[i], histoFileName);
#endif//MAKE_ANGLE_HISTOS

#ifdef MAKE_BRAKE_ANGLE_HISTOS
    sprintf(histoFileName, "breakAnglesX_histo_%d.root", i);
    SaveHisto(breakAngleXHistos[i], histoFileName);

    sprintf(histoFileName, "breakAnglesY_histo_%d.root", i);
    SaveHisto(breakAngleYHistos[i], histoFileName);

    sprintf(histoFileName, "breakTangentsX_histo_%d.root", i);
    SaveHisto(breakTanXHistos[i], histoFileName);

    sprintf(histoFileName, "breakTangentsY_histo_%d.root", i);
    SaveHisto(breakTanYHistos[i], histoFileName);
#endif//MAKE_BRAKE_ANGLE_HISTOS

#ifdef MAKE_DISPERSE_HISTOS
    sprintf(histoFileName, "disperse01X_histo_%d.root", i);
    SaveHisto(disperse01XHistos[i], histoFileName);

    sprintf(histoFileName, "disperse01Y_histo_%d.root", i);
    SaveHisto(disperse01YHistos[i], histoFileName);

    sprintf(histoFileName, "disperse02X_histo_%d.root", i);
    SaveHisto(disperse02XHistos[i], histoFileName);

    sprintf(histoFileName, "disperse02Y_histo_%d.root", i);
    SaveHisto(disperse02YHistos[i], histoFileName);
#endif//MAKE_DISPERSE_HISTOS
  }

#ifdef MAKE_MU_PLUS_MU_MINUS_HISTOS
  SaveHisto(muPlusAngleXHisto, "muPlusAnglesX_histo.root");
  SaveHisto(muPlusAngleYHisto, "muPlusAnglesY_histo.root");
  SaveHisto(muMinusAngleXHisto, "muMinusAnglesX_histo.root");
  SaveHisto(muMinusAngleYHisto, "muMinusAnglesY_histo.root");
#endif//MAKE_MU_PLUS_MU_MINUS_HISTOS

#ifdef MAKE_DIFF_FIT_HISTOS
  SaveHisto(stsXDiffFitHisto, "stsXDiff_fit_histo.root");
  SaveHisto(stsYDiffFitHisto, "stsYDiff_fit_histo.root");
  SaveHisto(stsTXDiffFitHisto, "stsTXDiff_fit_histo.root");
  SaveHisto(stsTYDiffFitHisto, "stsTYDiff_fit_histo.root");
#endif//MAKE_DIFF_FIT_HISTOS

#ifdef MAKE_DIFF_FIRST_HISTOS
  for (int i = 0; i < 8; ++i)
  {
    sprintf(histoFileName, "stsXDiff_first_histo_%d.root", i);
    SaveHisto(stsXDiffFirstHisto[i], histoFileName);
    sprintf(histoFileName, "stsYDiff_first_histo_%d.root", i);
    SaveHisto(stsYDiffFirstHisto[i], histoFileName);
    sprintf(histoFileName, "stsTXDiff_first_histo_%d.root", i);
    SaveHisto(stsTXDiffFirstHisto[i], histoFileName);
    sprintf(histoFileName, "stsTYDiff_first_histo_%d.root", i);
    SaveHisto(stsTYDiffFirstHisto[i], histoFileName);
  }
#endif//MAKE_DIFF_FIRST_HISTOS

#ifdef MAKE_FIT_COEFFS_HISTOS
  SaveHisto(bXSigHisto, "bXSignal_histo.root");
  SaveHisto(bYSigHisto, "bYSignal_histo.root");
  SaveHisto(bXBackHisto, "bXBackground_histo.root");
  SaveHisto(bYBackHisto, "bYBackground_histo.root");
#endif//MAKE_FIT_COEFFS_HISTOS

#ifdef MAKE_LAST_STS_NEAREST_Y
  SaveHisto(rightLastStsYHisto, "rightLastStsY_histo.root");
#endif//MAKE_LAST_STS_NEAREST_Y
#endif//MAKE_HISTOS

  {
    TFile fh("effByMomentumProfile.root", "RECREATE");
    effByMomentumProfile->Write();
    fh.Close();
    delete effByMomentumProfile;
  }

  TFile::CurrentFile() = curFile;

#ifdef MAKE_EFF_CALC
  incomplete_events.close();
#endif//MAKE_EFF_CALC

  FairTask::FinishTask();
}

void LxFinder::SaveInvMass()
{
  TFile* curFile = TFile::CurrentFile();

  TFile fh("jpsi_inv_mass_histo.root", "RECREATE");
  massHisto->Write();
  fh.Close();
  delete massHisto;

  TFile::CurrentFile() = curFile;
}

void LxFinder::SaveBackground()
{
  for (list<CbmStsTrack>::iterator i = positiveTracks.begin(); i != positiveTracks.end(); ++i)
  {
    CbmStsTrack& t1 = *i;
    CbmKFTrack muPlus(t1);

    for (list<CbmStsTrack>::iterator j = negativeTracks.begin(); j != negativeTracks.end(); ++j)
    {
      CbmStsTrack& t2 = *j;
      CbmKFTrack muMinus(t2);
      vector<CbmKFTrackInterface*> kfData;
      kfData.push_back(&muPlus);
      kfData.push_back(&muMinus);
      CbmKFParticle DiMu;
      DiMu.Construct(kfData, 0);
      DiMu.TransportToDecayVertex();
      Double_t m, merr;
      DiMu.GetMass(m, merr);

      for (int k = 0; k < 1000; ++k)
        backgroundMassHisto->Fill(m);
    }
  }

  TFile* curFile = TFile::CurrentFile();

  TFile fh("background_inv_mass_histo.root", "RECREATE");
  backgroundMassHisto->Write();
  fh.Close();
  delete backgroundMassHisto;

  TFile::CurrentFile() = curFile;
}

void LxFinder::SaveSignalChi2()
{
  TFile* curFile = TFile::CurrentFile();

  TFile fh("signal_chi2_histo.root", "RECREATE");
  signalChi2Histo->Write();
  fh.Close();
  delete signalChi2Histo;

  TFile::CurrentFile() = curFile;
}

void LxFinder::SaveBackgroundChi2()
{
  TFile* curFile = TFile::CurrentFile();

  TFile fh("background_chi2_histo.root", "RECREATE");
  backgroundChi2Histo->Write();
  fh.Close();
  delete backgroundChi2Histo;

  TFile::CurrentFile() = curFile;
}
