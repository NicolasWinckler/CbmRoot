/*  CbmRichRingFitterEllipse
    Description: This is the implementation of ellipse fitting
    using MINUIT.

    Author : Simeon Lebedev and Gennady Ososkov
    E-mail : S.Lebedev@gsi.de
*/

#include "CbmRichRingFitterEllipse.h"

#include "CbmRichRing.h"
#include "CbmRichHit.h"

#include "CbmRootManager.h"

///needed for radius correction
#include "TH1D.h" 
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"

#include <vector>

using std::vector;
using std::endl;
using std::cout;

// -----   Default constructor   -------------------------------------------
CbmRichRingFitterEllipse::CbmRichRingFitterEllipse()
{
    fVerbose      = 1;
    fCorrection   = 1;
    fFieldName = "muon";
    InitHistForRadiusCorrection();
}

// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmRichRingFitterEllipse::CbmRichRingFitterEllipse(Int_t verbose,Double_t correction, TString fieldName)
{
    fVerbose = verbose;
    fCorrection   = correction;
    fFieldName = fieldName;  
    InitHistForRadiusCorrection();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmRichRingFitterEllipse::~CbmRichRingFitterEllipse()
{

}
// -------------------------------------------------------------------------
void CbmRichRingFitterEllipse::DoFit(CbmRichRing *pRing)
{
    Int_t nofHits=pRing->GetNofHits();
    
    if (nofHits <= 5) {
        pRing->SetXYABPhi(-1.,-1.,-1., -1., -1.);    
        pRing->SetRadius(-1.);      
        return;
    }
    
    CbmRichHit* hit = NULL;
    std::vector<Double_t> hitX;
    std::vector<Double_t> hitY;
    hitX.clear();
    hitY.clear();
    for(Int_t i = 0; i < nofHits; i++) {
        hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(i));
        hitX.push_back(hit->X());
        hitY.push_back(hit->Y());
    }
      
    std::vector<Double_t> fpar;
    fpar = DoFit1(hitX, hitY);
    
    ///calculate center of the ellipse
    Double_t xc = (fpar[0] + fpar[2])/2.;
    Double_t yc = (fpar[1] + fpar[3])/2.;
    ///calculate length of b axes
    Double_t p1 = (fpar[0] - fpar[2])*(fpar[0] - fpar[2]) + (fpar[1] - fpar[3])*(fpar[1] - fpar[3]);
    if (p1 < 0.){
        pRing->SetXYABPhi(-1.,-1.,-1., -1., -1.);    
        pRing->SetRadius(-1.);      
        return;    
    }

    Double_t c = sqrt ( p1 ) / 2.;
    Double_t p2 = fpar[4]*fpar[4] - c*c;
    if (p2 < 0.){
        pRing->SetXYABPhi(-1.,-1.,-1., -1., -1.);    
        pRing->SetRadius(-1.);      
        return;    
    }    
    Double_t b = sqrt ( p2 );
    ///calculate angle
    Double_t p3 = fpar[2] - fpar[0];
    if (p3 == 0.){
        pRing->SetXYABPhi(-1.,-1.,-1., -1., -1.);    
        pRing->SetRadius(-1.);      
        return;    
    }      
    Double_t k = (fpar[3] - fpar[1]) / (fpar[2] - fpar[0]);
    Double_t ang = atan(k);
    
    pRing->SetXYABPhi(xc,yc,fpar[4], b, ang);    
    pRing->SetRadius((b + fpar[4])/2.);        
    
    ///Make radius correction if it's needed
    if (fCorrection == 1 )MakeRadiusCorrection(pRing);
    
    CalcChi2(pRing);    
}


// -----   Public method DoFit   ------------------------------------------
std::vector<Double_t> CbmRichRingFitterEllipse::DoFit1(std::vector<Double_t> x,
                                     std::vector<Double_t> y)
{
    FCNEllipse *theFCN = new FCNEllipse(x, y);
    // create initial starting values for parameters
    Double_t xf1 = 0.;
    Double_t yf1 = 0.;
    Double_t xf2 = 0.;
    Double_t yf2 = 0.;
    Double_t a = 0.;
    
    for(UInt_t i = 0; i < x.size(); i++) {
        xf1 += x[i];
        yf1 += y[i];
       
    }
    a = 5.;    
    xf1 = xf1 / x.size() - a;
    yf1 = yf1 / x.size();
    xf2 = xf1 + a;
    yf2 = yf1;
    
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
    
   // if (theFCN) delete theFCN;
    
    return fpar;
}

void CbmRichRingFitterEllipse::CalcChi2(CbmRichRing * pRing)
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
    Double_t c = sqrt ( axisA*axisA - axisB*axisB); 
      
    if (phi > 0.){
        xf1 = centerX + c*cos(phi);
        yf1 = centerY + c*sin(phi); 
        xf2 = centerX - c*cos(phi); 
        yf2 = centerY - c*sin(phi); 
    }else{
        xf1 = centerX + c*cos(TMath::Abs(phi)); 
        yf1 = centerY - c*sin(TMath::Abs(phi)); 
        xf2 = centerX - c*cos(TMath::Abs(phi)); 
        yf2 = centerY + c*sin(TMath::Abs(phi));     
    }

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

void CbmRichRingFitterEllipse::InitHistForRadiusCorrection()
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

    TFile *file = new TFile(fileName, "READ");
    if (!file || !file->IsOpen()) {
	cout << " -E- Read correction maps "<<endl;
	cout << " -E- Could not open input file." <<fileName<< endl;
	return;
    } else {
	cout <<" -I- Map Correction input file: "<< fileName << endl;
    }

    gROOT->cd();

    fh_mapaxisAXY =  (TH1D*) file->Get("fh_mapaxisAXY")->Clone();
    fh_mapaxisBXY =  (TH1D*) file->Get("fh_mapaxisBXY")->Clone();

    file->Close();
    delete file;
}

void CbmRichRingFitterEllipse::MakeRadiusCorrection(CbmRichRing * pRing)
{
    Double_t centerX = pRing->GetCenterX();
    Double_t centerY = pRing->GetCenterY();  
   // cout << "Before " <<   pRing->GetAaxis()<< " " <<pRing->GetBaxis()<< endl;
    Double_t axisA = pRing->GetAaxis() + fh_mapaxisAXY->GetBinContent(fh_mapaxisAXY->FindBin(centerX,centerY));
    Double_t axisB = pRing->GetBaxis() + fh_mapaxisBXY->GetBinContent(fh_mapaxisBXY->FindBin(centerX,centerY));
    
    pRing->SetAaxis(axisA);   
    pRing->SetBaxis(axisB); 
   // cout << "After " <<   pRing->GetAaxis()<< " " <<pRing->GetBaxis()<< endl;
       

}


ClassImp(CbmRichRingFitterEllipse)
