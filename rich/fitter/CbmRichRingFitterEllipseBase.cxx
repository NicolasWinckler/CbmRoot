/**
* \file CbmRichRingFitterEllipseBase.cxx
*
*
* \author Semen Lebedev <s.lebedev@gsi.de>
* \date 2011
**/

#include "CbmRichRingFitterEllipseBase.h"
#include "CbmRichRing.h"
#include "CbmRichHit.h"

#include "TSystem.h"
#include "TROOT.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH2D.h"

#include <cmath>
#include <string>
#include <iostream>
using std::cout;
using std::endl;
using std::string;

void CbmRichRingFitterEllipseBase::CalcChi2(
      CbmRichRing* ring)
{
   Int_t nofHits =  ring->GetNofHits();
   if (nofHits <= 5){
      ring->SetChi2(-1.);
      return;
   }

   Double_t axisA = ring->GetAaxis();
   Double_t axisB = ring->GetBaxis();
   Double_t phi = ring->GetPhi();
   Double_t centerX = ring->GetCenterX();
   Double_t centerY = ring->GetCenterY();

   // calculate ellipse focuses
   Double_t xf1, yf1, xf2, yf2;

   if (axisA < axisB){
      ring->SetChi2(-1.);
      return;
   }

   xf1 = ring->GetXF1();
   yf1 = ring->GetYF1();
   xf2 = ring->GetXF2();
   yf2 = ring->GetYF2();

   // calculate chi2
   Double_t chi2 = 0.;
   for(Int_t iHit = 0; iHit < nofHits; iHit++){
     CbmRichHit* hit = (CbmRichHit*) fHitsArray->At(ring->GetHit(iHit));
     if(NULL == hit) continue;
     Double_t x = hit->GetX();
     Double_t y = hit->GetY();

     Double_t d1 = sqrt( (x-xf1)*(x-xf1) + (y-yf1)*(y-yf1) );
     Double_t d2 = sqrt( (x-xf2)*(x-xf2) + (y-yf2)*(y-yf2) );

     chi2 += (d1 + d2 - 2.*axisA)*(d1 + d2 - 2.*axisA);
   }
   chi2 = chi2 / (nofHits - 5);

   ring->SetChi2(chi2);
}

void CbmRichRingFitterEllipseBase::CalcChi2(
      Double_t A,
      Double_t B,
      Double_t C,
		Double_t D,
		Double_t E,
		Double_t F,
		CbmRichRing* ring)
{
   Int_t nofHits =  ring->GetNofHits();
   if (nofHits <= 5){
      ring->SetChi2(-1.);
      return;
   }

   // calculate chi2
   Double_t chi2 = 0.;
   for(Int_t iHit = 0; iHit < nofHits; iHit++){
      CbmRichHit* hit = (CbmRichHit*) fHitsArray->At(ring->GetHit(iHit));
      if(NULL == hit) continue;
      Double_t x = hit->GetX();
      Double_t y = hit->GetY();

      Double_t d1 = TMath::Abs(A*x*x + B*x*y + C*y*y + D*x + E*y + F);
      Double_t d2 = sqrt( pow(2*A*x + B*y + D, 2) + pow(B*x + 2*C*y + E, 2) );

      chi2 += (d1*d1)/(d2*d2);
   }

   ring->SetChi2(chi2);
}


void CbmRichRingFitterEllipseBase::InitHistForRadiusCorrection()
{
   string fileName = gSystem->Getenv("VMCWORKDIR");
   fileName += "/parameters/rich/radius_correction_map_compact.root";

   TDirectory *current = gDirectory;
   TFile *file = new TFile(fileName.c_str(), "READ");

   if (NULL == file || !file->IsOpen()) {
      cout << " -E- Read correction maps "<<endl;
      cout << " -E- Could not open input file " <<fileName<< endl;
      return;
   } else {
      cout <<" -I- Map Correction input file: "<< fileName << endl;
   }

   gROOT->cd();

   fh_mapaxisAXY = (TH2D*) file->Get("fh_mapaxisAXY")->Clone();
   fh_mapaxisBXY = (TH2D*) file->Get("fh_mapaxisBXY")->Clone();

   file->Close();
   delete file;
   current->cd();
}

void CbmRichRingFitterEllipseBase::PerformRadiusCorrection(
      CbmRichRing * ring)
{
   Double_t centerX = ring->GetCenterX();
   Double_t centerY = ring->GetCenterY();
   Double_t axisA = ring->GetAaxis() + fh_mapaxisAXY->GetBinContent(fh_mapaxisAXY->FindBin(centerX,centerY));
   Double_t axisB = ring->GetBaxis() + fh_mapaxisBXY->GetBinContent(fh_mapaxisBXY->FindBin(centerX,centerY));

   ring->SetAaxisCor(axisA);
   ring->SetBaxisCor(axisB);
}

ClassImp(CbmRichRingFitterEllipseBase)
