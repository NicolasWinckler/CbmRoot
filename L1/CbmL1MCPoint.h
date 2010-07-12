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

#ifndef CbmL1MCPoint_H
#define CbmL1MCPoint_H

#include <vector>
using std::vector;

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

  vector<int> hitIds; // indices of CbmL1StsHits in L1->vStsHits array
};

#endif
