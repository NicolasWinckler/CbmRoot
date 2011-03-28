/*
 *====================================================================
 *
 *  CBM KF Track Quality
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  KF Fit performance 
 *
 *====================================================================
 */

#ifndef _CbmKFTrErrMCPoints_h_
#define _CbmKFTrErrMCPoints_h_

#include <CbmMvdPoint.h>
#include <CbmStsPoint.h>
#include <FairMCPoint.h>

#include "TClonesArray.h"
#include <vector>

using std::vector;

class CbmKFTrErrMCPoints : public TObject {

 public:

  CbmKFTrErrMCPoints();
  ~CbmKFTrErrMCPoints() {};

  CbmMvdPoint* GetMvdPoint(Int_t i) {return MvdArray[i];}
  CbmStsPoint* GetStsPoint(Int_t i) {return StsArray[i];}

  void SetMvdPoint(CbmMvdPoint* mp) { MvdArray.push_back(mp); }
  void SetStsPoint(CbmStsPoint* sp) { StsArray.push_back(sp); }

  void SetMvdPoint(int imp) { MvdArrayIndex.push_back(imp); }
  void SetStsPoint(int isp) { StsArrayIndex.push_back(isp); }

  int GetNMvdPoints() {return MvdArrayIndex.size();}
  int GetNStsPoints() {return StsArrayIndex.size();}

  int GetMvdPointIndex(Int_t i) {return MvdArrayIndex[i];}
  int GetStsPointIndex(Int_t i) {return StsArrayIndex[i];}

  double GetMvdPointX(int i) {return MvdArray[i]->FairMCPoint::GetX();}
  double GetMvdPointY(int i) {return MvdArray[i]->FairMCPoint::GetY();}
  double GetMvdPointZ(int i) {return MvdArray[i]->GetZ();}
  double GetMvdPointPx(int i) {return MvdArray[i]->GetPx();}
  double GetMvdPointPy(int i) {return MvdArray[i]->GetPy();}
  double GetMvdPointPz(int i) {return MvdArray[i]->GetPz();}

  double GetStsPointX(int i) {return StsArray[i]->FairMCPoint::GetX();}
  double GetStsPointY(int i) {return StsArray[i]->FairMCPoint::GetY();}
  double GetStsPointZ(int i) {return StsArray[i]->GetZ();}
  double GetStsPointPx(int i) {return StsArray[i]->GetPx();}
  double GetStsPointPy(int i) {return StsArray[i]->GetPy();}
  double GetStsPointPz(int i) {return StsArray[i]->GetPz();}

  vector<CbmStsPoint*> StsArray;
  vector<CbmMvdPoint*> MvdArray;

  vector<int> StsArrayIndex;
  vector<int> MvdArrayIndex;

 private:
  //TClonesArray *StsArray;
  //TClonesArray *MvdArray;

 ClassDef(CbmKFTrErrMCPoints,1);
};

#endif // _CbmKFTrErrMCPoints_h_ 
