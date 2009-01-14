#include "CbmRichRingFitterQa.h"
#include "CbmRichRingFitterEllipseTau.h"
#include "CbmRichRingFitterEllipse.h"
#include "TRandom.h"
#include "TMath.h"
#include "CbmRichRing.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TMath.h"
#include <iostream>
#include <vector>


using std::vector;
using std::cout;
using std::endl;

CbmRichRingFitterQa::CbmRichRingFitterQa()
{
	fhErrorA = new TH1D("fhErrorA","fhErrorA;Error A;Entries", 100, -2.,2.);
	fhErrorB = new TH1D("fhErrorB","fhErrorB;Error B;Entries", 100, -2.,2.);;
	fhErrorX = new TH1D("fhErrorX","fhErrorX;Error X;Entries", 100, -2.,2.);;
	fhErrorY = new TH1D("fhErrorY","fhErrorY;Error Y;Entries", 100, -2.,2.);;
	fhErrorPhi = new TH1D("fhErrorPhi","fhErrorPhi;Error Phi;Entries", 100, -2.,2.);;

	fhA = new TH1D("fhA","fhA;A;Entries", 100, 5, 7);
	fhB = new TH1D("fhB","fhB;B;Entries", 100, 5, 7);;
	fhX = new TH1D("fhX","fhX;X;Entries", 100, -1.,1.);;
	fhY = new TH1D("fhY","fhY;Y;Entries", 100, -1.,1.);;
	fhPhi = new TH1D("fhPhi","fhPhi;Phi;Entries", 100, -TMath::Pi()/2. -TMath::Pi()/6. , TMath::Pi()/2.+TMath::Pi()/6.);;
}

void CbmRichRingFitterQa::GenerateEllipse()
{
	Double_t maxX = 15;
	Double_t maxY = 15;
	Int_t nofHits = 50;
	Double_t A = 6.2;
	Double_t B = 5.6;
	vector<QaHit> hits;
	vector<Double_t> hitXar;
	vector<Double_t> hitYar;
	CbmRichRing ellipse;
	QaHit hit;
	Int_t nofBadFit = 0;

	//CbmRichRingFitterFuzzy * fit = new CbmRichRingFitterFuzzy();
	//CbmRichRingFitterEllipseTau * fit = new CbmRichRingFitterEllipseTau();
	CbmRichRingFitterEllipse * fit = new CbmRichRingFitterEllipse();

	TStopwatch timer;
	timer.Start();
	timer.Stop();

	for (Int_t iR = 0; iR < 100000; iR++){
		Double_t X0 = 0;//gRandom->Rndm()*(maxX - A);
		Double_t Y0 = 0;//gRandom->Rndm()* (maxY - A);
		Double_t phi =  TMath::Pi()*(6./12.); //gRandom->Rndm()*TMath::Pi() - TMath::Pi()/2.;

		ellipse.SetXYABPhi(X0, Y0, A, B, phi);

		hits.clear();
		hitXar.clear();
		hitYar.clear();
		for (Int_t i = 0; i< nofHits; i++){
			Double_t alfa = gRandom->Rndm()*2.*TMath::Pi();
			Double_t errorX = gRandom->Gaus(0, 0.2);
			Double_t errorY = gRandom->Gaus(0, 0.2);

			Double_t hitX = A*cos(alfa);
			Double_t hitY = B*sin(alfa);

			Double_t hitXRot = hitX*cos(phi) - hitY*sin(phi);
			Double_t hitYRot = hitX*sin(phi) + hitY*cos(phi);

			hit.fX = hitXRot + X0 + errorX;
			hit.fY = hitYRot + Y0 + errorY;

			hits.push_back(hit);

			hitXar.push_back(hitXRot + X0 + errorX);
			hitYar.push_back(hitYRot + Y0 + errorY);
		}
		timer.Start(false);
		//fit->DoFit1(&ellipse, hits);

		fit->DoFit1(&ellipse, hitXar,hitYar);
		timer.Stop();
		//cout << "Reconstruct (X0, Y0, A, B, phi) = " << ellipse.GetCenterX() <<
		//	 " " << ellipse.GetCenterY() << " " << ellipse.GetAaxis() << " "
		//	 << ellipse.GetBaxis() << " " << ellipse.GetPhi() << endl;

		fhErrorA->Fill(A-ellipse.GetAaxis());
		fhErrorB->Fill(B-ellipse.GetBaxis());
		fhErrorX->Fill(X0-ellipse.GetCenterX());
		fhErrorY->Fill(Y0-ellipse.GetCenterY());
		fhErrorPhi->Fill(phi-ellipse.GetPhi());

		if (ellipse.GetAaxis() > 9.2 || ellipse.GetAaxis() < 3.2 ||
		    ellipse.GetBaxis() > 8.6 || ellipse.GetBaxis() < 2.6 	) nofBadFit++;

		fhA->Fill(ellipse.GetAaxis());
		fhB->Fill(ellipse.GetBaxis());
		fhX->Fill(ellipse.GetCenterX());
		fhY->Fill(ellipse.GetCenterY());
		fhPhi->Fill(ellipse.GetPhi());
	}

    //timer.Stop();
    cout << "TIME  CPU time " <<  timer.CpuTime() << " s" << endl;

	DrawHist();
    cout << timer.CpuTime() << endl;
    cout << nofBadFit << endl;
}

void CbmRichRingFitterQa::DrawHist()
{
	TCanvas * c = new TCanvas();
	c->Divide(3,2);
	c->cd(1);
	fhErrorA->Draw();
	c->cd(2);
	fhErrorB->Draw();
	c->cd(3);
	fhErrorX->Draw();
	c->cd(4);
	fhErrorY->Draw();
	c->cd(5);
	fhErrorPhi->Draw();
	cout.precision(4);
	cout << fhErrorA->GetMean() <<" " << fhErrorA->GetRMS() << endl;
	cout << fhErrorB->GetMean() <<" " << fhErrorB->GetRMS() << endl;
	cout << fhErrorX->GetMean() <<" " << fhErrorX->GetRMS() << endl;
	cout << fhErrorY->GetMean() <<" " << fhErrorY->GetRMS() << endl;
	cout << fhErrorPhi->GetMean() <<" " << fhErrorPhi->GetRMS() << endl;

	TCanvas * c2 = new TCanvas();
	c2->Divide(3,2);
	c2->cd(1);
	fhA->Draw();
	c2->cd(2);
	fhB->Draw();
	c2->cd(3);
	fhX->Draw();
	c2->cd(4);
	fhY->Draw();
	c2->cd(5);
	fhPhi->Draw();
}


ClassImp(CbmRichRingFitterQa);

