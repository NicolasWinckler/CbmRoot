#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <unistd.h>
#include "LxDraw.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TGeoCompositeShape.h"
#include "TGeoArb8.h"
#include "TGeoManager.h"
#include "TGeoBoolNode.h"
#include "TGeoCone.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "Lx.h"
#include "TPolyMarker.h"
#include "CbmKF.h"
#include "TMCProcess.h"

#define USE_MUCH_ABSORBER

using namespace std;

static int StaColour = 17;
static int TrackColour = 2;

LxDraw::LxDraw() : YZ("YZ", "YZ Side View", -10, -50, 650, 1000), XZ("XZ", "XZ Top View", -10, -50, 650, 1000), YX("YX", "YX Front View", -500, 0, 1000, 1000), ask(true)
{
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasBorderSize(1);
  gStyle->SetCanvasColor(0);

  YZ.Range(-15.0, -300.0, 550.0, 300.0);
  YZ.Draw();
  YZ.Update();

  XZ.Range(-15.0, -300.0, 550.0, 300.0);
  XZ.Draw();
  XZ.Update();

  YX.Range(-300.0, -300.0, 300.0, 300.0);
  YX.Draw();
  YX.Update();
}

void LxDraw::ClearView()
{
  YZ.Clear();
  XZ.Clear();
  YX.Clear();
}

void LxDraw::Ask()
{
  char symbol;

  if (ask)
  {
    cout << "ask>";

    do
    {
      cin.get(symbol);

      if (symbol == 'r')
        ask = false;

      if (symbol == 'q')
        exit;
    } while (symbol != '\n');

    cout << endl;
  }
}

void LxDraw::DrawMCTracks()
{
  char buf[128];
  int NRegMCTracks = 0;
  LxFinder* finder = LxFinder::Instance();
  TPolyLine pline;

  static int mc_tr_count = 0;

  for(vector<LxMCTrack>::iterator it = finder->MCTracks.begin(); it != finder->MCTracks.end(); ++it)
  {
    LxMCTrack& T = *it;

    //if ((13 != T.pdg && -13 != T.pdg) || T.mother_ID >= 0)
      //continue;

    pline.SetLineColor(kRed);

    if (T.p < 0.5)
      pline.SetLineColor(kBlue);

    if (T.mother_ID != -1)
      pline.SetLineColor(8);

    if ((T.mother_ID != -1) && (T.p < 0.5))
      pline.SetLineColor(12);

    double par[6];
    par[0] = T.x;
    par[1] = T.y;
    par[2] = T.px/T.pz;
    par[3] = T.py/T.pz;
    par[4] = T.q/T.p;
    par[5] = T.z;

    if(T.Points.size() < 1)
      continue;

    vector<double> lx, ly, lz;
    lx.push_back(par[0]);
    ly.push_back(par[1]);
    lz.push_back(par[5]);

    bool ok = true;

    for(std::vector<LxMCPoint*>::iterator ip = T.Points.begin(); ip != T.Points.end(); ++ip)
    {
      LxMCPoint* p = *ip;
      double par1[6];
      par1[0] = p->x;
      par1[1] = p->y;
      par1[2] = p->px/p->pz;
      par1[3] = p->py/p->pz;
      par1[4] = p->q/p->p;
      par1[5] = p->z;

      double Zfrst = par[5];
      double Zlast = par1[5];
      double step = .5;

      if(step > fabs(Zfrst - Zlast) / 5)
        step = fabs(Zfrst - Zlast) / 5;

      if(Zlast < par[5])
        step = -step;

      while (fabs(par[5] - Zlast) > fabs(step))
      {
        double znxt = par[5]+step;
        CbmKF::Instance()->Propagate(par1, 0, znxt, par1[4]);
        CbmKF::Instance()->Propagate(par, 0, znxt, par[4]);
        double w = fabs(znxt-Zfrst);
        double w1 = fabs(znxt-Zlast);

        if( w+w1 < 1.e-3 )
        {
          w = 1;
          w1 = 0;
        }

        float xl = ( w1*par[0] + w*par1[0])/(w+w1);
        float yl = ( w1*par[1] + w*par1[1])/(w+w1);
        float zl = ( w1*par[5] + w*par1[5])/(w+w1);

        if ((fabs(xl) > 400.0)||(fabs(yl) > 400.0)){
          //cout << "*** track " << NRegMCTracks+1 << " xl = " << xl << ", zl = " << zl << endl;
          //cout << "*** track " << NRegMCTracks+1 << " yl = " << yl << ", zl = " << zl << endl;
          ok = false;// Timur
          continue;// Timur
        }

        lx.push_back( ( w1*par[0] + w*par1[0])/(w+w1) );
        ly.push_back( ( w1*par[1] + w*par1[1])/(w+w1) );
        lz.push_back( ( w1*par[5] + w*par1[5])/(w+w1) );

        if (lx.size() > 200)
          break;
      }

      par[0] = p->x;
      par[1] = p->y;

      if (p->pz != 0)
      {
        par[2] = p->px/p->pz;
        par[3] = p->py/p->pz;
      }
      else
      {
        par[2] = 1000 * 1000 * 1000;
        par[3] = 1000 * 1000 * 1000;
      }

      if (p->p !=0)
        par[4] = p->q/p->p;
      else
        par[4] = 1000 * 1000 * 1000;

      par[5] = p->z;
      lx.push_back(par[0]);
      ly.push_back(par[1]);
      lz.push_back(par[5]);
    }      

    if (ok)
    {
      double max_z = 0, min_z = 500;

      for (vector<double>::iterator i = lz.begin(); i != lz.end(); ++i)
      {
        if (*i > max_z)
          max_z = *i;

        if (*i < min_z)
          min_z = *i;
      }

      if (min_z < 10 && max_z > 503)
        ++mc_tr_count;

      NRegMCTracks++;
      YZ.cd(); 
      pline.DrawPolyLine(lx.size(), &(lz[0]), &(ly[0]) );
      XZ.cd(); 
      pline.DrawPolyLine(lx.size(), &(lz[0]), &(lx[0]) );
      YX.cd(); 
      pline.DrawPolyLine(lx.size(), &(lx[0]), &(ly[0]) );
    }
  }

  cout <<"LxDraw: number of registered MC tracks: " << NRegMCTracks << endl;
  YZ.cd();
  YZ.Update();
  XZ.cd();
  XZ.Update();
  YX.cd();
  YX.Update();
}

