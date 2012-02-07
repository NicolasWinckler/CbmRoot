/**
* \file CbmRichRingFitterEllipse.cxx
*
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/

#include "CbmRichRingFitterEllipse.h"

#include "CbmRichRing.h"
#include "CbmRichHit.h"

#include "FairRootManager.h"

///needed for radius correction
#include "TH1D.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TDirectory.h"

#include <vector>

using std::vector;
using std::endl;
using std::cout;

CbmRichRingFitterEllipse::CbmRichRingFitterEllipse()
{
   InitHistForRadiusCorrection();
}

CbmRichRingFitterEllipse::~CbmRichRingFitterEllipse()
{

}

void CbmRichRingFitterEllipse::DoFit(
      CbmRichRing *ring,
      const vector<Double_t>& x,
      const vector<Double_t>& y)
{
   vector<Double_t> fpar = DoFit(x, y);

   TransformToRichRing(ring, fpar);
}

void CbmRichRingFitterEllipse::DoFit(
      CbmRichRing *ring)
{
   Int_t nofHits=ring->GetNofHits();

   if (nofHits <= 5) {
      ring->SetXYABPhi(-1.,-1.,-1., -1., -1.);
      ring->SetRadius(-1.);
      return;
   }

   CbmRichHit* hit = NULL;
   vector<Double_t> hitX;
   vector<Double_t> hitY;
   hitX.clear();
   hitY.clear();
   for(Int_t i = 0; i < nofHits; i++) {
      hit = (CbmRichHit*)fHitsArray->At(ring->GetHit(i));
      hitX.push_back(hit->GetX());
      hitY.push_back(hit->GetY());
   }

   vector<Double_t> fpar = DoFit(hitX, hitY);

   TransformToRichRing(ring, fpar);

   // Perform radius correction.
   // Corection parameters are stored in fAaxisCor and fBaxisCor.
   PerformRadiusCorrection(ring);

   CalcChi2(ring);
}

void CbmRichRingFitterEllipse::TransformToRichRing(
      CbmRichRing* ring,
      const vector<Double_t>& fpar)
{
   ///calculate center of the ellipse
   Double_t xc = (fpar[0] + fpar[2])/2.;
   Double_t yc = (fpar[1] + fpar[3])/2.;
   ///calculate length of b axes
   Double_t p1 = (fpar[0] - fpar[2])*(fpar[0] - fpar[2]) + (fpar[1] - fpar[3])*(fpar[1] - fpar[3]);
   if (p1 < 0.){
      ring->SetXYABPhi(-1.,-1.,-1., -1., -1.);
      ring->SetRadius(-1.);
      return;
   }

   Double_t c = sqrt ( p1 ) / 2.;
   Double_t p2 = fpar[4]*fpar[4] - c*c;
   if (p2 < 0.){
      ring->SetXYABPhi(-1.,-1.,-1., -1., -1.);
      ring->SetRadius(-1.);
      return;
   }
   Double_t b = sqrt ( p2 );
   ///calculate angle
   Double_t p3 = fpar[2] - fpar[0];
   if (p3 == 0.){
      ring->SetXYABPhi(-1.,-1.,-1., -1., -1.);
      ring->SetRadius(-1.);
      return;
   }
   Double_t k = (fpar[3] - fpar[1]) / (fpar[2] - fpar[0]);
   Double_t ang = atan(k);

   ring->SetXYABPhi(xc,yc,fpar[4], b, ang);
   ring->SetRadius((b + fpar[4])/2.);
}

vector<Double_t> CbmRichRingFitterEllipse::DoFit(
      const vector<Double_t>& x,
      const vector<Double_t>& y)
{
   FCNEllipse *theFCN = new FCNEllipse(x, y);

   // create initial starting values for parameters
   Double_t xf1 = 0.;
   Double_t yf1 = 0.;
   for(UInt_t i = 0; i < x.size(); i++) {
      xf1 += x[i];
      yf1 += y[i];
   }
   Double_t a = 5.;
   xf1 = xf1 / x.size() - a;
   yf1 = yf1 / x.size();
   Double_t xf2 = xf1 + a;
   Double_t yf2 = yf1;

   TFitterMinuit theMinuit;
   theMinuit.SetPrintLevel(-1);
   theMinuit.SetMinuitFCN(theFCN);
   theMinuit.SetParameter(0, "xf1", xf1, 0.1, 1., -1.);
   theMinuit.SetParameter(1, "yf1", yf1, 0.1, 1., -1.);
   theMinuit.SetParameter(2, "xf2", xf2, 0.1, 1., -1.);
   theMinuit.SetParameter(3, "yf2", yf2, 0.1, 1., -1.);
   theMinuit.SetParameter(4, "a", a, 0.1, 1., -1.);
   theMinuit.CreateMinimizer();
   theMinuit.Minimize();
   vector<Double_t> fpar;
   fpar.clear();
   for (Int_t i = 0; i < 5; i++){
      fpar.push_back(theMinuit.GetParameter(i));
   }
   return fpar;
}

ClassImp(CbmRichRingFitterEllipse)
