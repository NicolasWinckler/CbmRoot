#include "CbmRichRingFitterEllipseBase.h"
#include "CbmRichRing.h"

#include "TSystem.h"
#include "TROOT.h"
#include "TDirectory.h"
#include "TFile.h"

#include <cmath>
#include <iostream>
using std::cout;
using std::endl;

// -----   Protected method CalcChi2   ----------------------------------------
void CbmRichRingFitterEllipseBase::CalcChi2(CbmRichRing* pRing)
{
    CbmRichHit* hit = NULL;

///ring parameters
    Int_t nofHits =  pRing->GetNofHits();
    if (nofHits <= 5){
        pRing->SetChi2(-1.);
        return;
    }

    Double_t axisA =  pRing->GetAaxis();
    Double_t axisB =  pRing->GetBaxis();
    Double_t phi =  pRing->GetPhi();
    Double_t centerX = pRing->GetCenterX();
    Double_t centerY = pRing->GetCenterY();

///calculate ellipse focuses
    Double_t xf1, yf1, xf2, yf2;

    if (axisA < axisB){
        pRing->SetChi2(-1.);
        return;
    }

    xf1 = pRing->GetXF1();
    yf1 = pRing->GetYF1();
    xf2 = pRing->GetXF2();
    yf2 = pRing->GetYF2();


///calculate chi2
    Double_t chi2 = 0.;
    for(Int_t iHit = 0; iHit < nofHits; iHit++){
        hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(iHit));
        if(!hit) continue;
        Double_t x = hit->GetX();
        Double_t y = hit->GetY();

        Double_t d1 = sqrt( (x-xf1)*(x-xf1) + (y-yf1)*(y-yf1) );
        Double_t d2 = sqrt( (x-xf2)*(x-xf2) + (y-yf2)*(y-yf2) );

        chi2 += (d1 + d2 - 2.*axisA)*(d1 + d2 - 2.*axisA);
    }
    chi2 = chi2 / (nofHits - 5);

    pRing->SetChi2(chi2);
}

//
void CbmRichRingFitterEllipseBase::CalcChi2(Double_t A, Double_t B, Double_t C,
		Double_t D, Double_t E, Double_t F, CbmRichRing* pRing)
{
    CbmRichHit* hit = NULL;

///ring parameters
    Int_t nofHits =  pRing->GetNofHits();
    if (nofHits <= 5){
        pRing->SetChi2(-1.);
        return;
    }

///calculate chi2
    Double_t chi2 = 0.;
    for(Int_t iHit = 0; iHit < nofHits; iHit++){
        hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(iHit));
        if(!hit) continue;
        Double_t x = hit->GetX();
        Double_t y = hit->GetY();

        Double_t d1 = TMath::Abs(A*x*x + B*x*y + C*y*y + D*x + E*y + F);
        Double_t d2 = sqrt( pow(2*A*x + B*y + D, 2) + pow(B*x + 2*C*y + E, 2) );

        chi2 += (d1*d1)/(d2*d2);
    }

    chi2 = chi2 / (nofHits - 5);

    pRing->SetChi2(chi2);
}


void CbmRichRingFitterEllipseBase::InitHistForRadiusCorrection()
{
    TString fileName;
    if (fFieldName == "muon"){
        fileName = gSystem->Getenv("VMCWORKDIR");
        fileName += "/parameters/rich/muon_radius_correction_map.root";
    }
    if(fFieldName == "active"){
        fileName = gSystem->Getenv("VMCWORKDIR");
        fileName += "/parameters/rich/active_radius_correction_map.root";
    }
    if (fFieldName != "active" || fFieldName != "muon"){
        fileName = gSystem->Getenv("VMCWORKDIR");
        fileName += "/parameters/rich/muon_radius_correction_map.root";
    }
    TDirectory *current = gDirectory;
    TFile *file = new TFile(fileName, "READ");

    if (!file || !file->IsOpen()) {
    	cout << " -E- Read correction maps "<<endl;
    	cout << " -E- Could not open input file " <<fileName<< endl;
	return;
    } else {
    	cout <<" -I- Map Correction input file: "<< fileName << endl;
    }

    gROOT->cd();

    fh_mapaxisAXY =  (TH2D*) file->Get("fh_mapaxisAXY")->Clone();
    fh_mapaxisBXY =  (TH2D*) file->Get("fh_mapaxisBXY")->Clone();

    file->Close();
    delete file;
    current->cd();
}

void CbmRichRingFitterEllipseBase::MakeRadiusCorrection(CbmRichRing * pRing)
{
    Double_t centerX = pRing->GetCenterX();
    Double_t centerY = pRing->GetCenterY();
    Double_t axisA = pRing->GetAaxis() + fh_mapaxisAXY->GetBinContent(fh_mapaxisAXY->FindBin(centerX,centerY));
    Double_t axisB = pRing->GetBaxis() + fh_mapaxisBXY->GetBinContent(fh_mapaxisBXY->FindBin(centerX,centerY));

    pRing->SetAaxisCor(axisA);
    pRing->SetBaxisCor(axisB);
}


ClassImp(CbmRichRingFitterEllipseBase)
