#include "CbmEcalQualityCheck.h"

#include "FairRootManager.h"

#include "CbmMCTrack.h"
#include "FairTrackParam.h"

#include "CbmEcalStructure.h"
#include "CbmEcalInf.h"
#include "CbmEcalRecParticle.h"
#include "CbmEcalCell.h"
#include "CbmEcalPoint.h"
#include "CbmEcalShowerLib.h"
#include "CbmEcalClusterV1.h"

#include "TTree.h"
#include "TClonesArray.h"
#include "TASImage.h"
#include "TText.h"

#include <stdio.h>

#include <iostream>

using namespace std;

class CbmEcalQualityCheckItem
{
public:
  CbmEcalQualityCheckItem(CbmEcalCell* cell, Float_t r, Float_t g, Float_t b)
    : fCell(cell), fR(r), fG(g), fB(b) {};
  CbmEcalCell* fCell;
  Float_t fR;
  Float_t fG;
  Float_t fB;
};

void CbmEcalQualityCheck::Exec(Option_t* option)
{
  Int_t i;
  Int_t j;
  Int_t pn=fPoints->GetEntriesFast();
  Int_t trn=fMCTracks->GetEntriesFast();
  Int_t rn=fReco->GetEntriesFast();
  CbmEcalPoint* p;
  CbmEcalPoint* po;
  CbmMCTrack* tr;
  CbmMCTrack* tq;
  CbmEcalRecParticle* r;
  CbmEcalRecParticle* rm;
  Double_t rmin;
  Double_t rad;
  Double_t dx;
  Double_t dy;

  CreateTree();
  fEventN++;
  if (fVerbose>0)
    Info("Exec", "Event %d.", fEventN);
  for(i=0;i<pn;i++)
  {
    p=(CbmEcalPoint*)fPoints->At(i);
    if (!p) continue;
    tr=(CbmMCTrack*)fMCTracks->At(p->GetTrackID());
    /** Only photons **/
    if (tr->GetPdgCode()!=22&&TMath::Abs(tr->GetPdgCode())!=2112) continue;
    /** ... prompt and pi0 decay photons ... **/
    if (tr->GetStartZ()>0.1) continue;
    /** ... energetic enough ... **/
    if (tr->GetEnergy()<0.5) continue;
    /** ... from pi^0 decay ... **/
    if (tr->GetMotherId()>=0&&tr->GetPdgCode()==22)
    {
      tq=(CbmMCTrack*)fMCTracks->At(tr->GetMotherId());
      if (tq->GetPdgCode()!=111) continue;
    }
    for(j=0;j<trn;j++)
    {
      tq=(CbmMCTrack*)fMCTracks->At(j);
      if (tq->GetMotherId()==p->GetTrackID())
        break;
    }
    /** ... and without daughter tracks. **/
    if (trn!=j) continue;
    rmin=1e10;
    for(j=0;j<pn;j++)
    {
      if (i==j) continue;
      po=(CbmEcalPoint*)fPoints->At(j);
      dx=po->GetX(); dx-=p->GetX(); dx*=dx;
      dy=po->GetY(); dy-=p->GetY(); dy*=dy;
      rad=dx; rad+=dy;
      if (rad<rmin) rmin=rad;
    }
    fR=TMath::Sqrt(rmin);

    fMCX=p->GetX();
    fMCY=p->GetY();
    fMCZ=p->GetZ();
    fMCPX=p->GetPx();
    fMCPY=p->GetPy();
    fMCPZ=p->GetPz();
    fMCE=tr->GetEnergy();
    fMCMotherTrN=tr->GetMotherId();
    fPdgCode=tr->GetPdgCode();
    rmin=2e10; rm=NULL;
    for(j=0;j<rn;j++)
    {
      r=(CbmEcalRecParticle*)fReco->At(j);
      rad=(fMCX-r->X())*(fMCX-r->X())+(fMCY-r->Y())*(fMCY-r->Y())+(fMCZ-r->Z())*(fMCZ-r->Z());
      if (rad<rmin)
      {
	rmin=rad;
	rm=r;
      }
    }
    if (rm==NULL)
    {
      Info("Exec", "No reconstructed photons found!");
      continue;
    }
    fX=rm->X(); fY=rm->Y(); fZ=rm->Z();
    fE=rm->E(); fPX=rm->Px(); fPY=rm->Py(); fPZ=rm->Pz();
    fChi2=rm->Chi2();
    fOut->Fill();
  }
  if (fC) DrawImage();
}

