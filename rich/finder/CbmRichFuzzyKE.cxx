#include "CbmRichFuzzyKE.h"
#include "TRandom.h"
#include <iostream>
#include "TH2D.h"
#include "TCanvas.h"
#include "TMarker.h"
#include "TArc.h"
#include "TMath.h"
using std::cout;
using std::endl;

CbmRichFuzzyKE::CbmRichFuzzyKE(){
	fQ = 2.;
}
Double_t CbmRichFuzzyKE::u(Int_t i, Int_t j)
{
	Double_t K = fEllipses.size();
	
	Double_t sumDD = 0;
	for (Int_t iK = 0; iK < K; iK++){
		sumDD += std::pow( D(i,j) / D(iK,j) ,  2. / (fQ-1.));
	}
	//cout << 1./sumDD << endl;
	return 1./sumDD;
}

Double_t CbmRichFuzzyKE::r(Int_t i)
{
	Double_t sum1 = 0;
	Double_t sum2 = 0;	
	Int_t N = fHits.size();
	
	for (Int_t jN = 0; jN < N; jN++){
		Double_t uij = pow( u(i, jN), fQ );
		sum1 += uij * (d1(i, jN) + d2(i,jN));
		sum2 += uij;
	}
	
	return sum1 / sum2;
}

Double_t CbmRichFuzzyKE::teta(Int_t i, Int_t j, Int_t focus)
{
	Double_t xf;
	Double_t yf;
	Double_t Pi = TMath::Pi();
	if (focus == 1){
		xf = fEllipses[i].fV11;
		yf = fEllipses[i].fV12;
	}else{
		xf = fEllipses[i].fV21;
		yf = fEllipses[i].fV22;
	}
	
	Double_t x = fHits[j].fX;
	Double_t y = fHits[j].fY;

	if( x > xf && y > yf ){
		return atan(fabs((y-yf)/(x-xf)));
	}
	if( x < xf && y > yf ){
		return Pi - atan(fabs((y-yf)/(x-xf)));
	}
	if( x < xf && y < yf ){
		return Pi + atan(fabs((y-yf)/(x-xf)));
	}
	if( x > xf && y < yf){
		return 2.*Pi - atan(fabs((y-yf)/(x-xf)));
	}
}

Double_t CbmRichFuzzyKE::v11(Int_t i)
{
	Double_t sum1 = 0;
	Double_t sum2 = 0;
	Int_t N = fHits.size();
	
	for (Int_t jN = 0; jN < N; jN++){
		Double_t uij = fUNew[i][jN];//pow(u(i,jN), fQ);
		
		//cout << teta(i,jN,1) << " " ;
		sum1 += uij * (fHits[jN].fX - (fRNew[i] - d2(i,jN)) * cos(teta(i,jN,1)) );
		sum2 += uij;
	}
	//cout << endl;
	return sum1/sum2;
}

Double_t CbmRichFuzzyKE::v12(Int_t i)
{
	Double_t sum1 = 0;
	Double_t sum2 = 0;
	Int_t N = fHits.size();
	
	for (Int_t jN = 0; jN < N; jN++){
		Double_t uij = fUNew[i][jN];//pow(u(i,jN), fQ);
		sum1 += uij * (fHits[jN].fY - (fRNew[i] - d2(i,jN)) * sin(teta(i,jN,1)) );
		sum2 += uij;
	}
	return sum1/sum2;
}

Double_t CbmRichFuzzyKE::v21(Int_t i)
{
	Double_t sum1 = 0;
	Double_t sum2 = 0;
	Int_t N = fHits.size();
	
	for (Int_t jN = 0; jN < N; jN++){
		Double_t uij = fUNew[i][jN];//pow(u(i,jN), fQ);
		sum1 += uij * (fHits[jN].fX - (fRNew[i] - d1(i,jN)) * cos(teta(i,jN,2)) );
		sum2 += uij;
	}
	return sum1/sum2;
}

Double_t CbmRichFuzzyKE::v22(Int_t i)
{
	Double_t sum1 = 0;
	Double_t sum2 = 0;
	Int_t N = fHits.size();
	
	for (Int_t jN = 0; jN < N; jN++){
		Double_t uij = fUNew[i][jN];//pow(u(i,jN), fQ);
		sum1 += uij * (fHits[jN].fY - (fRNew[i] - d1(i,jN)) * sin(teta(i,jN,2)) );
		sum2 += uij;
	}
	return sum1/sum2;
}

Double_t CbmRichFuzzyKE::d1(Int_t i, Int_t j)
{
	Double_t xf1 = fEllipses[i].fV11;
	Double_t yf1 = fEllipses[i].fV12;
	Double_t x = fHits[j].fX;
	Double_t y = fHits[j].fY;
    Double_t d1 = sqrt((x - xf1)*(x - xf1) + (y - yf1)*(y - yf1)); 
  //  cout << "d1 = " << d1 << endl;
    return d1;
}

