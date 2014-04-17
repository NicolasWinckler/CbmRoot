#include "LxCA.h"
#include <iostream>
#include "Lx.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Global declarations
///////////////////////////////////////////////////////////////////////////////////////////////////

static Double_t errorXcoeff = 8;
static Double_t errorYcoeff = 8;
static Double_t errorTxCoeff = 4;
static Double_t errorTyCoeff = 4;

// These ..Ext.. coefficients are applied to squares of uncertainties. So they can be bigger than the previous.
//static Double_t errorExtXcoeff = 16;
//static Double_t errorExtYcoeff = 16;
//static Double_t errorExtTxCoeff = 16;
//static Double_t errorExtTyCoeff = 16;

//static LxFinder* finderInstance = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LxTrackCandidate
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct LxTrackCandidate
{
  LxRay* rays[LXSTATIONS - LXFIRSTSTATION - 1];// Rays are stored left to right.
  Int_t length;
  Double_t chi2;

  LxTrackCandidate(LxRay** rr, Int_t len, Double_t c2) : length(len), chi2(c2)
  {
    for (int i = 0; i < len; ++i)
      rays[i] = rr[i];

    for (int i = len; i < LXSTATIONS - LXFIRSTSTATION - 1; ++i)
      rays[i] = 0;
  }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LxTrack
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LxTrack::LxTrack(LxTrackCandidate* tc) : matched(false), length(tc->length), chi2(tc->chi2), mcTrack(0),
    aX(0), bX(0), aY(0), bY(0), restoredPoints(0)
{
  memset(rays, 0, sizeof(rays));
  memset(points, 0, sizeof(points));

  for (int i = 0; i < length; ++i)
  {
    LxRay* ray = tc->rays[i];
    rays[i] = ray;
    LxPoint* point = ray->source;
    point->used = true;
    point->track = this;
    points[(i + 1) * LXLAYERS + LXMIDDLE] = point;

    if (point->artificial)
      ++restoredPoints;

    LxStation* station = ray->station;

    for (int j = 0; j < LXLAYERS; ++j)
    {
      if (LXMIDDLE == j)
        continue;

      LxLayer* layer = station->layers[j];
      LxPoint* p = layer->PickNearestPoint(ray);

      if (0 != p && !p->used)
      {
        p->used = true;
        p->track = this;
        points[(i + 1) * LXLAYERS + j] = p;
      }
    }
  }

  LxRay* ray = tc->rays[0];
  LxPoint* point = ray->end;
  point->used = true;
  point->track = this;
  points[LXMIDDLE] = point;

  if (point->artificial)
    ++restoredPoints;

  LxStation* station = ray->station;

  for (int i = 0; i < LXLAYERS; ++i)
  {
    if (LXMIDDLE == i)
      continue;

    LxLayer* layer = station->layers[i];
    LxPoint* p = layer->PickNearestPoint(ray);

    if (0 != p && !p->used)
    {
      p->used = true;
      p->track = this;
      points[i] = p;
    }
  }

  //cout << "This track contains: " << restoredPoints << " restored points" << endl;
}

static inline void Ask()
{
  char symbol;

  cout << "ask>";

  do
  {
    cin.get(symbol);
  } while (symbol != '\n');

  cout << endl;
}

void LxTrack::Fit()
{
  Double_t sumZ = 0;
  Double_t sumZ2 = 0;
  Double_t sumX = 0;
  Double_t sumZX = 0;
  Double_t sumY = 0;
  Double_t sumZY = 0;

  for (int i = 0; i < LXSTATIONS - LXFIRSTSTATION; ++i)
  {
    LxPoint* point = points[LXLAYERS * i + LXMIDDLE];
    Double_t z = point->z;
    Double_t x = point->x;
    Double_t y = point->y;

    sumZ += z;
    sumZ2 += z * z;
    sumX += x;
    sumZX += z * x;
    sumY += y;
    sumZY += z * y;
  }

  aX = (LXSTATIONS * sumZX - sumZ * sumX) / (LXSTATIONS * sumZ2 - sumZ * sumZ);
  bX = (sumX - aX * sumZ) / LXSTATIONS;
  aY = (LXSTATIONS * sumZY - sumZ * sumY) / (LXSTATIONS * sumZ2 - sumZ * sumZ);
  bY = (sumY - aY * sumZ) / LXSTATIONS;

  //LxPoint* point = points[LXLAYERS * (LXSTATIONS - 1) + LXMIDDLE];
  //cout << "bX = " << bX << " , aX = " << aX << " , X / Z = " << point->x / point->z << endl;
  //cout << "bY = " << bY << " , aY = " << aY << " , Y / Z = " << point->y / point->z << endl;
  //Ask();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Next (second) version of implementation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LxPoint
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LxPoint::~LxPoint()
{
  for (list<LxRay*>::iterator i = rays.begin(); i != rays.end(); ++i)
    delete *i;
}

void LxPoint::CreateRay(LxPoint* lPoint)
{
  rays.push_back(new LxRay(this, lPoint));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LxRay
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LxRay::LxRay(LxPoint* s, LxPoint* e) :
      source(s), end(e), tx((e->x - s->x) / (e->z - s->z)), ty((e->y - s->y) / (e->z - s->z)),
      dtx(sqrt(e->dx * e->dx + s->dx * s->dx) / (s->z - e->z)),
      dty(sqrt(e->dy * e->dy + s->dy * s->dy) / (s->z - e->z)),
      station(s->layer->station)
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LxLayer
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LxLayer::LxLayer(LxStation* st, int lNum) : station(st),
    layerNumber(lNum), zCoord(0)
{
}

LxLayer::~LxLayer()
{
  Clear();
}

void LxLayer::Clear()
{
  for (list<LxPoint*>::iterator i = points.begin(); i != points.end(); ++i)
    delete *i;

  points.clear();
}

LxPoint* LxLayer::PickNearestPoint(Double_t x, Double_t y)
{
  LxPoint* result = 0;
  Double_t minR2 = 0;

  for (list<LxPoint*>::iterator i = points.begin(); i != points.end(); ++i)
  {
    LxPoint* point = *i;

    if (point->used)
      continue;

    Double_t diffX = point->x - x;
    Double_t diffY = point->y - y;

    if (diffX < 0)
      diffX = -diffX;

    if (diffX > errorXcoeff * point->dx)
      continue;

    if (diffY < 0)
      diffY = -diffY;

    if (diffY > errorYcoeff * point->dy)
      continue;

    Double_t r2 = diffX * diffX + diffY * diffY;

    if (0 == result || r2 < minR2)
    {
      result = point;
      minR2 = r2;
    }
  }

  return result;
}

LxPoint* LxLayer::PickNearestPoint(LxRay* ray)
{
  LxPoint* point = ray->source;
  Double_t diffZ = zCoord - point->z;
  Double_t x = point->x + ray->tx * diffZ;
  Double_t y = point->y + ray->ty * diffZ;
  return PickNearestPoint(x, y);
}

LxPoint* LxLayer::PickNearestPoint(Double_t x, Double_t y, Double_t deltaX, Double_t deltaY)
{
  LxPoint* result = 0;
  Double_t minR2 = 0;
  Double_t xLBound = x - deltaX;
  Double_t xUBound = x + deltaX;
  Double_t yLBound = y - deltaY;
  Double_t yUBound = y + deltaY;

  for (list<LxPoint*>::iterator i = points.begin(); i != points.end(); ++i)
  {
    LxPoint* point = *i;

    if (point->x < xLBound || point->x > xUBound || point->y < yLBound || point->y > yUBound)
      continue;

    Double_t diffX = point->x - x;
    Double_t diffY = point->y - y;
    Double_t r2 = diffX * diffX + diffY * diffY;

    if (0 == result || r2 < minR2)
    {
      result = point;
      minR2 = r2;
    }
  }

  return result;
}

bool LxLayer::HasPointInRange(Double_t x, Double_t y, Double_t deltaX, Double_t deltaY)
{
  Double_t xLBound = x - deltaX;
  Double_t xUBound = x + deltaX;
  Double_t yLBound = y - deltaY;
  Double_t yUBound = y + deltaY;

  for (list<LxPoint*>::iterator i = points.begin(); i != points.end(); ++i)
  {
    LxPoint* point = *i;

    if (xLBound < point->x && point->x < xUBound && yLBound < point->y && point->y < yUBound)
      return true;
  }

  return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LxStation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// These arrays are filled with data obtained during Monte-Carlo simulations.
static Double_t txLimits[] = { 0.120, 0.120, 0.120, 0.120, 0.135, 0.160 };
static Double_t tyLimits[] = { 0.056, 0.067, 0.072, 0.080, 0.110, 0.150 };

static Double_t txBreakLimits[] = { 0.065, 0.090, 0.095, 0.11, 0.15, 0.15 };
static Double_t tyBreakLimits[] = { 0.058, 0.077, 0.083, 0.09, 0.13, 0.14 };

static Double_t txBreakSigmas[] = { 0.013, 0.013, 0.013, 0.016, 0.028, 0.05 };
static Double_t tyBreakSigmas[] = { 0.011, 0.011, 0.014, 0.017, 0.027, 0.05 };

static Double_t dispersions01X[] = { 2.3, 2.6, 3.0, 3.4, 4.2, 5.5 };
static Double_t dispersions01Y[] = { 2.1, 2.5, 2.6, 2.8, 3.7, 5.0 };

static Double_t dispersions02X[] = { 3.0, 3.0, 3.0, 2.5, 2.5, 3.0 };
static Double_t dispersions02Y[] = { 2.0, 2.0, 1.5, 2.0, 2.0, 3.0 };

LxStation::LxStation(LxSpace* sp, int stNum) : space(sp), stationNumber(stNum), zCoord(0), txLimit(txLimits[stNum]),
    tyLimit(tyLimits[stNum]), txBreakLimit(txBreakLimits[stNum]), tyBreakLimit(tyBreakLimits[stNum]),
    txBreakSigma(txBreakSigmas[stNum]), tyBreakSigma(tyBreakSigmas[stNum]), disp01X(dispersions01X[stNum]),
    disp01Y(dispersions01Y[stNum]), disp02X(dispersions02X[stNum]), disp02Y(dispersions02Y[stNum])
{
  for (int i = 0; i < LXLAYERS; ++i)
    layers.push_back(new LxLayer(this, i));
}

void LxStation::Clear()
{
  for (vector<LxLayer*>::iterator i = layers.begin(); i != layers.end(); ++i)
    (*i)->Clear();
}

void LxStation::RestoreMiddlePoints()
{
  LxLayer* lLayer = layers[0];
  LxLayer* mLayer = layers[LXMIDDLE];
  LxLayer* rLayer = layers[2];
  list<LxPoint*>& mPoints = mLayer->points;

  // 1. Loop through the points of the middle layer and check if they have at least on corresponding point on either left or right layer.
  for (list<LxPoint*>::iterator i = mPoints.begin(); i != mPoints.end(); ++i)
  {
    LxPoint* point = *i;
    Double_t tx = point->x / point->z;
    Double_t ty = point->y / point->z;
    Double_t diffZ = lLayer->zCoord - point->z;
    Double_t x = point->x + tx * diffZ;
    Double_t y = point->y + ty * diffZ;

    if (!lLayer->HasPointInRange(x, y, disp01X, disp01Y))
    {
      diffZ = rLayer->zCoord - point->z;
      x = point->x + tx * diffZ;
      y = point->y + tx * diffZ;

      if (!rLayer->HasPointInRange(x, y, disp01X, disp01Y))
        point->valid = false;
    }
  }

  // 2. Loop through the points of the left station. If there are corresponding points in the middle station -- it is OK.
  //    If there are no -- check corresponding points on the right station. If such points exist, choose the point nearest
  //    to the predicted value among them and reconstruct a middle point as a point of intersection of the segment between
  //    the left and corresponding right points and the middle layer.
  for (list<LxPoint*>::iterator i = mPoints.begin(); i != mPoints.end(); ++i)
  {
    LxPoint* lPoint = *i;
    Double_t tx = lPoint->x / lPoint->z;
    Double_t ty = lPoint->y / lPoint->z;
    Double_t diffZ = mLayer->zCoord - lPoint->z;
    Double_t x = lPoint->x + tx * diffZ;
    Double_t y = lPoint->y + ty * diffZ;

    if (mLayer->HasPointInRange(x, y, disp01X, disp01Y))
      continue;

    diffZ = rLayer->zCoord - lPoint->z;
    x = lPoint->x + tx * diffZ;
    y = lPoint->y + ty * diffZ;

    LxPoint* rPoint = rLayer->PickNearestPoint(x, y, disp02X, disp02Y);

    if (0 == rPoint)
      continue;

    x = (lPoint->x + rPoint->x) / 2;
    y = (lPoint->y + rPoint->y) / 2;
    mLayer->AddPoint(-1, x, y, mLayer->zCoord, lPoint->dx, lPoint->dy, lPoint->dz, true);
  }
}

void LxStation::BuildRays()
{
  if (stationNumber < LXFIRSTSTATION + 1)
    return;

  LxLayer* rLayer = layers[LXMIDDLE];
  LxStation* lStation = space->stations[stationNumber - 1];// Pointer to 'left' station. 'This' station is 'right'.
  LxLayer* lLayer = lStation->layers[LXMIDDLE];

  for (list<LxPoint*>::iterator rIter = rLayer->points.begin(); rIter != rLayer->points.end(); ++rIter)
  {
    LxPoint* rPoint = *rIter;

    if (!rPoint->valid)
      continue;

    Double_t tx1 = rPoint->x / rPoint->z;
    Double_t ty1 = rPoint->y / rPoint->z;

    for (list<LxPoint*>::iterator lIter = lLayer->points.begin(); lIter != lLayer->points.end(); ++lIter)
    {
      LxPoint* lPoint = *lIter;

      if (!lPoint->valid)
        continue;

      Double_t diffZ = lPoint->z - rPoint->z;
      Double_t tx = (lPoint->x - rPoint->x) / diffZ;
      Double_t ty = (lPoint->y - rPoint->y) / diffZ;
      Double_t diffTx = tx - tx1;
      Double_t diffTy = ty - ty1;

      if (diffTx < 0)
        diffTx = -diffTx;

      if (diffTy < 0)
        diffTy = -diffTy;

      if (diffTx > txLimit || diffTy > tyLimit)
        continue;

      rPoint->CreateRay(lPoint);
    }
  }
}

void LxStation::ConnectNeighbours()
{
  if (stationNumber < LXFIRSTSTATION + 2)
    return;

  LxLayer* layer = layers[LXMIDDLE];
  LxStation* lStation = space->stations[stationNumber - 1];

  for (list<LxPoint*>::iterator i = layer->points.begin(); i != layer->points.end(); ++i)
  {
    LxPoint* rPoint = *i;

    for (list<LxRay*>::iterator j = rPoint->rays.begin(); j != rPoint->rays.end(); ++j)
    {
      LxRay* rRay = *j;
      LxPoint* ePoint = rRay->end;

      for (list<LxRay*>::iterator k = ePoint->rays.begin(); k != ePoint->rays.end(); ++k)
      {
        LxRay* lRay = *k;
        Double_t diffTx = lRay->tx - rRay->tx;
        Double_t diffTy = lRay->ty - rRay->ty;

        if (diffTx < 0)
          diffTx = -diffTx;

        if (diffTy < 0)
          diffTy = -diffTy;

        if (diffTx > lStation->txBreakLimit || diffTy > lStation->tyBreakLimit)
          continue;

        rRay->neighbours.push_back(lRay);
      }
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LxSpace
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LxSpace::LxSpace()
{
  for (int i = 0; i < LXSTATIONS; ++i)
    stations.push_back(new LxStation(this, i));
}

void LxSpace::Clear()
{
  for (vector<LxStation*>::iterator i = stations.begin(); i != stations.end(); ++i)
    (*i)->Clear();

  for (list<LxTrack*>::iterator i = tracks.begin(); i != tracks.end(); ++i)
    delete *i;

  tracks.clear();
  extTracks.clear();
}

void LxSpace::RestoreMiddlePoints()
{
  for (int i = LXFIRSTSTATION; i < LXSTATIONS; ++i)
    stations[i]->RestoreMiddlePoints();
}

void LxSpace::BuildRays()
{
  for (int i = LXFIRSTSTATION + 1; i < LXSTATIONS; ++i)
    stations[i]->BuildRays();
}

void LxSpace::ConnectNeighbours()
{
  for (int i = LXFIRSTSTATION + 2; i < LXSTATIONS; ++i)
    stations[i]->ConnectNeighbours();
}

void LxSpace::BuildCandidates(LxRay* ray, LxRay** rays, list<LxTrackCandidate*>& candidates, Double_t chi2)
{
  LxStation* station = ray->station;
  int level = station->stationNumber - LXFIRSTSTATION - 1;
  rays[level] = ray;

  if (0 == level)
  {
    LxTrackCandidate* tc = new LxTrackCandidate(rays, LXSTATIONS - LXFIRSTSTATION - 1, chi2);
    candidates.push_back(tc);
    return;
  }

  for(list<LxRay*>::iterator i = ray->neighbours.begin(); i != ray->neighbours.end(); ++i)
  {
    LxRay* lRay = *i;

    if (lRay->source->used)
      continue;

    Double_t dtx = ray->tx - lRay->tx;
    dtx /= station->txBreakSigma;
    Double_t dty = ray->ty - lRay->ty;
    dty /= station->tyBreakSigma;

    // Continue process of track building.
    BuildCandidates(lRay, rays, candidates, chi2 + dtx * dtx + dty * dty);
  }
}

void LxSpace::Reconstruct()
{
  LxStation* startStation = stations[LXSTATIONS - 1];
  LxLayer* startLayer = startStation->layers[LXMIDDLE];
  list<LxPoint*>& startPoints = startLayer->points;

  for (list<LxPoint*>::iterator i = startPoints.begin(); i != startPoints.end(); ++i)
  {
    LxPoint* point = *i;

    if (point->used)
      continue;

    LxRay* rays[LXSTATIONS - LXFIRSTSTATION - 1];
    list<LxTrackCandidate*> candidates;
    Double_t chi2 = 0;
    list<LxRay*>& startRays = point->rays;

    for (list<LxRay*>::iterator j = startRays.begin(); j != startRays.end(); ++j)
    {
      LxRay* ray = *j;
      BuildCandidates(ray, rays, candidates, chi2);
    }

    LxTrackCandidate* bestCandidate = 0;

    for (list<LxTrackCandidate*>::iterator j = candidates.begin(); j != candidates.end(); ++j)
    {
      LxTrackCandidate* candidate = *j;

      if (0 == bestCandidate || candidate->chi2 < bestCandidate->chi2)
        bestCandidate = candidate;
    }

    if (0 != bestCandidate)
    {
      LxTrack* track = new LxTrack(bestCandidate);
      tracks.push_back(track);
    }

    for (list<LxTrackCandidate*>::iterator j = candidates.begin(); j != candidates.end(); ++j)
      delete *j;
  }
}

void LxSpace::FitTracks()
{
  for (list<LxTrack*>::iterator i = tracks.begin(); i != tracks.end(); ++i)
    (*i)->Fit();
}

void LxSpace::JoinExtTracks()
{
  Double_t sigmaX = 1.318;
  Double_t sigmaX2 = sigmaX * sigmaX;
  Double_t sigmaY = 1.148;
  Double_t sigmaY2 = sigmaY * sigmaY;
  Double_t sigmaTx = 0.02617;
  Double_t sigmaTx2 = sigmaTx * sigmaTx;
  Double_t sigmaTy = 0.01531;
  Double_t sigmaTy2 = sigmaTy * sigmaTy;

  for (list<LxTrack*>::iterator i = tracks.begin(); i != tracks.end(); ++i)
  {
    LxTrack* track = *i;
    LxRay* ray = track->rays[0];
    LxPoint* point = ray->end;

    LxExtTrack bestCandidate;
    Double_t minChi2 = 0;

    // External track are already filtered by P and Pt.
    for (list<LxExtTrack>::iterator j = extTracks.begin(); j != extTracks.end(); ++j)
    {
      LxExtTrack& extTrack = *j;
      const FairTrackParam* param = extTrack.track->GetParamLast();
      Double_t deltaZ = param->GetZ() - point->z;
      Double_t deltaX = param->GetX() - point->x - ray->tx * deltaZ;
      Double_t deltaY = param->GetY() - point->y - ray->ty * deltaZ;
      Double_t deltaTx = param->GetTx() - ray->tx;
      Double_t deltaTy = param->GetTy() - ray->ty;
      Double_t chi2 = deltaX * deltaX / sigmaX2 + deltaY * deltaY / sigmaY2 +
          deltaTx * deltaTx / sigmaTx2 + deltaTy * deltaTy / sigmaTy2;

      if (0 == bestCandidate.track || chi2 < minChi2)
      {
        bestCandidate = extTrack;
        minChi2 = chi2;
      }
    }

    track->externalTrack = bestCandidate;
  }
}

void LxSpace::CheckArray(Double_t xs[LXSTATIONS][LXLAYERS], Double_t ys[LXSTATIONS][LXLAYERS],
    Double_t zs[LXSTATIONS][LXLAYERS],
    Double_t xDisp2Limits[LXSTATIONS], Double_t yDisp2Limits[LXSTATIONS], Double_t tx2Limits[LXSTATIONS],
      Double_t ty2Limits[LXSTATIONS], Double_t txBreak2Limits[LXSTATIONS], Double_t tyBreak2Limits[LXSTATIONS])
{
  for (int i = 0; i < LXSTATIONS; ++i)
  {
    Double_t diffZ = zs[i][0] - zs[i][1];
    Double_t tx = xs[i][1] / zs[i][1];
    Double_t ty = ys[i][1] / zs[i][1];
    Double_t dispXL = xs[i][0] - tx * diffZ - xs[i][1];
    Double_t dispYL = ys[i][0] - ty * diffZ - ys[i][1];

    if (dispXL < 0)
      dispXL = -dispXL;

    if (dispYL < 0)
      dispYL = -dispYL;

    diffZ = zs[i][2] - zs[i][1];
    Double_t dispXR = xs[i][2] - tx * diffZ - xs[i][1];
    Double_t dispYR = ys[i][2] - ty * diffZ - ys[i][1];

    if (dispXR < 0)
      dispXR = -dispXR;

    if (dispYR < 0)
      dispYR = -dispYR;

    Double_t dispX = dispXL < dispXR ? dispXL : dispXR;
    Double_t dispY = dispYL < dispYR ? dispYL : dispYR;

    if (stations[i]->disp01X - dispX < xDisp2Limits[i])
      xDisp2Limits[i] = stations[i]->disp01X - dispX;

    if (stations[i]->disp01Y - dispY < yDisp2Limits[i])
      yDisp2Limits[i] = stations[i]->disp01Y - dispY;

    if (i > 0)
    {
      Double_t diffZ = zs[i][1] - zs[i - 1][1];
      Double_t tx1 = xs[i][1] / zs[i][1];
      Double_t tx = (xs[i][1] - xs[i - 1][1]) / diffZ;
      Double_t dtx = tx - tx1;

      if (dtx < 0)
        dtx = -dtx;

      Double_t ty1 = ys[i][1] / zs[i][1];
      Double_t ty = (ys[i][1] - ys[i - 1][1]) / diffZ;
      Double_t dty = ty - ty1;

      if (dty < 0)
        dty = -dty;

      if (stations[i]->txLimit - dtx < tx2Limits[i])
        tx2Limits[i] = stations[i]->txLimit - dtx;

      if (stations[i]->tyLimit - dty < ty2Limits[i])
        ty2Limits[i] = stations[i]->tyLimit - dty;

      if (i < LXSTATIONS - 1)
      {
        diffZ = zs[i + 1][1] - zs[i][1];
        Double_t tx2 = (xs[i + 1][1] - xs[i][1]) / diffZ;
        Double_t ty2 = (ys[i + 1][1] - ys[i][1]) / diffZ;
        Double_t txBreak = tx2 - tx;
        Double_t tyBreak = ty2 - ty;

        if (txBreak < 0)
          txBreak = -txBreak;

        if (tyBreak < 0)
          tyBreak = -tyBreak;

        if (stations[i]->txBreakLimit - txBreak < txBreak2Limits[i])
          txBreak2Limits[i] = stations[i]->txBreakLimit - txBreak;

        if (stations[i]->tyBreakLimit - tyBreak < tyBreak2Limits[i])
          tyBreak2Limits[i] = stations[i]->tyBreakLimit - tyBreak;
      }
    }
  }
}

void LxSpace::CheckArray(Double_t xs[LXSTATIONS][LXLAYERS], Double_t ys[LXSTATIONS][LXLAYERS],
    Double_t zs[LXSTATIONS][LXLAYERS], list<LxPoint*> pts[LXSTATIONS][LXLAYERS], int level,
    Double_t xDisp2Limits[LXSTATIONS], Double_t yDisp2Limits[LXSTATIONS], Double_t tx2Limits[LXSTATIONS],
      Double_t ty2Limits[LXSTATIONS], Double_t txBreak2Limits[LXSTATIONS], Double_t tyBreak2Limits[LXSTATIONS])
{
  if (LXSTATIONS * LXLAYERS == level)
  {
    CheckArray(xs, ys, zs, xDisp2Limits, yDisp2Limits, tx2Limits, ty2Limits, txBreak2Limits, tyBreak2Limits);
    return;
  }

  int stNum = level / LXLAYERS;
  int lyNum = level % LXLAYERS;
  list<LxPoint*>& points = pts[stNum][lyNum];

  for (list<LxPoint*>::iterator i = points.begin(); i != points.end(); ++i)
  {
    LxPoint* point = *i;

    xs[stNum][lyNum] = point->x;
    ys[stNum][lyNum] = point->y;
    zs[stNum][lyNum] = point->z;

    CheckArray(xs, ys, zs, pts, level + 1, xDisp2Limits, yDisp2Limits, tx2Limits, ty2Limits, txBreak2Limits,
        tyBreak2Limits);
  }
}

void LxSpace::CheckArray(ostream& out, LxMCTrack& track)
{
  Double_t xs[LXSTATIONS][LXLAYERS];
  Double_t ys[LXSTATIONS][LXLAYERS];
  Double_t zs[LXSTATIONS][LXLAYERS];
  list<LxPoint*> pts[LXSTATIONS][LXLAYERS];
  int inits[LXSTATIONS][LXLAYERS];
  Double_t xDisp2Limits[LXSTATIONS];
  Double_t yDisp2Limits[LXSTATIONS];
  Double_t tx2Limits[LXSTATIONS];
  Double_t ty2Limits[LXSTATIONS];
  Double_t txBreak2Limits[LXSTATIONS];
  Double_t tyBreak2Limits[LXSTATIONS];
  bool busyHits[LXSTATIONS];

  for (int i = 0; i < LXSTATIONS; ++i)
  {
    for (int j = 0; j < LXLAYERS; ++j)
      inits[i][j] = 0;

    xDisp2Limits[i] = stations[i]->disp01X;
    yDisp2Limits[i] = stations[i]->disp01Y;
    tx2Limits[i] = stations[i]->txLimit;
    ty2Limits[i] = stations[i]->tyLimit;
    txBreak2Limits[i] = stations[i]->txBreakLimit;
    tyBreak2Limits[i] = stations[i]->tyBreakLimit;
    busyHits[i] = false;
  }

  for (vector<LxMCPoint*>::iterator i = track.Points.begin(); i != track.Points.end(); ++i)
  {
    LxMCPoint* point = *i;

    for (list<LxPoint*>::iterator j = point->lxPoints.begin(); j != point->lxPoints.end(); ++j)
    {
      LxPoint* lxPoint = *j;
      pts[point->stationNumber][point->layerNumber].push_back(lxPoint);

      if (lxPoint->used)
        busyHits[point->stationNumber] = true;
    }

    inits[point->stationNumber][point->layerNumber] += point->lxPoints.size();
  }

  bool ini = true;

  for (int i = 0; i < LXSTATIONS; ++i)
  {
    for (int j = 0; j < LXLAYERS; ++j)
      ini = ini && 0 < inits[i][j];
  }

  if (!ini)
  {
    out << "CheckArray: track does not contain all the points" << endl;
    return;
  }

  CheckArray(xs, ys, zs, pts, 0, xDisp2Limits, yDisp2Limits, tx2Limits, ty2Limits, txBreak2Limits,
      tyBreak2Limits);

  for (int i = 0; i < LXSTATIONS; ++i)
  {
    out << "CheckArray on station " << i << ": ";
    out << "dispX to limit: " << xDisp2Limits[i];
    out << "; dispY to limit: " << yDisp2Limits[i];

    if (i > 0)
    {
      out << "; Tx to limit: " << tx2Limits[i];
      out << "; Ty to limit: " << ty2Limits[i];

      if (i < LXSTATIONS - 1)
      {
        out << "; tx break to limit: " << txBreak2Limits[i];
        out << "; ty break to limit: " << tyBreak2Limits[i];
      }
    }

    if (busyHits[i])
      out << "; have busy hits";

    out << endl;
  }

  out << endl;
}
