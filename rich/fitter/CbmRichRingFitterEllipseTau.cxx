/**********************************************************************************
*    Class: CbmRichRingFitterEllipseTau                                           *
*    Description: This is the source of a particular ellipse fitting              *
*                 Here the ring is fitted with Taubin algorithm from              *
*                 Alexander Ayriyan, G. Ososkov and N. Chernov                    *
*                                                                                 *
*    Author : Alexander Ayriyan and Semen Lebedev                                *
*    E-mail : A.Ayriyan@gsi.de                                                    *
**********************************************************************************/

// GMij are indices for a 5x5 matrix.
#define GM00 0
#define GM01 1
#define GM02 2
#define GM03 3
#define GM04 4

#define GM10 5
#define GM11 6
#define GM12 7
#define GM13 8
#define GM14 9

#define GM20 10
#define GM21 11
#define GM22 12
#define GM23 13
#define GM24 14

#define GM30 15
#define GM31 16
#define GM32 17
#define GM33 18
#define GM34 19

#define GM40 20
#define GM41 21
#define GM42 22
#define GM43 23
#define GM44 24


// Aij are indices for a 6x6 matrix.

#define GA00 0
#define GA01 1
#define GA02 2
#define GA03 3
#define GA04 4
#define GA05 5

#define GA10 6
#define GA11 7
#define GA12 8
#define GA13 9
#define GA14 10
#define GA15 11

#define GA20 12
#define GA21 13
#define GA22 14
#define GA23 15
#define GA24 16
#define GA25 17

#define GA30 18
#define GA31 19
#define GA32 20
#define GA33 21
#define GA34 22
#define GA35 23

#define GA40 24
#define GA41 25
#define GA42 26
#define GA43 27
#define GA44 28
#define GA45 29

#define GA50 30
#define GA51 31
#define GA52 32
#define GA53 33
#define GA54 34
#define GA55 35

#include "CbmRichRingFitterEllipseTau.h"
#include "CbmRichRing.h"
#include "CbmRichHit.h"

// The following classes are
// needed for radius correction
#include "TH1D.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"

#include <cmath>

//using std::vector;
//using std::endl;
//using std::cout;

// -----   Default constructor   -------------------------------------------
CbmRichRingFitterEllipseTau::CbmRichRingFitterEllipseTau()
{
    fVerbose = 1;
    fFieldName = "compact";
    InitHistForRadiusCorrection();
}

// -------------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
CbmRichRingFitterEllipseTau::CbmRichRingFitterEllipseTau(
      Int_t verbose,
      Double_t correction,
      TString fieldName)
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

void CbmRichRingFitterEllipseTau::DoFit1(
      CbmRichRing *pRing,
      const vector<Double_t>& hitX,
		const vector<Double_t>& hitY)
{
   if (hitX.size() > MAX_NOF_HITS){
      cout <<endl << endl<<  "-E- MAX_NOF_HITS = " << MAX_NOF_HITS <<
            ". Please set limits in CbmRichRingFitterEllipseTau.h file." << endl << endl;
   }
   fX.clear();
   fY.clear();
	//fX.assign(hitX.begin(), hitX.end());
	//fY.assign(hitY.begin(), hitY.end());

	for (Int_t i = 0; i < hitX.size(); i++){
	   fX.push_back(hitX[i]);
	   fY.push_back(hitY[i]);
	}
	InitMatrices();
	Taubin();
	TransEllipse(pRing);
}