void LxDraw::DrawInputHits()
{
  DrawMuch();

  // Draw hits
  int mcolour = 4;
  int hitsMStyle = 1;//5;
  double HitSize = 1;
  LxFinder* finder = LxFinder::Instance();
  LxSpace& lxSpace = finder->caSpace;

  int nhits = 0;

  for (int i = 0; i < 6; ++i)
  //for (int i = 0; i < 1; ++i)
  {
    LxStation* pSt = lxSpace.stations[i];

    for (int j = 0; j < 3; ++j)
    //for (int j = 0; j < 1; ++j)
    {
      LxLayer* pLa = pSt->layers[j];
      //nhits += static_cast<KDTreePointsType*> (pLa->pointsHandle)->size();
      nhits += pLa->points.size();
    }
  }

  Double_t x_poly[nhits], y_poly[nhits], z_poly[nhits];
  Double_t x_poly2[nhits], y_poly2[nhits], z_poly2[nhits];// Removed.
  Double_t x_poly3[nhits], y_poly3[nhits], z_poly3[nhits];// Restored.
  TVector3 pos, err;
  int n_poly = 0;
  int n_poly2 = 0;
  int n_poly3 = 0;

  for (int i = 5; i >= 0; --i)
  //for (int i = 0; i >= 0; --i)
  {
    LxStation* pSt = lxSpace.stations[i];

    for (int j = 2; j >= 0; --j)
    //for (int j = 0; j >= 0; --j)
    {
      LxLayer* pLa = pSt->layers[j];

      //for (KDTreePointsType::iterator k = static_cast<KDTreePointsType*> (pLa->pointsHandle)->begin(); k != static_cast<KDTreePointsType*> (pLa->pointsHandle)->end(); ++k)
      for (list<LxPoint*>::iterator k = pLa->points.begin(); k != pLa->points.end(); ++k)
      {
        LxPoint* pPo = *k;

        if (pPo->artificial)
        {
          x_poly3[n_poly3] = pPo->x;
          y_poly3[n_poly3] = pPo->y;
          z_poly3[n_poly3] = pPo->z;
          ++n_poly3;
        }
        else if (!pPo->valid)
        {
          x_poly2[n_poly2] = pPo->x;
          y_poly2[n_poly2] = pPo->y;
          z_poly2[n_poly2] = pPo->z;
          ++n_poly2;
        }
        else
        {
          x_poly[n_poly] = pPo->x;
          y_poly[n_poly] = pPo->y;
          z_poly[n_poly] = pPo->z;
          ++n_poly;
        }
      }
    }
  }

  YZ.cd();

  TPolyMarker* pmyz = new TPolyMarker(n_poly, z_poly, y_poly);
  pmyz->SetMarkerColor(mcolour);
  pmyz->SetMarkerStyle(hitsMStyle);
  pmyz->SetMarkerSize(HitSize);
  pmyz->Draw();

  TPolyMarker* pmyz2 = new TPolyMarker(n_poly2, z_poly2, y_poly2);
  pmyz2->SetMarkerColor(2);
  pmyz2->SetMarkerStyle(hitsMStyle);
  pmyz2->SetMarkerSize(HitSize);
  pmyz2->Draw();

  TPolyMarker* pmyz3 = new TPolyMarker(n_poly3, z_poly3, y_poly3);
  pmyz3->SetMarkerColor(3);
  pmyz3->SetMarkerStyle(hitsMStyle);
  pmyz3->SetMarkerSize(HitSize);
  pmyz3->Draw();
    
  XZ.cd();

  TPolyMarker* pmxz = new TPolyMarker(n_poly, z_poly, x_poly);
  pmxz->SetMarkerColor(mcolour);
  pmxz->SetMarkerStyle(hitsMStyle);
  pmxz->SetMarkerSize(HitSize);
  pmxz->Draw();

  TPolyMarker* pmxz2 = new TPolyMarker(n_poly2, z_poly2, x_poly2);
  pmxz2->SetMarkerColor(2);
  pmxz2->SetMarkerStyle(hitsMStyle);
  pmxz2->SetMarkerSize(HitSize);
  pmxz2->Draw();

  TPolyMarker* pmxz3 = new TPolyMarker(n_poly3, z_poly3, x_poly3);
  pmxz3->SetMarkerColor(3);
  pmxz3->SetMarkerStyle(hitsMStyle);
  pmxz3->SetMarkerSize(HitSize);
  pmxz3->Draw();
    
  YX.cd();

  TPolyMarker* pmyx = new TPolyMarker(n_poly, x_poly, y_poly);
  pmyx->SetMarkerColor(mcolour);
  pmyx->SetMarkerStyle(hitsMStyle);
  pmyx->SetMarkerSize(HitSize);
  pmyx->Draw();

  TPolyMarker* pmyx2 = new TPolyMarker(n_poly2, x_poly2, y_poly2);
  pmyx2->SetMarkerColor(2);
  pmyx2->SetMarkerStyle(hitsMStyle);
  pmyx2->SetMarkerSize(HitSize);
  pmyx2->Draw();

  TPolyMarker* pmyx3 = new TPolyMarker(n_poly3, x_poly3, y_poly3);
  pmyx3->SetMarkerColor(3);
  pmyx3->SetMarkerStyle(hitsMStyle);
  pmyx3->SetMarkerSize(HitSize);
  pmyx3->Draw();
}

