/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  L1 Monte Carlo information
 *
 *====================================================================
 */

#ifndef CbmL1MC_H
#define CbmL1MC_H

#include <vector>

class L1StsHit;

struct CbmL1MCPoint
{ 
  double x, y, z, px, py, pz;
  double p, q, mass;
  int pdg, ID, mother_ID;
  int iStation;

  static bool compareIDz( const CbmL1MCPoint &a, const CbmL1MCPoint &b )
  {
    return ( a.ID < b.ID ) || ( ( a.ID == b.ID ) && (a.z < b.z) );
  }

  static bool pcompareIDz( const CbmL1MCPoint *a, const CbmL1MCPoint *b )
  {
    return ( a->ID < b->ID ) || ( ( a->ID == b->ID ) && (a->z < b->z) );
  }
};

struct CbmL1MCTrack
{
  double mass, q, p, x, y, z, px, py, pz;
  int ID, mother_ID, pdg;
  std::vector<CbmL1MCPoint*> Points;
  std::vector<int> StsHits;
  int nMCContStations;
  int nHitContStations;
};


#endif
