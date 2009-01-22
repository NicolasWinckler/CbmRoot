/**********************************************************************************
*    Class: CbmRichRingFitterEllipseTau                                           *
*    Description: This is the source of a particular ellipse fitting              *
*                 Here the ring is fitted with Taubin algorithm from              *
*                 Alexander Ayriyan, G. Ososkov and N. Chernov                    *
*                                                                                 *
*    Author : Alexander Ayriyan and Semen Lebedev                                *
*    E-mail : A.Ayriyan@gsi.de                                                    *
**********************************************************************************/

#include "CbmRichRingFitterEllipseTau.h"
#include "CbmRootManager.h"
#include "CbmRichRing.h"
#include "CbmRichHit.h"

// The following classes are
// needed for radius correction
#include "TH1D.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"

//using std::vector;
//using std::endl;
//using std::cout;

// -----   Default constructor   -------------------------------------------
CbmRichRingFitterEllipseTau::CbmRichRingFitterEllipseTau()
{
    fVerbose      = 1;
    fFieldName = "muon";
    InitHistForRadiusCorrection();
}

// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmRichRingFitterEllipseTau::CbmRichRingFitterEllipseTau(Int_t verbose,Double_t correction, TString fieldName)
{
    fVerbose = verbose;
    fFieldName = fieldName;
    InitHistForRadiusCorrection();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmRichRingFitterEllipseTau::~CbmRichRingFitterEllipseTau()
{
    fX.clear();
    fY.clear();
}

void CbmRichRingFitterEllipseTau::DoFit1(CbmRichRing *pRing, vector<Double_t> hitX,
		vector<Double_t> hitY)
{
	fX = hitX;
	fY = hitY;

	InitMatrices();
	Taubin();
	TransEllipse(pRing);
}

// -----   Public method: DoFit   ------------------------------------------
void CbmRichRingFitterEllipseTau::DoFit(CbmRichRing *pRing)
{
    Int_t fNumHits = pRing->GetNofHits();

	if (fNumHits <= 5) {
		pRing->SetXYABPhi(-1., -1., -1., -1., -1.);
		pRing->SetRadius(-1.);

		if (fVerbose > 1)
			cout <<" No way for RingFitter as ellipse: Number of Hits is less then 5 "
					<<endl;
		return;
	}
    fX.clear();
    fY.clear();
	CbmRichHit* hit= NULL;
	for (Int_t i = 0; i < fNumHits; i++) {
		hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(i));
		fX.push_back(hit->X());
		fY.push_back(hit->Y());
	}

	InitMatrices();
	Taubin();
	TransEllipse(pRing);

	MakeRadiusCorrection(pRing);

	CalcChi2(pRing);
}

// -----   Public method: Taubin   -----------------------------------------
void CbmRichRingFitterEllipseTau::Taubin()
{
	Double_t det;
	TMatrixD fPQ(5,5); // fPQ = P^(-1) * Q
	fPQ = fP.InvertFast(&det)*fQ;
	TMatrixDEigen eig(fPQ);
	TMatrixD evc = eig.GetEigenVectors();

	Double_t AlgParF = 0.;
	for(Int_t iPar = 0; iPar < 5; iPar++){
		AlgParF -= evc(iPar,0) * fM (iPar,5);
		fAlgPar.push_back( evc(iPar,0) );
	}
	fAlgPar.push_back(AlgParF);
}


