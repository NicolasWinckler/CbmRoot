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
    fFieldName = "compact";
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
	fX.assign(hitX.begin(), hitX.end());
	fY.assign(hitY.begin(), hitY.end());

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

	//CalcChi2(pRing);
}

// -----   Public method: Taubin   -----------------------------------------
void CbmRichRingFitterEllipseTau::Taubin()
{
	Double_t det;
	TMatrixD PQ(5,5); // fPQ = P^(-1) * Q
	PQ = fP.InvertFast(&det)*fQ;
	TMatrixDEigen eig(PQ);
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
	fM.Clear();
	fP.Clear();
	fQ.Clear();
	fAlgPar.clear();

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
	CalcChi2(Pxx, Pxy, Pyy, Px, Py, P, pRing);
	pRing->SetAPar(Pxx);
	pRing->SetBPar(Pxy);
	pRing->SetCPar(Pyy);
	pRing->SetDPar(Px);
	pRing->SetEPar(Py);
	pRing->SetFPar(P);

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

ClassImp(CbmRichRingFitterEllipseTau)