void CbmEcalQualityCheck::PutPixel(Int_t x, Int_t y, Float_t r, Float_t g, Float_t b)
{
  if (x<0||x>=fCX) return;
  if (y<0||y>=fCY) return;
  if (r<0||r>1)return;
  if (g<0||g>1)return;
  if (b<0||b>1)return;
  Int_t ri=(Int_t)(255.0*r);
  Int_t gi=(Int_t)(255.0*g);
  Int_t bi=(Int_t)(255.0*b);
  static char color[10];
  sprintf(color, "#%.2X%.2X%.2X", ri, gi, bi);
  fC->PutPixel(x, y, color);
}

void CbmEcalQualityCheck::PutPixel(Int_t x, Int_t y, const char* color)
{
  if (x<0||x>=fCX) return;
  if (y<0||y>=fCY) return;
  fC->PutPixel(x, fCY-y-1, color);
}

/** draw cell with filled color. fEdging used as egding of cell **/
void CbmEcalQualityCheck::DrawCell(CbmEcalCell* cell, Float_t r, Float_t g, Float_t b)
{
  Int_t ri=(Int_t)(255.0*r);
  Int_t gi=(Int_t)(255.0*g);
  Int_t bi=(Int_t)(255.0*b);
  static char color[10];
  sprintf(color, "#%.2X%.2X%.2X", ri, gi, bi);
  DrawCell(cell, color);
}

void CbmEcalQualityCheck::DrawCell(CbmEcalCell* cell, const char* color)
{
  Int_t xi1=(Int_t)((cell->X1()/fInf->GetEcalSize(0))*fCX+fCX/2.0);
  Int_t xi2=(Int_t)((cell->X2()/fInf->GetEcalSize(0))*fCX+fCX/2.0);
  Int_t yi1=(Int_t)((cell->Y1()/fInf->GetEcalSize(1))*fCY+fCY/2.0);
  Int_t yi2=(Int_t)((cell->Y2()/fInf->GetEcalSize(1))*fCY+fCY/2.0);
  for(Int_t x=xi1;x<=xi2;x++)
    for(Int_t y=yi1;y<=yi2;y++)
      if (x==xi1||y==yi1||x==xi2||y==yi2)
	PutPixel(x, y, fEdging);
      else
	PutPixel(x, y, color);

}

void CbmEcalQualityCheck::DrawMark(Double_t x, Double_t y, const char* color, Int_t type)
{
  Int_t xi=(Int_t)((x/fInf->GetEcalSize(0))*fCX+fCX/2.0);
  Int_t yi=(Int_t)((y/fInf->GetEcalSize(1))*fCY+fCY/2.0);
  PutPixel(xi-1, yi-1, color);
  PutPixel(xi  , yi-1, color);
  PutPixel(xi+1, yi-1, color);
  PutPixel(xi-1, yi  , color);
  PutPixel(xi  , yi  , color);
  PutPixel(xi+1, yi  , color);
  PutPixel(xi-1, yi+1, color);
  PutPixel(xi  , yi+1, color);
  PutPixel(xi+1, yi+1, color);
  if (type==1) return;
  PutPixel(xi-2, yi-2, color);
  PutPixel(xi+2, yi-2, color);
  PutPixel(xi-2, yi+2, color);
  PutPixel(xi+2, yi+2, color);
  PutPixel(xi  , yi-2, color);
  PutPixel(xi-2, yi  , color);
  PutPixel(xi  , yi+2, color);
  PutPixel(xi+2, yi  , color);
  PutPixel(xi  , yi-3, color);
  PutPixel(xi-3, yi  , color);
  PutPixel(xi  , yi+3, color);
  PutPixel(xi+3, yi  , color);
}