// -----   Public method: InitMatrices   -----------------------------------
void CbmRichRingFitterEllipseTau::InitMatrices()
{
	UInt_t numHits = fX.size();

	TMatrixD Z(numHits,6);
	TMatrixD ZT(6,numHits);
	for(Int_t i = 0; i < numHits; i++){
		Z(i,0) = ZT(0,i) = fX[i]*fX[i];
		Z(i,1) = ZT(1,i) = fX[i]*fY[i];
		Z(i,2) = ZT(2,i) = fY[i]*fY[i];
		Z(i,3) = ZT(3,i) = fX[i];
		Z(i,4) = ZT(4,i) = fY[i];
		Z(i,5) = ZT(5,i) = 1.;
	}
	fM.ResizeTo(6,6);
	fM = ZT*Z;

	fM = fM * (1./ numHits);

	fP.ResizeTo(5,5);
	fP(0,0) = fP(0,0) = fM(0,0) - fM(0,5) * fM(0,5);
	fP(0,1) = fP(1,0) = fM(0,1) - fM(0,5) * fM(1,5);
	fP(0,2) = fP(2,0) = fM(0,2) - fM(0,5) * fM(2,5);
	fP(0,3) = fP(3,0) = fM(0,3) - fM(0,5) * fM(3,5);
	fP(0,4) = fP(4,0) = fM(0,4) - fM(0,5) * fM(4,5);

	fP(1,1) = fP(1,1) = fM(1,1) - fM(1,5) * fM(1,5);
	fP(1,2) = fP(2,1) = fM(1,2) - fM(1,5) * fM(2,5);
	fP(1,3) = fP(3,1) = fM(1,3) - fM(1,5) * fM(3,5);
	fP(1,4) = fP(4,1) = fM(1,4) - fM(1,5) * fM(4,5);

	fP(2,2) = fP(2,2) = fM(2,2) - fM(2,5) * fM(2,5);
	fP(2,3) = fP(3,2) = fM(2,3) - fM(2,5) * fM(3,5);
	fP(2,4) = fP(4,2) = fM(2,4) - fM(2,5) * fM(4,5);

	fP(3,3) = fP(3,3) = fM(3,3) - fM(3,5) * fM(3,5);
	fP(3,4) = fP(4,3) = fM(3,4) - fM(3,5) * fM(4,5);

	fP(4,4) = fP(4,4) = fM(4,4) - fM(4,5) * fM(4,5);

	fQ.ResizeTo(5,5);
	fQ.Zero();
	fQ(0,0)           = 4 * fM(5,0);
	fQ(0,1) = fQ(1,0) = 2 * fM(5,1);
	fQ(0,3) = fQ(3,0) = 2 * fM(5,3);
	fQ(1,1)           = fM(5,0)+fM(5,2);
	fQ(1,2) = fQ(2,1) = 2 * fM(5,1);
	fQ(1,3) = fQ(3,1) = fM(5,4);
	fQ(1,4) = fQ(4,1) = fM(5,3);
	fQ(2,2)           = 4 * fM(5,2);
	fQ(2,4) = fQ(4,2) = 2 * fM(5,4);
	fQ(3,3) = fQ(4,4) = 1.;

}

// -----   Public method: TransposeEllipse   -------------------------------
void CbmRichRingFitterEllipseTau::TransEllipse(CbmRichRing *pRing)
{
	Double_t Pxx = fAlgPar[0];
	Double_t Pxy = fAlgPar[1];
	Double_t Pyy = fAlgPar[2];
	Double_t Px = fAlgPar[3];
	Double_t Py = fAlgPar[4];
	Double_t P = fAlgPar[5];

	Double_t alpha;
	Double_t QQx, QQy, Qxx, Qyy, Qx, Qy, Q;
	Double_t cosa, sina, cca, ssa, sin2a;
	Double_t xc, yc;
	if (fabs(Pxx - Pyy) > 0.1e-10) {
		alpha = atan(Pxy / (Pxx - Pyy));
		alpha = alpha / 2.0;
	} else
		alpha = 1.57079633;

	cosa = cos(alpha);
	sina = sin(alpha);
	cca = cosa * cosa;
	ssa = sina * sina;
	sin2a = sin(2. * alpha);
	Pxy = Pxy * sin2a / 2.;
	Qx = Px * cosa + Py * sina;
	Qy = -Px * sina + Py * cosa;
	QQx = Qx * Qx;
	QQy = Qy * Qy;
	Qxx = 1. / (Pxx * cca + Pxy + Pyy * ssa);
	Qyy = 1. / (Pxx * ssa - Pxy + Pyy * cca);
	Q = -P + Qxx * QQx / 4. + Qyy * QQy / 4.;
	xc = Qx * Qxx;
	yc = Qy * Qyy;

	Double_t axisA = TMath::Sqrt(Q * Qxx);
	Double_t axisB = TMath::Sqrt(Q * Qyy);
	Double_t centerX = -xc * cosa / 2. + yc * sina / 2.;
	Double_t centerY = -xc * sina / 2. - yc * cosa / 2.;

	pRing->SetXYABPhi(centerX, centerY, axisA, axisB, alpha);

	if (pRing->GetAaxis() < pRing->GetBaxis()) {

		Double_t tmp = pRing->GetAaxis();
		pRing->SetAaxis(pRing->GetBaxis());
		pRing->SetBaxis(tmp);

		tmp = pRing->GetPhi();
		if (pRing->GetPhi() <= 0)
			pRing->SetPhi(pRing->GetPhi() + 1.57079633);
		else
			pRing->SetPhi(pRing->GetPhi() - 1.57079633);
	}
}

// -----   Protected method: CalcChi2   ------------------------------------
void CbmRichRingFitterEllipseTau::CalcChi2(CbmRichRing * pRing)
{
    CbmRichHit* hit = NULL;

// Getting Ring parameters
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

// Calculate ellipse focuses
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

// Calculate chi2
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

// -----   Protected method: InitHistForRadiusCorrection   -----------------
void CbmRichRingFitterEllipseTau::InitHistForRadiusCorrection()
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

// -----   Protected method: MakeRadiusCorrection   ------------------------
void CbmRichRingFitterEllipseTau::MakeRadiusCorrection(CbmRichRing * pRing)
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

ClassImp(CbmRichRingFitterEllipseTau)
