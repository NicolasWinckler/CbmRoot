
#include "L1HitsSortHelper.h"

#include <vector>
#include <algorithm>
#include "L1StsHit.h"
#include "L1HitPoint.h"
#include "L1Algo.h"

using std::vector;

L1HitsSortHelper::L1HitsSortHelper( vector<L1StsHit> &hits, vector<L1HitPoint> &points, vector<THitI> &indices, THitI* iStart, THitI* iStop ): fHits(hits), fPoints(points), fIndices(indices), fStsHitsUnusedStartIndex( iStart ), fStsHitsUnusedStopIndex( iStop ) {
  L1_ASSERT( hits.size() ==  points.size(), hits.size() << " " << points.size() );
  const int NHits = hits.size();
  fD.resize( NHits );
  for ( int i = 0; i < NHits; i++ ) {
    fD[i].h = &(hits[i]);
    fD[i].p = &(points[i]);
    fD[i].i = &(indices[i]);
  }
}

void L1HitsSortHelper::Sort(){
  for ( int iS = 0; iS < L1Algo::MaxNStations; ++iS ) { // TODO
    std::sort(fD.begin()+fStsHitsUnusedStartIndex[iS], fD.begin()+fStsHitsUnusedStopIndex[iS], L1HitsSortHelperData::compare);
  }
    
    
  vector<L1StsHit> hits = fHits;
  vector<L1HitPoint> points = fPoints;
  vector<THitI> indices = fIndices;
  const int NHits = fD.size();
  for ( int i = 0; i < NHits; i++ ) {
    hits[i] = *fD[i].h;
    points[i] = *fD[i].p;
    indices[i] = *fD[i].i;
  }
  fHits = hits;
  fPoints = points;
  fIndices = indices;
}