// -----   Public method: DoFit   ------------------------------------------
void CbmRichRingFitterEllipseTau::DoFit(
      CbmRichRing *pRing)
{
    Int_t fNumHits = pRing->GetNofHits();

	if (fNumHits <= 5) {
		pRing->SetXYABPhi(-1., -1., -1., -1., -1.);
		pRing->SetRadius(-1.);

		if (fVerbose > 1)
			cout <<" No way for RingFitter as ellipse: Number of Hits is less then 5 "	<<endl;
		   return;
	}
    fX.clear();
    fY.clear();
	CbmRichHit* hit= NULL;
	for (Int_t i = 0; i < fNumHits; i++) {
		hit = (CbmRichHit*)fHitsArray->At(pRing->GetHit(i));
		fX.push_back(hit->GetX());
		fY.push_back(hit->GetY());
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
//	TMatrixD PQ(5,5); // fPQ = P^(-1) * Q

	Inv5x5();
	Double_t PQa[25];
	AMultB(fP, 25, 5, fQ, 25, 5, PQa); //PQ = fP*fQ;
	TMatrixD PQ(5,5,PQa);

//	Double_t PQa2[5][5];
//	Double_t d[5];
//	Double_t v[5][5];
//	for(Int_t i = 0; i<5; i++){
//		for (Int_t j = 0; j<5;j++){
//			PQa2[i][j] = PQa[5*i+j];
//			cout << PQa2[i][j] << "  ";
//		}
//		cout << endl;
//	}
//	cout << endl << endl;
//
//	Jacobi(PQa2, d, v);
//	Eigsrt(d, v);

	TMatrixDEigen eig(PQ);
	TMatrixD evc = eig.GetEigenVectors();

	Double_t AlgParF = 0.;
	AlgParF -= evc(0,0) * fM[GA05];
	fAlgPar[0] = evc(0,0);

	AlgParF -= evc(1,0) * fM[GA15];
	fAlgPar[1] = evc(1,0);

	AlgParF -= evc(2,0) * fM[GA25];
	fAlgPar[2] = evc(2,0);

	AlgParF -= evc(3,0) * fM[GA35];
	fAlgPar[3] = evc(3,0);

	AlgParF -= evc(4,0) * fM[GA45];
	fAlgPar[4] = evc(4,0);

	fAlgPar[5] = AlgParF;
}


// -----   Public method: InitMatrices   -----------------------------------
void CbmRichRingFitterEllipseTau::InitMatrices()
{
	const UInt_t numHits = fX.size();
	const UInt_t numHits2 = 2* numHits;
	const UInt_t numHits3 = 3* numHits;
	const UInt_t numHits4 = 4* numHits;
	const UInt_t numHits5 = 5* numHits;
	const UInt_t numHits6 = 6* numHits;
	const Double_t oneOverNumHits = 1./ numHits;
	UInt_t i6;
	for(UInt_t i = 0; i < numHits; i++){
		i6= i*6;
		fZ[i6] = fZT[i] = fX[i]*fX[i];
		fZ[i6+1] = fZT[i+numHits] = fX[i]*fY[i];
		fZ[i6+2] = fZT[i+numHits2] = fY[i]*fY[i];
		fZ[i6+3] = fZT[i+numHits3] = fX[i];
		fZ[i6+4] = fZT[i+numHits4] = fY[i];
		fZ[i6+5] = fZT[i+numHits5] = 1.;
	}
	AMultB(fZT, numHits6, numHits, fZ, numHits6, 6, fM);
	for (Int_t i = 0; i < numHits6;i++) fM[i]=oneOverNumHits*fM[i];

	for (Int_t i = 0; i < 25;i++) fP[i]=0.;

	fP[GM00]            = fM[GA00] - fM[GA05] * fM[GA05];
	fP[GM01] = fP[GM10] = fM[GA01] - fM[GA05] * fM[GA15];
	fP[GM02] = fP[GM20] = fM[GA02] - fM[GA05] * fM[GA25];
	fP[GM03] = fP[GM30] = fM[GA03] - fM[GA05] * fM[GA35];
	fP[GM04] = fP[GM40] = fM[GA04] - fM[GA05] * fM[GA45];

	fP[GM11]            = fM[GA11] - fM[GA15] * fM[GA15];
	fP[GM12] = fP[GM21] = fM[GA12] - fM[GA15] * fM[GA25];
	fP[GM13] = fP[GM31] = fM[GA13] - fM[GA15] * fM[GA35];
	fP[GM14] = fP[GM41] = fM[GA14] - fM[GA15] * fM[GA45];

	fP[GM22]            = fM[GA22] - fM[GA25] * fM[GA25];
	fP[GM23] = fP[GM32] = fM[GA23] - fM[GA25] * fM[GA35];
	fP[GM24] = fP[GM42] = fM[GA24] - fM[GA25] * fM[GA45];

	fP[GM33]            = fM[GA33] - fM[GA35] * fM[GA35];
	fP[GM34] = fP[GM43] = fM[GA34] - fM[GA35] * fM[GA45];

	fP[GM44]            = fM[GA44] - fM[GA45] * fM[GA45];


	for (Int_t i = 0; i < 25;i++) fQ[i]=0.;
	fQ[GM00]            = 4. * fM[GA50];
	fQ[GM01] = fQ[GM10] = 2. * fM[GA51];
	fQ[GM03] = fQ[GM30] = 2. * fM[GA53];
	fQ[GM11]            = fM[GA50]+fM[GA52];
	fQ[GM12] = fQ[GM21] = 2. * fM[GA51];
	fQ[GM13] = fQ[GM31] = fM[GA54];
	fQ[GM14] = fQ[GM41] = fM[GA53];
	fQ[GM22]            = 4. * fM[GA52];
	fQ[GM24] = fQ[GM42] = 2. * fM[GA54];
	fQ[GM33] = fQ[GM44] = 1.;
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
	pRing->SetRadius( (axisA + axisB) / 2.);

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

void CbmRichRingFitterEllipseTau::Inv5x5()
{
	// Find all NECESSARY 2x2 dets:  (30 of them)
	const Double_t det2_23_01 = fP[GM20] * fP[GM31] - fP[GM21] * fP[GM30];
	const Double_t det2_23_02 = fP[GM20] * fP[GM32] - fP[GM22] * fP[GM30];
	const Double_t det2_23_03 = fP[GM20] * fP[GM33] - fP[GM23] * fP[GM30];
	const Double_t det2_23_04 = fP[GM20] * fP[GM34] - fP[GM24] * fP[GM30];
	const Double_t det2_23_12 = fP[GM21] * fP[GM32] - fP[GM22] * fP[GM31];
	const Double_t det2_23_13 = fP[GM21] * fP[GM33] - fP[GM23] * fP[GM31];
	const Double_t det2_23_14 = fP[GM21] * fP[GM34] - fP[GM24] * fP[GM31];
	const Double_t det2_23_23 = fP[GM22] * fP[GM33] - fP[GM23] * fP[GM32];
	const Double_t det2_23_24 = fP[GM22] * fP[GM34] - fP[GM24] * fP[GM32];
	const Double_t det2_23_34 = fP[GM23] * fP[GM34] - fP[GM24] * fP[GM33];
	const Double_t det2_24_01 = fP[GM20] * fP[GM41] - fP[GM21] * fP[GM40];
	const Double_t det2_24_02 = fP[GM20] * fP[GM42] - fP[GM22] * fP[GM40];
	const Double_t det2_24_03 = fP[GM20] * fP[GM43] - fP[GM23] * fP[GM40];
	const Double_t det2_24_04 = fP[GM20] * fP[GM44] - fP[GM24] * fP[GM40];
	const Double_t det2_24_12 = fP[GM21] * fP[GM42] - fP[GM22] * fP[GM41];
	const Double_t det2_24_13 = fP[GM21] * fP[GM43] - fP[GM23] * fP[GM41];
	const Double_t det2_24_14 = fP[GM21] * fP[GM44] - fP[GM24] * fP[GM41];
	const Double_t det2_24_23 = fP[GM22] * fP[GM43] - fP[GM23] * fP[GM42];
	const Double_t det2_24_24 = fP[GM22] * fP[GM44] - fP[GM24] * fP[GM42];
	const Double_t det2_24_34 = fP[GM23] * fP[GM44] - fP[GM24] * fP[GM43];
	const Double_t det2_34_01 = fP[GM30] * fP[GM41] - fP[GM31] * fP[GM40];
	const Double_t det2_34_02 = fP[GM30] * fP[GM42] - fP[GM32] * fP[GM40];
	const Double_t det2_34_03 = fP[GM30] * fP[GM43] - fP[GM33] * fP[GM40];
	const Double_t det2_34_04 = fP[GM30] * fP[GM44] - fP[GM34] * fP[GM40];
	const Double_t det2_34_12 = fP[GM31] * fP[GM42] - fP[GM32] * fP[GM41];
	const Double_t det2_34_13 = fP[GM31] * fP[GM43] - fP[GM33] * fP[GM41];
	const Double_t det2_34_14 = fP[GM31] * fP[GM44] - fP[GM34] * fP[GM41];
	const Double_t det2_34_23 = fP[GM32] * fP[GM43] - fP[GM33] * fP[GM42];
	const Double_t det2_34_24 = fP[GM32] * fP[GM44] - fP[GM34] * fP[GM42];
	const Double_t det2_34_34 = fP[GM33] * fP[GM44] - fP[GM34] * fP[GM43];

	// Find all NECESSARY 3x3 dets:   (40 of them)
	const Double_t det3_123_012 = fP[GM10] * det2_23_12 - fP[GM11] * det2_23_02
			+ fP[GM12] * det2_23_01;
	const Double_t det3_123_013 = fP[GM10] * det2_23_13 - fP[GM11] * det2_23_03
			+ fP[GM13] * det2_23_01;
	const Double_t det3_123_014 = fP[GM10] * det2_23_14 - fP[GM11] * det2_23_04
			+ fP[GM14] * det2_23_01;
	const Double_t det3_123_023 = fP[GM10] * det2_23_23 - fP[GM12] * det2_23_03
			+ fP[GM13] * det2_23_02;
	const Double_t det3_123_024 = fP[GM10] * det2_23_24 - fP[GM12] * det2_23_04
			+ fP[GM14] * det2_23_02;
	const Double_t det3_123_034 = fP[GM10] * det2_23_34 - fP[GM13] * det2_23_04
			+ fP[GM14] * det2_23_03;
	const Double_t det3_123_123 = fP[GM11] * det2_23_23 - fP[GM12] * det2_23_13
			+ fP[GM13] * det2_23_12;
	const Double_t det3_123_124 = fP[GM11] * det2_23_24 - fP[GM12] * det2_23_14
			+ fP[GM14] * det2_23_12;
	const Double_t det3_123_134 = fP[GM11] * det2_23_34 - fP[GM13] * det2_23_14
			+ fP[GM14] * det2_23_13;
	const Double_t det3_123_234 = fP[GM12] * det2_23_34 - fP[GM13] * det2_23_24
			+ fP[GM14] * det2_23_23;
	const Double_t det3_124_012 = fP[GM10] * det2_24_12 - fP[GM11] * det2_24_02
			+ fP[GM12] * det2_24_01;
	const Double_t det3_124_013 = fP[GM10] * det2_24_13 - fP[GM11] * det2_24_03
			+ fP[GM13] * det2_24_01;
	const Double_t det3_124_014 = fP[GM10] * det2_24_14 - fP[GM11] * det2_24_04
			+ fP[GM14] * det2_24_01;
	const Double_t det3_124_023 = fP[GM10] * det2_24_23 - fP[GM12] * det2_24_03
			+ fP[GM13] * det2_24_02;
	const Double_t det3_124_024 = fP[GM10] * det2_24_24 - fP[GM12] * det2_24_04
			+ fP[GM14] * det2_24_02;
	const Double_t det3_124_034 = fP[GM10] * det2_24_34 - fP[GM13] * det2_24_04
			+ fP[GM14] * det2_24_03;
	const Double_t det3_124_123 = fP[GM11] * det2_24_23 - fP[GM12] * det2_24_13
			+ fP[GM13] * det2_24_12;
	const Double_t det3_124_124 = fP[GM11] * det2_24_24 - fP[GM12] * det2_24_14
			+ fP[GM14] * det2_24_12;
	const Double_t det3_124_134 = fP[GM11] * det2_24_34 - fP[GM13] * det2_24_14
			+ fP[GM14] * det2_24_13;
	const Double_t det3_124_234 = fP[GM12] * det2_24_34 - fP[GM13] * det2_24_24
			+ fP[GM14] * det2_24_23;
	const Double_t det3_134_012 = fP[GM10] * det2_34_12 - fP[GM11] * det2_34_02
			+ fP[GM12] * det2_34_01;
	const Double_t det3_134_013 = fP[GM10] * det2_34_13 - fP[GM11] * det2_34_03
			+ fP[GM13] * det2_34_01;
	const Double_t det3_134_014 = fP[GM10] * det2_34_14 - fP[GM11] * det2_34_04
			+ fP[GM14] * det2_34_01;
	const Double_t det3_134_023 = fP[GM10] * det2_34_23 - fP[GM12] * det2_34_03
			+ fP[GM13] * det2_34_02;
	const Double_t det3_134_024 = fP[GM10] * det2_34_24 - fP[GM12] * det2_34_04
			+ fP[GM14] * det2_34_02;
	const Double_t det3_134_034 = fP[GM10] * det2_34_34 - fP[GM13] * det2_34_04
			+ fP[GM14] * det2_34_03;
	const Double_t det3_134_123 = fP[GM11] * det2_34_23 - fP[GM12] * det2_34_13
			+ fP[GM13] * det2_34_12;
	const Double_t det3_134_124 = fP[GM11] * det2_34_24 - fP[GM12] * det2_34_14
			+ fP[GM14] * det2_34_12;
	const Double_t det3_134_134 = fP[GM11] * det2_34_34 - fP[GM13] * det2_34_14
			+ fP[GM14] * det2_34_13;
	const Double_t det3_134_234 = fP[GM12] * det2_34_34 - fP[GM13] * det2_34_24
			+ fP[GM14] * det2_34_23;
	const Double_t det3_234_012 = fP[GM20] * det2_34_12 - fP[GM21] * det2_34_02
			+ fP[GM22] * det2_34_01;
	const Double_t det3_234_013 = fP[GM20] * det2_34_13 - fP[GM21] * det2_34_03
			+ fP[GM23] * det2_34_01;
	const Double_t det3_234_014 = fP[GM20] * det2_34_14 - fP[GM21] * det2_34_04
			+ fP[GM24] * det2_34_01;
	const Double_t det3_234_023 = fP[GM20] * det2_34_23 - fP[GM22] * det2_34_03
			+ fP[GM23] * det2_34_02;
	const Double_t det3_234_024 = fP[GM20] * det2_34_24 - fP[GM22] * det2_34_04
			+ fP[GM24] * det2_34_02;
	const Double_t det3_234_034 = fP[GM20] * det2_34_34 - fP[GM23] * det2_34_04
			+ fP[GM24] * det2_34_03;
	const Double_t det3_234_123 = fP[GM21] * det2_34_23 - fP[GM22] * det2_34_13
			+ fP[GM23] * det2_34_12;
	const Double_t det3_234_124 = fP[GM21] * det2_34_24 - fP[GM22] * det2_34_14
			+ fP[GM24] * det2_34_12;
	const Double_t det3_234_134 = fP[GM21] * det2_34_34 - fP[GM23] * det2_34_14
			+ fP[GM24] * det2_34_13;
	const Double_t det3_234_234 = fP[GM22] * det2_34_34 - fP[GM23] * det2_34_24
			+ fP[GM24] * det2_34_23;

	// Find all NECESSARY 4x4 dets:   (25 of them)
	const Double_t det4_0123_0123 = fP[GM00] * det3_123_123 - fP[GM01]
			* det3_123_023 + fP[GM02] * det3_123_013 - fP[GM03] * det3_123_012;
	const Double_t det4_0123_0124 = fP[GM00] * det3_123_124 - fP[GM01]
			* det3_123_024 + fP[GM02] * det3_123_014 - fP[GM04] * det3_123_012;
	const Double_t det4_0123_0134 = fP[GM00] * det3_123_134 - fP[GM01]
			* det3_123_034 + fP[GM03] * det3_123_014 - fP[GM04] * det3_123_013;
	const Double_t det4_0123_0234 = fP[GM00] * det3_123_234 - fP[GM02]
			* det3_123_034 + fP[GM03] * det3_123_024 - fP[GM04] * det3_123_023;
	const Double_t det4_0123_1234 = fP[GM01] * det3_123_234 - fP[GM02]
			* det3_123_134 + fP[GM03] * det3_123_124 - fP[GM04] * det3_123_123;
	const Double_t det4_0124_0123 = fP[GM00] * det3_124_123 - fP[GM01]
			* det3_124_023 + fP[GM02] * det3_124_013 - fP[GM03] * det3_124_012;
	const Double_t det4_0124_0124 = fP[GM00] * det3_124_124 - fP[GM01]
			* det3_124_024 + fP[GM02] * det3_124_014 - fP[GM04] * det3_124_012;
	const Double_t det4_0124_0134 = fP[GM00] * det3_124_134 - fP[GM01]
			* det3_124_034 + fP[GM03] * det3_124_014 - fP[GM04] * det3_124_013;
	const Double_t det4_0124_0234 = fP[GM00] * det3_124_234 - fP[GM02]
			* det3_124_034 + fP[GM03] * det3_124_024 - fP[GM04] * det3_124_023;
	const Double_t det4_0124_1234 = fP[GM01] * det3_124_234 - fP[GM02]
			* det3_124_134 + fP[GM03] * det3_124_124 - fP[GM04] * det3_124_123;
	const Double_t det4_0134_0123 = fP[GM00] * det3_134_123 - fP[GM01]
			* det3_134_023 + fP[GM02] * det3_134_013 - fP[GM03] * det3_134_012;
	const Double_t det4_0134_0124 = fP[GM00] * det3_134_124 - fP[GM01]
			* det3_134_024 + fP[GM02] * det3_134_014 - fP[GM04] * det3_134_012;
	const Double_t det4_0134_0134 = fP[GM00] * det3_134_134 - fP[GM01]
			* det3_134_034 + fP[GM03] * det3_134_014 - fP[GM04] * det3_134_013;
	const Double_t det4_0134_0234 = fP[GM00] * det3_134_234 - fP[GM02]
			* det3_134_034 + fP[GM03] * det3_134_024 - fP[GM04] * det3_134_023;
	const Double_t det4_0134_1234 = fP[GM01] * det3_134_234 - fP[GM02]
			* det3_134_134 + fP[GM03] * det3_134_124 - fP[GM04] * det3_134_123;
	const Double_t det4_0234_0123 = fP[GM00] * det3_234_123 - fP[GM01]
			* det3_234_023 + fP[GM02] * det3_234_013 - fP[GM03] * det3_234_012;
	const Double_t det4_0234_0124 = fP[GM00] * det3_234_124 - fP[GM01]
			* det3_234_024 + fP[GM02] * det3_234_014 - fP[GM04] * det3_234_012;
	const Double_t det4_0234_0134 = fP[GM00] * det3_234_134 - fP[GM01]
			* det3_234_034 + fP[GM03] * det3_234_014 - fP[GM04] * det3_234_013;
	const Double_t det4_0234_0234 = fP[GM00] * det3_234_234 - fP[GM02]
			* det3_234_034 + fP[GM03] * det3_234_024 - fP[GM04] * det3_234_023;
	const Double_t det4_0234_1234 = fP[GM01] * det3_234_234 - fP[GM02]
			* det3_234_134 + fP[GM03] * det3_234_124 - fP[GM04] * det3_234_123;
	const Double_t det4_1234_0123 = fP[GM10] * det3_234_123 - fP[GM11]
			* det3_234_023 + fP[GM12] * det3_234_013 - fP[GM13] * det3_234_012;
	const Double_t det4_1234_0124 = fP[GM10] * det3_234_124 - fP[GM11]
			* det3_234_024 + fP[GM12] * det3_234_014 - fP[GM14] * det3_234_012;
	const Double_t det4_1234_0134 = fP[GM10] * det3_234_134 - fP[GM11]
			* det3_234_034 + fP[GM13] * det3_234_014 - fP[GM14] * det3_234_013;
	const Double_t det4_1234_0234 = fP[GM10] * det3_234_234 - fP[GM12]
			* det3_234_034 + fP[GM13] * det3_234_024 - fP[GM14] * det3_234_023;
	const Double_t det4_1234_1234 = fP[GM11] * det3_234_234 - fP[GM12]
			* det3_234_134 + fP[GM13] * det3_234_124 - fP[GM14] * det3_234_123;

	// Find the 5x5 det:
	const Double_t det = fP[GM00] * det4_1234_1234 - fP[GM01] * det4_1234_0234
			+ fP[GM02] * det4_1234_0134 - fP[GM03] * det4_1234_0124 + fP[GM04]
			* det4_1234_0123;
//	if (determ)
//		*determ = det;
//
//	if (det == 0) {
//		Error("Inv5x5", "matrix is singular");
//		return kFALSE;
//	}
	const Double_t oneOverDet = 1.0 / det;
	const Double_t mn1OverDet = -oneOverDet;

	fP[GM00] = det4_1234_1234 * oneOverDet;
	fP[GM01] = det4_0234_1234 * mn1OverDet;
	fP[GM02] = det4_0134_1234 * oneOverDet;
	fP[GM03] = det4_0124_1234 * mn1OverDet;
	fP[GM04] = det4_0123_1234 * oneOverDet;

	fP[GM10] = det4_1234_0234 * mn1OverDet;
	fP[GM11] = det4_0234_0234 * oneOverDet;
	fP[GM12] = det4_0134_0234 * mn1OverDet;
	fP[GM13] = det4_0124_0234 * oneOverDet;
	fP[GM14] = det4_0123_0234 * mn1OverDet;

	fP[GM20] = det4_1234_0134 * oneOverDet;
	fP[GM21] = det4_0234_0134 * mn1OverDet;
	fP[GM22] = det4_0134_0134 * oneOverDet;
	fP[GM23] = det4_0124_0134 * mn1OverDet;
	fP[GM24] = det4_0123_0134 * oneOverDet;

	fP[GM30] = det4_1234_0124 * mn1OverDet;
	fP[GM31] = det4_0234_0124 * oneOverDet;
	fP[GM32] = det4_0134_0124 * mn1OverDet;
	fP[GM33] = det4_0124_0124 * oneOverDet;
	fP[GM34] = det4_0123_0124 * mn1OverDet;

	fP[GM40] = det4_1234_0123 * oneOverDet;
	fP[GM41] = det4_0234_0123 * mn1OverDet;
	fP[GM42] = det4_0134_0123 * oneOverDet;
	fP[GM43] = det4_0124_0123 * mn1OverDet;
	fP[GM44] = det4_0123_0123 * oneOverDet;
}

void CbmRichRingFitterEllipseTau::AMultB(const Double_t * const ap, Int_t na, Int_t ncolsa,
            const Double_t * const bp, Int_t nb, Int_t ncolsb, Double_t *cp)
{
// Elementary routine to calculate matrix multiplication A*B

   const Double_t *arp0 = ap;                     // Pointer to  A[i,0];
   while (arp0 < ap+na) {
      for (const Double_t *bcp = bp; bcp < bp+ncolsb; ) { // Pointer to the j-th column of B, Start bcp = B[0,0]
         const Double_t *arp = arp0;                       // Pointer to the i-th row of A, reset to A[i,0]
         Double_t cij = 0;
         while (bcp < bp+nb) {                     // Scan the i-th row of A and
            cij += *arp++ * *bcp;                   // the j-th col of B
            bcp += ncolsb;
         }
         *cp++ = cij;
         bcp -= nb-1;                              // Set bcp to the (j+1)-th col
      }
      arp0 += ncolsa;                             // Set ap to the (i+1)-th row
   }
}

#define ROTATE(a,i,j,k,l)  g=a[i][j];h=a[k][l];a[i][j]=g-s*(h+g*tau);a[k][l]=h+s*(g-h*tau)
#define MAXSWEEP 50
void CbmRichRingFitterEllipseTau::Jacobi(Double_t a[5][5], Double_t d[5], Double_t v[5][5])
{

	Double_t tresh, theta, tau, t, sm, s, h, g, c;

	Double_t b[5], z[5];
	UInt_t ip, iq, i,j;
	for (ip = 0; ip < 5; ip++) {
		for (iq = 0; iq < 5; iq++)
			v[ip][iq] = 0.;
		v[ip][ip] = 1.;
	}

	for (ip = 0; ip < 5; ip++) {
		b[ip] = a[ip][ip];
		z[ip] = 0.;
	}

	Int_t nrot = 0;

	for (i = 1; i <= MAXSWEEP; i++) {

		for (sm = 0., ip = 0; ip < 5; ip++)
			for (iq = ip + 1; iq < 5; iq++)
				sm += fabs(a[ip][iq]);
		if (sm == 0.) {
			return;
		}

		tresh = (i < 4 ? 0.2 * sm / (5 * 5) : 0.);

		for (ip = 0; ip < 4; ip++)
			for (iq = ip + 1; iq < 5; iq++) {

				g = 100. * fabs(a[ip][iq]);
				if (i > 4 && (float) fabs(d[ip] + g) == (float) fabs(d[ip])
						&& (float) fabs(d[iq] + g) == (float) fabs(d[iq]))
					a[ip][iq] = 0.;

				else if (fabs(a[ip][iq]) > tresh) {
					h = d[ip] - d[iq];

					if ((float) (fabs(h) + g) == (float) fabs(h))
						t = a[ip][iq] / h;
					else {
						theta = 0.5 * h / a[ip][iq];
						t = 1. / (fabs(theta) + sqrt(1. + theta * theta));
						if (theta < 0.)
							t = -t;
					}
					c = 1. / sqrt(1 + t * t);
					s = t * c;
					tau = s / (1. + c);
					h = t * a[ip][iq];
					z[ip] -= h;
					z[iq] += h;
					d[ip] -= h;
					d[iq] += h;
					a[ip][iq] = 0.;
					for (j = 0; j < ip; j++) {
						ROTATE(a,j,ip,j,iq);
					}
					for (j = ip + 1; j < iq; j++) {
						ROTATE(a,ip,j,iq,j);
					}
					for (j = iq + 1; j < 5; j++) {
						ROTATE(a,ip,j,j,iq);
					}
					for (j = 0; j < 5; j++) {
						ROTATE(v,j,ip,j,iq);
					}
					++nrot;
				}
			}
		for (ip = 0; ip < 5; ip++) {
			b[ip] += z[ip];
			d[ip] = b[ip];
			z[ip] = 0.;
		}
	}//i rot
}

void CbmRichRingFitterEllipseTau::Eigsrt(Double_t d[5],  Double_t v[5][5])
{
	Double_t p;
	Int_t i,k,j;
	for (i = 0; i < 5; i++) {
		p = d[k = i];
		for (j = i + 1; j < 5; j++)
			if (d[j] >= p)
				p = d[k = j];
		if (k != i) {
			d[k] = d[i];
			d[i] = p;
			for (j = 0; j < 5; j++) {
				p = v[j][i];
				v[j][i] = v[j][k];
				v[j][k] = p;
			}
		}
	}
}




ClassImp(CbmRichRingFitterEllipseTau)