void LxDraw::DrawMuch(TGeoNode* node)
{
  TObjArray* children;
  TObject* childO;

  if (0 != strstr(node->GetName(), "active"))
  {
    TGeoCompositeShape* cs = dynamic_cast<TGeoCompositeShape*> (node->GetVolume()->GetShape());

    if (cs)
    {
      TGeoBoolNode* bn = cs->GetBoolNode();
      TGeoTrap* trap = dynamic_cast<TGeoTrap*> (bn->GetLeftShape());

      if (trap)
      {
        Double_t minY = 0, maxY = 0, minX = 0, maxX = 0, Z = 0;
        Double_t* xy = trap->GetVertices();
        Double_t trapX[5];
        Double_t trapY[5];

        for (int i = 0; i < 4; ++i)
        {
          Double_t localCoords[3] = { xy[2*i], xy[2*i + 1], 0. };
          Double_t globalCoords[3];
          gGeoManager->LocalToMaster(localCoords, globalCoords);

          if (minY > globalCoords[1])
            minY = globalCoords[1];

          if (maxY < globalCoords[1])
            maxY = globalCoords[1];

          if (minX > globalCoords[0])
            minX = globalCoords[0];

          if (maxX < globalCoords[0])
            maxX = globalCoords[0];

          Z = globalCoords[2];
          trapX[i] = globalCoords[0];
          trapY[i] = globalCoords[1];
        }

        trapX[4] = trapX[0];
        trapY[4] = trapY[0];

        YZ.cd();
        TLine* line = new TLine();
        line->SetLineColor(StaColour);
        line->DrawLine(Z, minY, Z,  maxY);

        XZ.cd();
        line->DrawLine(Z, minX, Z,  maxX);

        YX.cd();
        TPolyLine* pline = new TPolyLine(5, trapX, trapY);
        pline->SetFillColor(StaColour);
        pline->SetLineColor(kCyan + 4);
        pline->SetLineWidth(1);
        pline->Draw("f");
        pline->Draw();
      }
    }

    goto exit;
  }

  children = node->GetNodes();

  if (0 == children)
    goto exit;

  childO = children->First();

  while (childO)
  {
    TGeoNode* child = dynamic_cast<TGeoNode*> (childO);

    if (child)
    {
      gGeoManager->GetCurrentNavigator()->CdDown(child);
      DrawMuch(child);
    }

    childO = children->After(childO);
  }

exit:
  gGeoManager->CdUp();
}

