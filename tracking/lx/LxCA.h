#ifndef LXCA_INCLUDED
#define LXCA_INCLUDED

#include "Rtypes.h"
#include <list>
#include <math.h>
#include "LxSettings.h"
#include "LxMC.h"

struct LxRay;
struct LxLayer;
struct LxTrack;
struct LxStation;
struct LxSpace;
struct LxTrackCandidate;

struct LxPoint
{
  Double_t x, y, z;
  Double_t dx, dy, dz;
  bool used;
  bool valid;
  bool artificial;
  LxTrack* track;
  std::list<LxRay*> rays;
  LxLayer* layer;
  Int_t hitId;

  LxPoint(Double_t X, Double_t Y, Double_t Z, Double_t Dx, Double_t Dy, Double_t Dz, LxLayer* lay, int hId, bool isArtificial = false) :
      x(X), y(Y), z(Z), dx(Dx), dy(Dy), dz(Dz), layer(lay), used(false), valid(true), track(0), hitId(hId), artificial(isArtificial) {}
  ~LxPoint();
  void CreateRay(LxPoint* lPoint);
};

struct LxRay
{
  Double_t tx, ty;
  Double_t dtx, dty;
  LxPoint* source;
  LxPoint* end;
  LxStation* station;
  std::list<LxRay*> neighbours;
  LxRay(LxPoint* s, LxPoint* e);
};

struct LxLayer
{
  std::list<LxPoint*> points;
  LxStation* station;
  int layerNumber;
  Double_t zCoord;
  LxLayer(LxStation* st, int lNum);
  ~LxLayer();
  void Clear();

  LxPoint* AddPoint(int hitId, Double_t x, Double_t y, Double_t z, Double_t dx, Double_t dy, Double_t dz, bool isArtificial = false)
  {
    LxPoint* result = new LxPoint(x, y, z, dx, dy, dz, this, hitId, isArtificial);
    points.push_back(result);
    return result;
  }

  LxPoint* PickNearestPoint(Double_t x, Double_t y);// Used in track building.
  LxPoint* PickNearestPoint(LxRay* ray);// Used in track building.
  LxPoint* PickNearestPoint(Double_t x, Double_t y, Double_t deltaX, Double_t deltaY);// Used in middle point building.
  bool HasPointInRange(Double_t x, Double_t y, Double_t deltaX, Double_t deltaY);
};

struct LxStation
{
  std::vector<LxLayer*> layers;
  LxSpace* space;
  int stationNumber;
  Double_t zCoord;
  Double_t txLimit;
  Double_t tyLimit;
  Double_t txBreakLimit;
  Double_t tyBreakLimit;
  Double_t txBreakSigma;
  Double_t tyBreakSigma;
  Double_t disp01X;// 'disp' -- means: dispersion.
  Double_t disp01Y;
  Double_t disp02X;
  Double_t disp02Y;
  LxStation(LxSpace* sp, int stNum);
  void Clear();

  LxPoint* AddPoint(int layerNumber, int hitId, Double_t x, Double_t y, Double_t z, Double_t dx, Double_t dy, Double_t dz)
  {
    return layers[layerNumber]->AddPoint(hitId, x, y, z, dx, dy, dz);
  }

  void RestoreMiddlePoints();
  void BuildRays();
  void ConnectNeighbours();
};

struct LxExtTrack
{
  CbmStsTrack* track;
  Int_t extId;
  LxMCTrack* mcTrack;

  LxExtTrack() : track(0), extId(-1), mcTrack(0) {}
};

struct LxTrack
{
  LxExtTrack externalTrack;
  bool matched;
  LxMCTrack* mcTrack;
#ifdef CALC_LINK_WITH_STS_EFF
  std::list<LxMCTrack*> mcTracks;
#endif//CALC_LINK_WITH_STS_EFF
  int length;
  LxRay* rays[LXSTATIONS - LXFIRSTSTATION - 1];// Rays are stored left to right.
  LxPoint* points[(LXSTATIONS - LXFIRSTSTATION) * LXLAYERS];
  Double_t chi2;
  Double_t aX;
  Double_t bX;
  Double_t aY;
  Double_t bY;
  int restoredPoints;

  explicit LxTrack(LxTrackCandidate* tc);
  void Fit();
};

struct LxSpace
{
  std::vector<LxStation*> stations;
  std::list<LxTrack*> tracks;
  std::list<LxExtTrack> extTracks;

  LxSpace();
  void Clear();

  LxPoint* AddPoint(int stationNumber, int layerNumber, int hitId, Double_t x, Double_t y, Double_t z, Double_t dx, Double_t dy, Double_t dz)
  {
    return stations[stationNumber]->AddPoint(layerNumber, hitId, x, y, z, dx, dy, dz);
  }

  void RestoreMiddlePoints();
  void BuildRays();
  void ConnectNeighbours();
  void BuildCandidates(LxRay* ray, LxRay** rays, std::list<LxTrackCandidate*>& candidates, Double_t chi2);
  void Reconstruct();
  void FitTracks();
  void JoinExtTracks();
  void CheckArray(Double_t xs[LXSTATIONS][LXLAYERS], Double_t ys[LXSTATIONS][LXLAYERS],
      Double_t zs[LXSTATIONS][LXLAYERS],
      Double_t xDisp2Limits[LXSTATIONS], Double_t yDisp2Limits[LXSTATIONS], Double_t tx2Limits[LXSTATIONS],
        Double_t ty2Limits[LXSTATIONS], Double_t txBreak2Limits[LXSTATIONS], Double_t tyBreak2Limits[LXSTATIONS]);
  void CheckArray(Double_t xs[LXSTATIONS][LXLAYERS], Double_t ys[LXSTATIONS][LXLAYERS],
      Double_t zs[LXSTATIONS][LXLAYERS], std::list<LxPoint*> pts[LXSTATIONS][LXLAYERS], int level,
      Double_t xDisp2Limits[LXSTATIONS], Double_t yDisp2Limits[LXSTATIONS], Double_t tx2Limits[LXSTATIONS],
        Double_t ty2Limits[LXSTATIONS], Double_t txBreak2Limits[LXSTATIONS], Double_t tyBreak2Limits[LXSTATIONS]);
  void CheckArray(std::ostream& out, LxMCTrack& track);
};

#endif//LXCA_INCLUDED