void CbmEcalQualityCheck::DrawImage()
{
  TString name=fNamePrefix;
  if (fEventN<10) name+="0";
  if (fEventN<100) name+="0";
  if (fEventN<1000) name+="0";
  name+=fEventN;
  fC->FillRectangle("#000000", 0, 0, fCX, fCY);
  fEdging="#FFFFFF";
  DrawCells();
  DrawMC();
  DrawTracks();
  DrawPhotons();
  /** some beauty **/
  /*
  PutPixel((Int_t)(fCX/2.0+ 0), (Int_t)(fCY/2.0+ 0), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 0), (Int_t)(fCY/2.0+ 1), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 0), (Int_t)(fCY/2.0+ 3), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 0), (Int_t)(fCY/2.0+ 4), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 1), (Int_t)(fCY/2.0+ 2), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 2), (Int_t)(fCY/2.0+ 0), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 2), (Int_t)(fCY/2.0+ 1), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 2), (Int_t)(fCY/2.0+ 3), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 2), (Int_t)(fCY/2.0+ 4), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 4), (Int_t)(fCY/2.0+ 0), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 4), (Int_t)(fCY/2.0+ 3), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 4), (Int_t)(fCY/2.0+ 4), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 5), (Int_t)(fCY/2.0+ 0), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 5), (Int_t)(fCY/2.0+ 2), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 6), (Int_t)(fCY/2.0+ 1), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 6), (Int_t)(fCY/2.0+ 2), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 6), (Int_t)(fCY/2.0+ 3), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 6), (Int_t)(fCY/2.0+ 4), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 8), (Int_t)(fCY/2.0+ 0), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 8), (Int_t)(fCY/2.0+ 1), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 8), (Int_t)(fCY/2.0+ 2), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 8), (Int_t)(fCY/2.0+ 3), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 8), (Int_t)(fCY/2.0+ 4), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 9), (Int_t)(fCY/2.0+ 1), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+ 9), (Int_t)(fCY/2.0+ 6), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+10), (Int_t)(fCY/2.0+ 2), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+10), (Int_t)(fCY/2.0+ 6), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+11), (Int_t)(fCY/2.0+ 0), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+11), (Int_t)(fCY/2.0+ 1), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+11), (Int_t)(fCY/2.0+ 2), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+11), (Int_t)(fCY/2.0+ 3), "#FFFFFF");
  PutPixel((Int_t)(fCX/2.0+11), (Int_t)(fCY/2.0+ 4), "#FFFFFF");
  */
  fC->WriteImage(name+".png", TImage::kPng);
}

void CbmEcalQualityCheck::DrawLine(Double_t x1, Double_t y1, Double_t x2, Double_t y2,const char* color)
{
  Int_t x1i=(Int_t)((x1/fInf->GetEcalSize(0))*fCX+fCX/2.0);
  Int_t y1i=(Int_t)((y1/fInf->GetEcalSize(1))*fCY+fCY/2.0);
  Int_t x2i=(Int_t)((x2/fInf->GetEcalSize(0))*fCX+fCX/2.0);
  Int_t y2i=(Int_t)((y2/fInf->GetEcalSize(1))*fCY+fCY/2.0);
  if (x1i<0) x1i=0;
  if (x1i>=fCX) x1i=fCX-1;
  if (x2i<0) x2i=0;
  if (x2i>=fCX) x2i=fCX-1;
  if (y1i<0) y1i=0;
  if (y1i>=fCY) y1i=fCY-1;
  if (y2i<0) y2i=0;
  if (y2i>=fCY) y2i=fCY-1;
  y1i=fCY-y1i-1;
  y2i=fCY-y2i-1;
  fC->DrawLine(x1i, y1i, x2i, y2i, color);
}

void CbmEcalQualityCheck::DrawLine(Double_t x, Double_t y, const char* color, Int_t track)
{
  if (track==-1111) return;
  Int_t i=0;
  Int_t n=fPoints->GetEntriesFast();
  CbmEcalPoint* pt;

  for(i=0;i<n;i++)
  {
    pt=(CbmEcalPoint*)fPoints->At(i);
    if (pt->GetTrackID()==track) break; 
  }
  if (i==n) return;
  DrawLine(x, y, pt->GetX(), pt->GetY(), color);
}

