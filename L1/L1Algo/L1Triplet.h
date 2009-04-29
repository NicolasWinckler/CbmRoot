#ifndef L1Triplet_H
#define L1Triplet_H

class L1Triplet
{

 private:

  unsigned short int w0; // left hit (16b)
  unsigned short int w1; // first neighbour(16b) or middle hit(16b)
  unsigned short int w2; // N neighbours(16b) or right hit(16b)
  unsigned char      b0; // chi2(5b) + level(3b)
  unsigned char      bl; // level
  unsigned char      b1; // qp (8b)

 public:

  unsigned char Cqp;
  
  void Set( unsigned int iHitL, unsigned int iHitM, unsigned int iHitR , 
	    unsigned int iFirstNeighbour, unsigned int NNeighbours, 
	    unsigned char Level, unsigned char  Qp, 
	    float Chi2  
	    ){ 
    w0 = iHitL;
    if( Level==0 ){
      w1 = iHitM;
      w2 = iHitR;
    }else{
      w1 = iFirstNeighbour;
      w2 = NNeighbours;
    }
    Chi2 = sqrt(fabs(Chi2))*31./3.5 ; // 3.5 == 31
    if( Chi2>31 || !finite(Chi2) ) Chi2 = 31;
    b0 = ( ((unsigned char) Chi2)<<3 ) + (Level%8);
    b1 = Qp;
    bl = Level;
  }

  static unsigned int GetMaxFirstNeighbour(){ 
    return (1<<16)-1; 
  }
  static unsigned int GetMaxNNeighbours(){ 
    return (1<<16)-1;
  }

  unsigned int GetLHit() const { 
    return w0; 
  }
  unsigned int GetMHit() const { 
    return w1; 
  }
  unsigned int GetRHit() const { 
    return w2; 
  }

  unsigned int GetFirstNeighbour() const { 
    return w1; 
  }
  unsigned int GetNNeighbours() const { 
    return w2; 
  }
  
  unsigned char GetLevel() const {
//    return b0%8;
      return bl;
  }

  unsigned char GetQp() const {
    return b1;
  }

  float GetChi2() const {
    float x = (b0>>3)*3.5/31.;
    return x*x;
  }

  float GetQpOrig( float MaxInvMom ){
    return(GetQp()*(MaxInvMom*2)/255. - MaxInvMom);
  }

};

#endif
