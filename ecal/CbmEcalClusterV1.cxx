#include "CbmEcalClusterV1.h"

#include "CbmEcalCell.h"

#include "TMath.h"

#include <algorithm>

using namespace std;

/** An empty constructor **/
CbmEcalClusterV1::CbmEcalClusterV1()
{
  fCells.clear();
  fPeaks.clear();
  fPhotons.clear();
}

/** A standart constructor **/
CbmEcalClusterV1::CbmEcalClusterV1(Int_t num, const std::list<CbmEcalCell*>& cluster)
  : fNum(num)
{
  fCells.clear();
  list<CbmEcalCell*>::const_iterator p=cluster.begin();
  for(;p!=cluster.end();++p)
    fCells.push_back(*p);
  Init();
}

/** An virtual destructor **/
CbmEcalClusterV1::~CbmEcalClusterV1()
{
  fCells.clear();
  fPeaks.clear();
  fPhotons.clear();
}

/** An initialization **/
void CbmEcalClusterV1::Init()
{
  list<CbmEcalCell*>::const_iterator p1;
  list<CbmEcalCell*> neib;
  Double_t e;
  Double_t x;
  Double_t y;
  Double_t r;
  Double_t ar=0;
  Double_t tx=0;
  Double_t ty=0;
  Double_t tr=0;

  //fCells.sort(CbmEcalClusterSortProcess());
  //fCells.reverse();
  list<CbmEcalCell*>::const_iterator p=Begin();
  fEnergy=0;
  fX=0;
  fY=0;
  fType=0;

  fSize=fCells.size();
  fPeaks.clear();
  for(;p!=End();++p)
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
  /** Still not clear about next 3 variables **/
  fMomentX=tx-fX*fX/fSize;
  fMomentY=ty-fY*fY/fSize;
  fMoment=tr-ar*ar/fSize;
  fX/=fEnergy;
  fY/=fEnergy;
  fType/=fSize;
  fMaxs=fPeaks.size();
  fChi2=-1111;
  fPhotons.clear();
}

ClassImp(CbmEcalClusterV1)