void LxDraw::DrawMuch()
{
  TLatex latex;
  latex.SetTextFont(132);
  latex.SetTextAlign(12);
  latex.SetTextSize(0.035);

  YZ.cd();
  latex.DrawLatex(0.0, 250.0, "YZ Side View");
  YZ.Draw();

  XZ.cd();
  latex.DrawLatex(0.0, 250.0, "XZ Top View");
  XZ.Draw();

  YX.cd();
  latex.DrawLatex(-270.0, 250.0, "YX Front View");
  YX.Draw();

  for (int i = 8; i > 0; --i)
  {
    char buf[128];
    // Draw 3 layers of the much station
    if (i < 7)
    {
      sprintf(buf, "/cave_1/much_0/muchstation0%d_0", i);
      gGeoManager->cd(buf);
      DrawMuch(gGeoManager->GetCurrentNode());
    }
#ifdef USE_MUCH_ABSORBER
    // Draw an absorber
    sprintf(buf, "/cave_1/much_0/muchabsorber0%d_0", i);
    gGeoManager->cd(buf);
    Double_t localCoords[3] = {0., 0., 0.};
    Double_t globalCoords[3];
    gGeoManager->LocalToMaster(localCoords, globalCoords);
    TGeoVolume* muchAbsVol = gGeoManager->GetCurrentVolume();
    TGeoShape* muchAbsShape = muchAbsVol->GetShape();
    TGeoCone* muchAbsCone = dynamic_cast<TGeoCone*> (muchAbsShape);

    Double_t fRmax1 = muchAbsCone->GetRmax1();
    Double_t fRmax2 = muchAbsCone->GetRmax2();
    Double_t fDz = muchAbsCone->GetDz();
    Double_t fZ = globalCoords[2];

    Double_t maXs[5] = { fZ - fDz, fZ - fDz, fZ + fDz, fZ + fDz, fZ - fDz };
    Double_t maYs[5] = { -fRmax1, fRmax1, fRmax2, -fRmax2, -fRmax1 };
    TPolyLine* muchAbsorber = new TPolyLine(5, maXs, maYs);
    muchAbsorber->SetFillColor(kYellow);
    muchAbsorber->SetLineColor(kYellow);
    muchAbsorber->SetLineWidth(1);
    YZ.cd();
    muchAbsorber->Draw("f");
    muchAbsorber->Draw();
    XZ.cd();
    muchAbsorber->Draw("f");
    muchAbsorber->Draw();
    YX.cd();
    TEllipse *ellipse = new TEllipse(0.0, 0.0, fRmax2);
    ellipse->SetFillColor(kYellow);
    ellipse->SetLineColor(kYellow);
    ellipse->SetLineWidth(1);
    ellipse->SetFillStyle(3002);
    ellipse->Draw("f");
    ellipse->Draw();
    TEllipse *ellipseInner = new TEllipse(0.0, 0.0, fRmax1);
    ellipseInner->SetFillColor(kYellow - 7);
    ellipseInner->SetLineColor(kYellow - 7);
    ellipseInner->SetLineWidth(1);
    ellipseInner->SetFillStyle(3002);
    ellipseInner->Draw("f");
    ellipseInner->Draw();
#endif//USE_MUCH_ABSORBER
  }
}

