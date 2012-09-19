#ifndef _L1HitsL1HitsSortHelper_h_
#define _L1HitsL1HitsSortHelper_h_

#include <vector>
#include <algorithm>
#include "L1StsHit.h"
#include "L1HitPoint.h"

using std::vector;

struct L1HitsSortHelperData{
  L1StsHit* h;
  L1HitPoint* p;
  THitI i;

  static bool compare(const L1HitsSortHelperData &a, const L1HitsSortHelperData &b){
    return ( a.p->y/a.p->z < b.p->y/b.p->z );
    // return ( a.p->y < b.p->y );
  }
};

class L1HitsSortHelper{
 public:
  L1HitsSortHelper( vector<L1StsHit> &hits, vector<L1HitPoint> &points, vector<THitI> &indices, THitI* iStart, THitI* iStop, int nStations );

  void Sort();
  
 private:
  vector<L1HitsSortHelperData> fD;
  vector<L1StsHit> &fHits;
  vector<L1HitPoint> &fPoints;
  vector<THitI> &fIndices;
  THitI *fStsHitsUnusedStartIndex, *fStsHitsUnusedStopIndex;
  int fNStations;
};
    
#endif
