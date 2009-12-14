#ifndef _L1StsHit_h_
#define _L1StsHit_h_

struct L1StsHit 
{ 
  unsigned short int f, b; // front and back strip indices
//   fscal z; // z from CbmL1::Instance()->listStsHits .
  char iz; // index of z position in vStsZPos array
};

#endif