void CbmEcalQualityCheck::DrawPhotons()
{
  Int_t rn=fReco->GetEntriesFast();
  CbmEcalRecParticle* r;
  Int_t i;
  for(i=0;i<rn;i++)
  {
    r=(CbmEcalRecParticle*)fReco->At(i);
    DrawLine(r->X(), r->Y(), "#0000FF", r->MCTrack());
    DrawMark(r->X(), r->Y(), "#0000FF", 0);
    DrawEnergy(r->X(), r->Y(), r->E(), "#0000FF");
    DrawChi2(r->X(), r->Y(), r->Chi2(), "#0000FF");
  }
}

void CbmEcalQualityCheck::DrawTracks()
{
  FairTrackParam* tr;
  Int_t i;
  Int_t n=fTracks->GetEntriesFast();
  Double_t p;
  for(i=0;i<n;i++)
  {
    tr=(FairTrackParam*)fTracks->At(i);
    p=(TMath::Abs(tr->GetQp())>1.e-4)?1./TMath::Abs(tr->GetQp()):1.e4;
    DrawMark(tr->GetX(), tr->GetY(), "#FF0000", 0);
    DrawEnergy(tr->GetX(), tr->GetY(), p, "#FF7777");
  }
}

Double_t CbmEcalQualityCheck::GetP(CbmEcalPoint* p)
{
  Double_t px=p->GetPx();
  Double_t py=p->GetPy();
  Double_t pz=p->GetPz();

  return TMath::Sqrt(px*px+py*py+pz*pz);
}

  /** Draw chi2 for photons **/
void CbmEcalQualityCheck::DrawChi2(Float_t x, Float_t y, Float_t chi2, const char* color)
{
  char stri[30];
  Int_t xi=(Int_t)((x/fInf->GetEcalSize(0))*fCX+fCX/2.0);
  Int_t yi=(Int_t)((y/fInf->GetEcalSize(1))*fCY+fCY/2.0);
  yi-=8;
  yi=fCY-yi-1;
  xi-=6;

  if (chi2!=-1111)
  {
    sprintf(stri,"%.1f", chi2);
    fC->DrawText(xi, yi, stri, 8, color, "arialbd.ttf");
  }
  else
    fC->DrawText(xi, yi, "-111", 8, color, "arialbd.ttf");
}


void CbmEcalQualityCheck::DrawEnergy(CbmEcalPoint* p, const char* color)
{
  if (GetP(p)<0.01) return;
  char stri[10];
  Int_t x=(Int_t)((p->GetX()/fInf->GetEcalSize(0))*fCX+fCX/2.0);
  Int_t y=(Int_t)((p->GetY()/fInf->GetEcalSize(1))*fCY+fCY/2.0);
  y+=9;
  y=fCY-y-1;
  x-=6;

  sprintf(stri,"%.2f", GetP(p));
  if (stri[0]=='0')
  {
    x+=2;
    stri[0]=stri[1];
    stri[1]=stri[2];
    stri[2]=stri[3];
    stri[3]=stri[4];
    stri[4]=stri[5];
    stri[5]=stri[6];
    stri[6]=stri[7];
  }
  fC->DrawText(x, y, stri, 8, color, "arialbd.ttf");
}

void CbmEcalQualityCheck::DrawEnergy(Float_t x, Float_t y, Float_t p, const char* color)
{
  if (p<0.01) return;
  char stri[10];
  Int_t xi=(Int_t)((x/fInf->GetEcalSize(0))*fCX+fCX/2.0);
  Int_t yi=(Int_t)((y/fInf->GetEcalSize(1))*fCY+fCY/2.0);
  yi-=2;
  yi=fCY-yi-1;
  xi-=6;

  sprintf(stri,"%.2f", p);
  fC->DrawText(xi, yi, stri, 8, color, "arialbd.ttf");
}