Double_t CbmRichFuzzyKE::d2(Int_t i, Int_t j)
{
	Double_t xf2 = fEllipses[i].fV21;
	Double_t yf2 = fEllipses[i].fV22;
	Double_t x = fHits[j].fX;
	Double_t y = fHits[j].fY;
    Double_t d2 = sqrt((x - xf2)*(x - xf2) + (y - yf2)*(y - yf2)); 
  //  cout << "d2 = " << d2 << endl;
    return d2;
}

// i-ellipse , j-hit
Double_t CbmRichFuzzyKE::D(Int_t i, Int_t j)
{
	Double_t r = fEllipses[i].fR;

    Double_t ri = d1(i,j) + d2(i,j) - r;
    return ri;
}

void CbmRichFuzzyKE::Minimize(){
	
	Int_t K = fEllipses.size();
	Int_t N = fHits.size();

	std::vector<Ellipse> tempEllipses;
	std::vector<vector<Double_t> > tempU;

	tempEllipses.resize(K);
	fRNew.resize(K);	
	tempU.resize(K);	
	fU.resize(K);
	fUNew.resize(K);
	for (Int_t iK=0; iK<K; iK++){
		tempU[iK].resize(N);
		fU[iK].resize(N);
		fUNew[iK].resize(N);
	}
	
	for (Int_t iK=0; iK<K; iK++){
		for (Int_t iN=0; iN<N; iN++){
			fU[iK][iN] = 1.;
		}
	}
	/*cout << " init param:" << endl;
	for(Int_t iK=0; iK<K; iK++){
			cout << fEllipses[iK].fR <<" " << fEllipses[iK].fV11 << " " <<
			" " << fEllipses[iK].fV12 << " " << fEllipses[iK].fV21 << " " << fEllipses[iK].fV22 << endl;
	}
	cout << " hits param:" << N <<endl;*/

	for (Int_t iter = 0; iter < 50; iter++){
		//std::cout << "iter = " << iter << std::endl;
		
		for(Int_t iK=0; iK<K; iK++){
			tempEllipses[iK].fR = r(iK);	
			fRNew[iK] = tempEllipses[iK].fR;
		}
		//for(Int_t iK=0; iK<K; iK++){
	//		for(Int_t jN =0; jN <N; jN++){
	//			fUNew[iK][jN] = u(iK, jN);
	//		}
		//}
		
		for(Int_t iK=0; iK<K; iK++){
			for(Int_t jN =0; jN <N; jN++){
				fUNew[iK][jN] = u(iK, jN);
			}
			tempEllipses[iK].fV11 = v11(iK);
			tempEllipses[iK].fV12 = v12(iK);
			tempEllipses[iK].fV21 = v21(iK);
			tempEllipses[iK].fV22 = v22(iK);		
		}
		
		Double_t er = 0;
		for(Int_t iK=0; iK<K; iK++){
			for(Int_t jN =0; jN <N; jN++){
				tempU[iK][jN] = u(iK, jN);
				er += fabs(tempU[iK][jN] - fU[iK][jN]);
			}
		}
		//cout << "er = "  << er << endl;
		fEllipses.clear();
		fEllipses.assign(tempEllipses.begin(), tempEllipses.end());

		for (Int_t iK=0; iK<K; iK++){
			fU[iK].clear();
			fU[iK].assign(tempU[iK].begin(), tempU[iK].end());
		}
		//for(Int_t iK=0; iK<K; iK++){
		//		cout << tempEllipses[iK].fR <<" " << fEllipses[iK].fV11 << " " <<
		//		" " << fEllipses[iK].fV12 << " " << fEllipses[iK].fV21 << " " << fEllipses[iK].fV22 << endl;
		//}
		//cout << endl;
	}// iter
	/*cout << "after minimization: "<< endl;
	for(Int_t iK=0; iK<K; iK++){
			cout << tempEllipses[iK].fR <<" " << fEllipses[iK].fV11 << " " <<
			" " << fEllipses[iK].fV12 << " " << fEllipses[iK].fV21 << " " << fEllipses[iK].fV22 << endl;
	}
	cout << endl;*/
	Draw(kGreen);
}

