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
 *  L1 branch class
 *
 *====================================================================
 */

#ifndef L1Branch_H
#define L1Branch_H

#include <vector>

class L1Triplet;
class L1StsHit;

struct L1Branch
{
  unsigned short int Quality;
  float Momentum, chi2;
  std::vector<unsigned /*short*/ int > StsHits;

  void Set( unsigned char iStation, unsigned char Length, float Chi2, float Qp ){
    
    unsigned short int length = Length;
    unsigned short int ista = 16-iStation;
    float tmp = sqrt(Chi2)/3.5*255;
    if( tmp>255 ) tmp = 255;
    unsigned short int chi_2 = 255 - (unsigned char ) tmp ;
    Quality = (length<<12) + (ista<<8) + chi_2;
    Momentum = 1.0/fabs(Qp);
    chi2 = chi_2;
  }

  static bool compareChi2(const L1Branch &a, const L1Branch &b){
    return (a.Quality > b.Quality );
  }
  static bool comparePChi2(const L1Branch *a, const L1Branch *b){
    return compareChi2(*a,*b);
  }
  static bool compareMomentum(const L1Branch &a, const L1Branch &b){
    return (a.Momentum > b.Momentum );
  }
  static bool comparePMomentum(const L1Branch *a, const L1Branch *b){
    return compareMomentum(*a,*b);
  }
};

#endif