void CbmEcalQualityCheck::DrawMC()
{
  Int_t i;
  Int_t pn=fPoints->GetEntriesFast();
  CbmEcalPoint* p;
  CbmMCTrack* tr;
  CbmMCTrack* tq;


  for(i=0;i<pn;i++)
  {
    p=(CbmEcalPoint*)fPoints->At(i);
    if (!p) continue;
    tr=(CbmMCTrack*)fMCTracks->At(p->GetTrackID());
    /** Secondary gammas **/
    if (tr->GetPdgCode()==22&&tr->GetStartZ()>=0.1)
    {
      DrawMark(p->GetX(), p->GetY(), "#00FF00", 1);
      DrawEnergy(p, "#00FF00");
      continue;
    }
    if (TMath::Abs(tr->GetPdgCode()==11))
    {
      if (tr->GetMotherId()>=0)
      {
	tq=(CbmMCTrack*)fMCTracks->At(tr->GetMotherId());
	/** Secondary electrons **/
	if (tq->GetPdgCode()==22)
	{
          DrawMark(p->GetX(), p->GetY(), "#FFFF00", 1);
          DrawEnergy(p, "#FFFF00");
          continue;
	}
      }
    }
    /** Neutrons and antineutrons **/
    if (TMath::Abs(tr->GetPdgCode()==2112))
    {
      DrawMark(p->GetX(), p->GetY(), "#FF00FF", 1);
      DrawEnergy(p, "#FF00FF");
      continue;
    }
    /** others **/
    DrawMark(p->GetX(), p->GetY(), "#FF0000", 1);
    DrawEnergy(p, "#FF0000");
  }
  for(i=0;i<pn;i++)
  {
    p=(CbmEcalPoint*)fPoints->At(i);
    if (!p) continue;
    tr=(CbmMCTrack*)fMCTracks->At(p->GetTrackID());
    if (tr->GetPdgCode()==22&&tr->GetStartZ()<0.1)
    {
      DrawMark(p->GetX(), p->GetY(), "#00FFFF", 1);
      DrawEnergy(p, "#00FFFF");
    }
  }
}