void LxDraw::DrawRays()
{
  LxFinder* finder = LxFinder::Instance();
  LxSpace& caSpace = finder->caSpace;
  int stationsNumber = caSpace.stations.size();

  for (int i = stationsNumber - 1; i > 0; --i)
  {
    LxStation* rSt = caSpace.stations[i];
    LxLayer* rLa = rSt->layers[0];
    LxStation* lSt = caSpace.stations[i - 1];
    int lLaInd = lSt->layers.size() - 1;
    LxLayer* lLa = lSt->layers[lLaInd];

    if (rLa->points.size() == 0 || lLa->points.size() == 0)
      continue;

    Double_t lZ = (*lLa->points.begin())->z;

    for (list<LxPoint*>::iterator j = rLa->points.begin(); j != rLa->points.end(); ++j)
    {
      LxPoint* rPo = *j;
      Double_t deltaZ = lZ - rPo->z;

      for (list<LxRay*>::iterator k = rPo->rays.begin(); k != rPo->rays.end(); ++k)
      {
        LxRay* ray = *k;

        Double_t lX = rPo->x + ray->tx * deltaZ;
        Double_t lY = rPo->y + ray->ty * deltaZ;

        YZ.cd();
        TLine* yzLine = new TLine(rPo->z, rPo->y, lZ, lY);
        yzLine->SetLineColor(kRed);
        yzLine->Draw();

        XZ.cd();
        TLine* xzLine = new TLine(rPo->z, rPo->x, lZ, lX);
        xzLine->SetLineColor(kRed);
        xzLine->Draw();

        YX.cd();
        TLine* yxLine = new TLine(rPo->x, rPo->y, lX, lY);
        yxLine->SetLineColor(kRed);
        yxLine->Draw();
      }
    }
  }

  YZ.cd();
  YZ.Update();
  XZ.cd();
  XZ.Update();
  YX.cd();
  YX.Update();
}

void LxDraw::DrawRecoTracks()
{
  LxFinder* finder = LxFinder::Instance();
  LxSpace& caSpace = finder->caSpace;
  int stationsNumber = caSpace.stations.size();

  for (list<LxTrack*>::iterator i = caSpace.tracks.begin(); i != caSpace.tracks.end(); ++i)
  {
    LxTrack* track = *i;

    for (int j = 0; j < track->length; ++j)
    {
      LxRay* ray = track->rays[j];

      if (0 == ray)
        break;

      Double_t rX = ray->source->x;
      Double_t rY = ray->source->y;
      Double_t rZ = ray->source->z;

      Double_t lZ = caSpace.stations[ray->station->stationNumber - 1]->zCoord;
      Double_t deltaZ = lZ - rZ;
      Double_t lX = rX + ray->tx * deltaZ;
      Double_t lY = rY + ray->ty * deltaZ;

      YZ.cd();
      TLine* yzLineL = new TLine(rZ, rY, lZ, lY);
      yzLineL->SetLineColor(kGreen);
      yzLineL->Draw();

      XZ.cd();
      TLine* xzLineL = new TLine(rZ, rX, lZ, lX);
      xzLineL->SetLineColor(kGreen);
      xzLineL->Draw();

      YX.cd();
      TLine* yxLineL = new TLine(rX, rY, lX, lY);
      yxLineL->SetLineColor(kGreen);
      yxLineL->Draw();
    }

    // Draw a segment of an external track if it is set.

    if (track->externalTrack.track)
    {
      Double_t rZ = caSpace.stations[0]->layers[0]->zCoord;
      const FairTrackParam* param = track->externalTrack.track->GetParamLast();

      Double_t lX = param->GetX();
      Double_t lY = param->GetY();
      Double_t lZ = param->GetZ();
      Double_t deltaZ = rZ - lZ;
      Double_t rX = lX + param->GetTx() * deltaZ;
      Double_t rY = lY + param->GetTy() * deltaZ;

      YZ.cd();
      TLine* yzLine = new TLine(lZ, lY, rZ, rY);
      yzLine->SetLineColor(kRed);
      yzLine->Draw();

      XZ.cd();
      TLine* xzLine = new TLine(lZ, lX, rZ, rX);
      xzLine->SetLineColor(kRed);
      xzLine->Draw();

      YX.cd();
      TLine* yxLine = new TLine(lX, lY, rX, rY);
      yxLine->SetLineColor(kRed);
      yxLine->Draw();
    }
  }

  YZ.cd();
  YZ.Update();
  XZ.cd();
  XZ.Update();
  YX.cd();
  YX.Update();
}

