#include "CbmEcalClusterV1.h"

#include "CbmEcalCell.h"

#include "TMath.h"

#include <algorithm>

using namespace std;

/** An empty constructor **/
CbmEcalClusterV1::CbmEcalClusterV1()
{
  ;
}

/** A standart constructor **/
CbmEcalClusterV1::CbmEcalClusterV1(Int_t num, const std::list<CbmEcalCell*>& cluster)
  : fNum(num)
{
  std::list<CbmEcalCell*> cls;
  cls.clear();
  list<CbmEcalCell*>::const_iterator p=cluster.begin();
  for(;p!=cluster.end();++p)
    cls.push_back(*p);
  Init(cls);
}

/** An virtual destructor **/
CbmEcalClusterV1::~CbmEcalClusterV1()
{
  ;
}

/** An initialization **/
void CbmEcalClusterV1::Init(std::list<CbmEcalCell*>& fCells)
{
  list<CbmEcalCell*>::const_iterator p1;
  list<CbmEcalCell*> neib;
  list<CbmEcalCell*> fPeaks;
  Double_t e;
  Double_t x;
  Double_t y;
  Double_t r;
  Double_t ar=0;
  Double_t tx=0;
  Double_t ty=0;
  Double_t tr=0;
  Int_t i;

  fCells.sort(CbmEcalClusterSortProcess());
  fCells.reverse();
  list<CbmEcalCell*>::const_iterator p=fCells.begin();
  fEnergy=0;
  fX=0;
  fY=0;
  fType=0;

  fSize=fCells.size();
  fPeaks.clear();
  for(;p!=fCells.end();++p)
  {
    e=(*p)->GetTotalEnergy();
    fEnergy+=e;
    x=(*p)->GetCenterX(); y=(*p)->GetCenterY();
    fX+=e*x; x*=x; tx+=e*x;
    fY+=e*y; y*=y; ty+=e*y;
    r=TMath::Sqrt(x+y);
    ar+=e*r; tr+=e*(x+y);
    fType+=(*p)->GetType();
    neib.clear();
    (*p)->GetNeighborsList(0, neib);
    for(p1=neib.begin(); p1!=neib.end();++p1)
      if ((*p1)->GetTotalEnergy()>e) break;
    if (p1!=neib.end()) continue;
    for(p1=neib.begin(); p1!=neib.end();++p1)
      if (find(fPeaks.begin(), fPeaks.end(), *p1)!=fPeaks.end()) break;
    if (p1!=neib.end()) continue;
    fPeaks.push_back(*p);
  }
  fX/=fEnergy;
  fY/=fEnergy;
  fType/=fSize;
  fMaxs=fPeaks.size();
  fChi2=-1111;
  fMomentX=0; fMomentY=0; fMoment=0;
  for(p=fCells.begin();p!=fCells.end();++p)
  {
    /** Still not clear about next 3 variables **/
    e=(*p)->GetTotalEnergy();
    x=fX-(*p)->GetCenterX(); x*=x;
    y=fY-(*p)->GetCenterY(); y*=y;
    fMomentX+=x*e; fMomentY+=y*e; fMoment+=(x+y)*e;
  }
  fMomentX/=fEnergy; fMomentY/=fEnergy; fMoment/=fEnergy;
  fCellNums.Set(fSize);
  fPeakNums.Set(fMaxs);
  i=0;
  for(p=fCells.begin();p!=fCells.end();++p)
    fCellNums[i++]=(*p)->GetCellNumber();
  i=0;
  for(p=fPeaks.begin();p!=fPeaks.end();++p)
    fPeakNums[i++]=(*p)->GetCellNumber();

}

ClassImp(CbmEcalClusterV1)