void CbmEcalQualityCheck::DrawCells()
{
  list<CbmEcalQualityCheckItem*>::const_iterator p;
  list<CbmEcalCell*> clusters;
  list<CbmEcalCell*>::const_iterator cp;
  CbmMCTrack* tr;
  CbmMCTrack* tq;
  Float_t max;
  std::map<Int_t, Float_t>::const_iterator p1;
  CbmEcalCell* c;
  CbmEcalRecParticle* r;
  Int_t rn=fReco->GetEntriesFast();
  Int_t i;
  Int_t j;
  Float_t x;
  Float_t y;
  Float_t dx;
  Float_t dy;
  Float_t rad;
  Float_t phi;

  clusters.clear();

  for(i=0;i<rn;i++)
  {
    r=(CbmEcalRecParticle*)fReco->At(i);
    CbmEcalClusterV1* cl=(CbmEcalClusterV1*)fClusters->At(r->ClusterNum());
    for(j=0;j<cl->Size();j++)
      clusters.push_back(fStr->GetHitCell(cl->CellNum(j)));
  }

  for(p=fCells.begin();p!=fCells.end();++p)
  {
    c=(*p)->fCell;
    (*p)->fG=c->GetTotalEnergy();
    (*p)->fR=0;
    (*p)->fB=0;
    for(p1=c->GetTrackEnergyBegin();p1!=c->GetTrackEnergyEnd();++p1)
    {
      tr=(CbmMCTrack*)fMCTracks->At(p1->first);
      if (tr->GetPdgCode()==22)
	(*p)->fR+=p1->second;
      else
      if (tr->GetMotherId()>=0)
      {
	tq=(CbmMCTrack*)fMCTracks->At(tr->GetMotherId());
	if (tr->GetMotherId()==22)
	  (*p)->fR+=p1->second;
      }
    }
    for(p1=c->GetTrackPSEnergyBegin();p1!=c->GetTrackPSEnergyEnd();++p1)
    {
      tr=(CbmMCTrack*)fMCTracks->At(p1->first);
      if (tr->GetPdgCode()==22)
	(*p)->fR+=p1->second;
      else
      if (tr->GetMotherId()>=0)
      {
	tq=(CbmMCTrack*)fMCTracks->At(tr->GetMotherId());
	if (tr->GetMotherId()==22)
	  (*p)->fR+=p1->second;
      }
    }
    if (find(clusters.begin(), clusters.end(), c)==clusters.end())
      continue;
    for(i=0;i<rn;i++)
    {
      r=(CbmEcalRecParticle*)fReco->At(i);
      dx=x=c->GetCenterX();
      dy=y=c->GetCenterY();
      dx-=r->X();
      dy-=r->Y();
      if (TMath::Abs(dx)>30) continue;
      if (TMath::Abs(dy)>30) continue;
      rad=TMath::Sqrt(x*x+y*y);
      phi=TMath::ACos(x/rad)*TMath::RadToDeg();
      if (y<0) phi=360-phi;
      (*p)->fB+=fLib->GetSumEThetaPhi(dx, dy, fInf->GetModuleSize()/c->GetType(), r->E(), TMath::ATan(rad/fInf->GetZPos())*TMath::RadToDeg(), phi);
    }
  }
  /** Normalization **/
  max=0;
  for(p=fCells.begin();p!=fCells.end();++p)
  {
    if (max<(*p)->fR) max=(*p)->fR;
    if (max<(*p)->fG) max=(*p)->fG;
    if (max<(*p)->fB) max=(*p)->fB;
  }
  if (max>0)
    for(p=fCells.begin();p!=fCells.end();++p)
    {
      (*p)->fR/=max;
      (*p)->fG/=max;
      (*p)->fB/=max;
    }

  /** At the moment
   **	B --- reconstructed energy
   **   G --- energy deposited in cell
   **   R --- energy deposited by photons and their e+/e- daughters **/
  for(p=fCells.begin();p!=fCells.end();++p)
  {
    Float_t l=(*p)->fG; l=TMath::Sqrt(l);
    Float_t a=0;
    Float_t b=((*p)->fG-(*p)->fB)/((*p)->fG+(*p)->fB);
    if ((*p)->fB==0) { b=0; } 
    else
    {
      if (b<-0.7) b=-0.7;
      if (b>0.7) b=0.7;
    }
    LabToRGB(l, b, a, (*p)->fR, (*p)->fG, (*p)->fB);
  }
  /** Drawing **/
  fEdging="#FFFFFF";
  for(p=fCells.begin();p!=fCells.end();++p)
    if (find(clusters.begin(), clusters.end(), (*p)->fCell)==clusters.end())
      DrawCell((*p)->fCell, (*p)->fR, (*p)->fG, (*p)->fB);
  fEdging="#FF0000";
  for(p=fCells.begin();p!=fCells.end();++p)
    if (find(clusters.begin(), clusters.end(), (*p)->fCell)!=clusters.end())
      DrawCell((*p)->fCell, (*p)->fR, (*p)->fG, (*p)->fB);
}

void CbmEcalQualityCheck::CreateTree()
{
  if (fOut) return;
  fOut=new TTree(fOutName, "Quality control tree for calorimeter");
  fOut->Branch("x", &fX, "x/D");
  fOut->Branch("y", &fY, "y/D");
  fOut->Branch("z", &fZ, "z/D");
  fOut->Branch("px", &fPX, "px/D");
  fOut->Branch("py", &fPY, "py/D");
  fOut->Branch("pz", &fPZ, "pz/D");
  fOut->Branch("mcx", &fMCX, "mcx/D");
  fOut->Branch("mcy", &fMCY, "mcy/D");
  fOut->Branch("mcz", &fMCZ, "mcz/D");
  fOut->Branch("mcpx", &fMCPX, "mcpx/D");
  fOut->Branch("mcpy", &fMCPY, "mcpy/D");
  fOut->Branch("mcpz", &fMCPZ, "mcpz/D");
  fOut->Branch("e", &fE, "e/D");
  fOut->Branch("mce", &fMCE, "mce/D");
  fOut->Branch("ev", &fEventN, "ev/I");
  fOut->Branch("chi2", &fChi2, "chi2/D");
  fOut->Branch("r", &fR, "r/D");
  fOut->Branch("mcmothertrackn", &fMCMotherTrN, "mcmothertrackn/I");
  fOut->Branch("pdg", &fPdgCode, "pdg/I");
}

