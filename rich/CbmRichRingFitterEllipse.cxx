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
#include "TDirectory.h"

#include <vector>

using std::vector;
using std::endl;
using std::cout;

// -----   Default constructor   -------------------------------------------
CbmRichRingFitterEllipse::CbmRichRingFitterEllipse()
{
    fVerbose      = 1;
    fFieldName = "muon";
    InitHistForRadiusCorrection();
}

// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmRichRingFitterEllipse::CbmRichRingFitterEllipse(Int_t verbose,Int_t correction, TString fieldName)
{
    fVerbose = verbose;
    fFieldName = fieldName;
    InitHistForRadiusCorrection();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmRichRingFitterEllipse::~CbmRichRingFitterEllipse()
{

}

void CbmRichRingFitterEllipse::DoFit1(CbmRichRing *pRing, std::vector<Double_t> x,
                                     std::vector<Double_t> y)
{

    std::vector<Double_t> fpar;
    fpar = DoFit1(x, y);

    TransformToRichRing(pRing, fpar);
}

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

    TransformToRichRing(pRing, fpar);

    ///Make radius correction
    //Corection parameters are stored in fAaxisCor and fBaxisCor !!!!
    MakeRadiusCorrection(pRing);

    CalcChi2(pRing);
}

void CbmRichRingFitterEllipse::TransformToRichRing(CbmRichRing* pRing, vector<Double_t> fpar)
{

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

    return fpar;
}

ClassImp(CbmRichRingFitterEllipse)