void CbmRichFuzzyKE::GenerateEllipses()
{
	std::cout << "CbmRichFuzzyKE::GenerateEllipses()" << endl;
	Int_t nofEl = 1;
	Double_t maxX = 15;
	Double_t maxY = 15;
	Int_t nofHits = 25;
	Double_t A = 3;
	Double_t B = 2.85;
	cout << "generate ellipses"<< endl;
	for (Int_t iE = 0; iE < nofEl; iE++){
		Double_t X0 = gRandom->Rndm()*(maxX - A);
		Double_t Y0 = gRandom->Rndm()* (maxY - A);	
		Double_t phi = gRandom->Rndm()*TMath::Pi() - TMath::Pi()/2.;
		CbmRichRing ellipse;
		ellipse.SetXYABPhi(X0, Y0, A, B, phi);
		fEllipses.push_back(ParTransform(&ellipse));

		std::cout <<  fEllipses[iE].fR << " " << fEllipses[iE].fV11 << " " <<
		" " << fEllipses[iE].fV12 << " " << fEllipses[iE].fV21 << " " << fEllipses[iE].fV22 << std::endl;
		for (Int_t i = 0; i< nofHits; i++){
			Double_t alfa = gRandom->Rndm()*2.*TMath::Pi();
			Double_t errorX = gRandom->Gaus(0, 0.2);
			Double_t errorY = gRandom->Gaus(0, 0.2);
			
			Double_t hitX = A*cos(alfa);
			Double_t hitY = B*sin(alfa);			

			Double_t hitXRot = hitX*cos(phi) - hitY*sin(phi);
			Double_t hitYRot = hitX*sin(phi) + hitY*cos(phi);	
			
			Hit hit;
			hit.fX = hitXRot + X0 + errorX;
			hit.fY = hitYRot + Y0 + errorY;	

			fHits.push_back(hit);
		}
	}
	TCanvas* c = new TCanvas();
	c->SetGrid();
	TH2D *h = new TH2D("h", "Simulated rings", 25, 0,15, 25, 0 ,15);
	h->SetStats(kFALSE);
	h->Draw();
	Draw(kRed);

}

Ellipse CbmRichFuzzyKE::ParTransform(CbmRichRing* ellipse)
{
	Ellipse el;
	
	el.fR = 2. * ellipse->GetAaxis();
	el.fV11 = ellipse->GetXF1();
	el.fV12 = ellipse->GetYF1();
	el.fV21 = ellipse->GetXF2();
	el.fV22 = ellipse->GetYF2();
	return el;
}

void CbmRichFuzzyKE::Draw(Int_t color)
{
	//TCanvas* c = new TCanvas();
	//c->SetGrid();
	//TH2D *h = new TH2D("h", "Simulated rings", 25, 0,15, 25, 0 ,15);
	//h->SetStats(kFALSE);
	//h->Draw();

	TMarker fDisplayedHit;
	fDisplayedHit.SetMarkerStyle ( 21 );
	fDisplayedHit.SetMarkerSize ( 1 );
	fDisplayedHit.SetMarkerColor ( kBlue );
	
	TEllipse fDisplayedRing;
	fDisplayedRing.SetLineWidth ( 3 );
	fDisplayedRing.SetLineColor ( color );
	fDisplayedRing.SetFillStyle ( 0 );

	TMarker fDisplayedCenter;
	fDisplayedCenter.SetMarkerStyle ( 5 );
	fDisplayedCenter.SetMarkerSize ( 0.7 );
	fDisplayedCenter.SetMarkerColor ( 4 );

	Int_t nofHits = fHits.size();
	
	for (int iH = 0; iH < nofHits; iH++){ 
		fDisplayedHit.DrawMarker( fHits[iH].fX, fHits[iH].fY);
	}
	Int_t nofEl = fEllipses.size();
	for (int iE = 0; iE < nofEl; iE++){ 
		std::vector<Double_t> par = GetXYABP(fEllipses[iE]);
		cout << par[2] << " " << par [3] << endl;
		fDisplayedRing.DrawEllipse( par[0], par[1], par[2], par[3], 0., 360., par[4] * 180. / TMath::Pi() ); //risuem kol'co
		fDisplayedCenter.DrawMarker ( par[0], par[1] );

	}
}

std::vector<Double_t> CbmRichFuzzyKE::GetXYABP(Ellipse el){
	///calculate center of the ellipse
	 Double_t xc = (el.fV11 + el.fV21)/2.;
	 Double_t yc = (el.fV12 + el.fV22)/2.;
	 ///calculate length of b axes
	 Double_t p1 = (el.fV11 - el.fV21)*(el.fV11 - el.fV21) + (el.fV12 - el.fV22)*(el.fV12 - el.fV22);
	
	 Double_t c = sqrt ( p1 ) / 2.;
	 Double_t p2 = (el.fR/2.)*(el.fR/2.) - c*c;
	 Double_t b = sqrt ( p2 );
	 ///calculate angle
	 Double_t p3 = el.fV21 - el.fV11;
	 Double_t k = (el.fV22 - el.fV12) / (el.fV21 - el.fV11);
	 Double_t ang = atan(k);
	 std::vector<Double_t> par;
	 par.push_back(xc);
	 par.push_back(yc);
	 par.push_back(el.fR/2.);
	 par.push_back(b);
	 par.push_back(ang);	
	 return par;
}
 
ClassImp(CbmRichFuzzyKE);