/** Init **/
InitStatus CbmEcalQualityCheck::Init()
{
  fEventN=0;


  FairRootManager* io=FairRootManager::Instance();
  if (!io)
  {
    Fatal("Init", "Can't find IOManager.");
    return kFATAL;
  }
  fReco=(TClonesArray*)io->GetObject(fInName);
  if (!fReco)
  {
    Fatal("Init", "Can't find array of reconstructed tracks named %s.", fInName.Data());
    return kFATAL;
  }
  fMCTracks=(TClonesArray*)io->ActivateBranch("MCTrack");
  if (!fMCTracks)
  {
    Fatal("Init", "Can't find array of MC tracks");
    return kFATAL;
  }
  fPoints=(TClonesArray*)io->ActivateBranch("ECALPoint");
  if (!fPoints)
  {
    Fatal("Init", "Can't find array of Ecal Points");
    return kFATAL;
  }
  if (fNeedDraw==kFALSE) return kSUCCESS;
  fStr=(CbmEcalStructure*)io->GetObject("EcalStructure");
  if (!fStr)
  {
    Fatal("Init", "Can't find calorimeter structure for drawing");
    return kFATAL;
  }
  fTracks=(TClonesArray*)io->GetObject("EcalTrackParam");
  if (!fTracks)
  {
    Fatal("Init", "Can't find array of reconstructed tracks");
    return kFATAL;
  }
  fClusters=(TClonesArray*)io->GetObject("EcalClusters");
  if (!fClusters)
  {
    Fatal("Init", "Can't find array of calorimeter clusters");
    return kFATAL;
  }
  fInf=fStr->GetEcalInf();
  fCX=fInf->GetXSize(); fCY=fInf->GetYSize();
  fCX*=fCellSize*4+4; fCX+=1;
  fCY*=fCellSize*4+4; fCY+=1;
  fC=new TASImage(fCX, fCY);

  list<CbmEcalCell*> cells;
  list<CbmEcalCell*>::const_iterator p;
  fStr->GetCells(cells);
  for(p=cells.begin();p!=cells.end();++p)
    fCells.push_back(new CbmEcalQualityCheckItem((*p), 0.0, 0.0, 0.0));
  fLib=(CbmEcalShowerLib*)io->GetObject("EcalShowerLib");
  if (!fLib)
  {
    Fatal("Init", "No shower library found in system.");
    return kFATAL;
  }
  fTxt->SetTextFont(43);
  fTxt->SetTextSizePixels(8);
  fTxt->SetTextAlign(21);
  return kSUCCESS;
}

void CbmEcalQualityCheck::Finish()
{
  if (fOut) fOut->Write();
}


/** Destructor **/
CbmEcalQualityCheck::~CbmEcalQualityCheck()
{
  list<CbmEcalQualityCheckItem*>::const_iterator p=fCells.begin();
  for(;p!=fCells.end();++p)
    delete (*p);
  delete fTxt;
}

/** Standard constructor **/
CbmEcalQualityCheck::CbmEcalQualityCheck(const char *name, const Int_t iVerbose)
  : FairTask(name, iVerbose)
{
  fNeedDraw=kFALSE;
  fInName="EcalReco";
  fOutName="EcalQC";
  fOut=NULL;
  fMCTracks=NULL;
  fReco=NULL;
  fPoints=NULL;
  fNamePrefix="ecal";
  fCellSize=8;
  fC=NULL;
  fTxt=new TText();
}

/** Default constructor. Requirement of ROOT system **/
CbmEcalQualityCheck::CbmEcalQualityCheck()
{
  ;
}

ClassImp(CbmEcalQualityCheck);