void LxDraw::DrawMCPoints()
{
  int mcolour = 2;
  int hitsMStyle = 2;
  double HitSize = 1;
  LxFinder* finder = LxFinder::Instance();

  int nhits = finder->MCPoints.size();
  Double_t x_poly[nhits], y_poly[nhits], z_poly[nhits];
  TVector3 pos;
  int n_poly = 0;

  for (vector<LxMCPoint>::iterator i = finder->MCPoints.begin(); i != finder->MCPoints.end(); ++i)
  {
    LxMCPoint& point = *i;

    x_poly[n_poly] = point.x;
    y_poly[n_poly] = point.y;
    z_poly[n_poly] = point.z;

    ++n_poly;
  }

  YZ.cd();

  TPolyMarker* pmyz = new TPolyMarker(n_poly, z_poly, y_poly);
  pmyz->SetMarkerColor(mcolour);
  pmyz->SetMarkerStyle(hitsMStyle);
  pmyz->SetMarkerSize(HitSize);
  pmyz->Draw();
  YZ.Update();

  XZ.cd();

  TPolyMarker* pmxz = new TPolyMarker(n_poly, z_poly, x_poly);
  pmxz->SetMarkerColor(mcolour);
  pmxz->SetMarkerStyle(hitsMStyle);
  pmxz->SetMarkerSize(HitSize);
  pmxz->Draw();
  XZ.Update();

  YX.cd();

  TPolyMarker* pmyx = new TPolyMarker(n_poly, x_poly, y_poly);
  pmyx->SetMarkerColor(mcolour);
  pmyx->SetMarkerStyle(hitsMStyle);
  pmyx->SetMarkerSize(HitSize);
  pmyx->Draw();
  YX.Update();
}

void LxDraw::DrawExtTracks()
{
  LxFinder* finder = LxFinder::Instance();
  LxSpace& caSpace = finder->caSpace;
  Double_t rZ = caSpace.stations[0]->layers[0]->zCoord;

  for (list<LxExtTrack>::iterator i = caSpace.extTracks.begin(); i != caSpace.extTracks.end(); ++i)
  {
    LxExtTrack& extTrack = *i;
    const FairTrackParam* param = extTrack.track->GetParamLast();

    Double_t lX = param->GetX();
    Double_t lY = param->GetY();
    Double_t lZ = param->GetZ();
    Double_t deltaZ = rZ - lZ;
    Double_t rX = lX + param->GetTx() * deltaZ;
    Double_t rY = lY + param->GetTy() * deltaZ;

    YZ.cd();
    TLine* yzLine = new TLine(lZ, lY, rZ, rY);
    yzLine->SetLineColor(kRed);
    yzLine->Draw();

    XZ.cd();
    TLine* xzLine = new TLine(lZ, lX, rZ, rX);
    xzLine->SetLineColor(kRed);
    xzLine->Draw();

    YX.cd();
    TLine* yxLine = new TLine(lX, lY, rX, rY);
    yxLine->SetLineColor(kRed);
    yxLine->Draw();
  }

  YZ.cd();
  YZ.Update();
  XZ.cd();
  XZ.Update();
  YX.cd();
  YX.Update();
}

void LxDraw::SaveCanvas(TString name)
{
  system("mkdir LxDraw -p");
  chdir("LxDraw");
  TString tmp = name;
  tmp += "YXView.pdf";
  YX.cd();
  YX.SaveAs(tmp);

  tmp = name;
  tmp += "XZView.pdf";
  XZ.cd();
  XZ.SaveAs(tmp);

  tmp = name;
  tmp += "YZView.pdf";
  YZ.cd();
  YZ.SaveAs(tmp);

  chdir("..");
}
