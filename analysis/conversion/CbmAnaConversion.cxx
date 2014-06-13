/**
 * \file CbmAnaConversion.cxx
 *
 * \author Tariq Mahmoud <t.mahmoud@gsi.de>
 * \date 2012
 **/

#include "CbmAnaConversion.h"

#include "TH1D.h"
#include "TH1.h"
#include "TH3.h"
#include "TCanvas.h"
#include "TClonesArray.h"

#include "CbmMCTrack.h"
#include "FairTrackParam.h"
#include "CbmRichHit.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmDrawHist.h"
#include "CbmTrackMatchNew.h"
#include "CbmRichPoint.h"
#include "CbmGlobalTrack.h"
#include "CbmStsTrack.h"

#include "std/utils/CbmLitUtils.h"

#include <iostream>
#include <string>
#include <boost/assign/list_of.hpp>

using namespace std;
using boost::assign::list_of;

CbmAnaConversion::CbmAnaConversion()
  : FairTask("CbmAnaConversion"),
    fRichPoints(NULL),
    fMcTracks(NULL),
    fEventNum(0)
{
}

CbmAnaConversion::~CbmAnaConversion()
{

}

InitStatus CbmAnaConversion::Init()
{
   cout << "CbmAnaConversion::Init"<<endl;
   FairRootManager* ioman = FairRootManager::Instance();
   if (NULL == ioman) { Fatal("CbmAnaConversion::Init","RootManager not instantised!"); }

   fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
   if ( NULL == fRichPoints) { Fatal("CbmAnaConversion::Init","No RichPoint array!"); }

   fMcTracks = (TClonesArray*) ioman->GetObject("MCTrack");
   if ( NULL == fMcTracks) { Fatal("CbmAnaConversion::Init","No MCTrack array!"); }

   fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
   if ( NULL == fStsTracks) { Fatal("CbmAnaConversion::Init","No StsTrack array!"); }

   fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
  if (NULL == fStsTrackMatches) { Fatal("CbmAnaConversion::Init","No StsTrackMatch array!"); }

	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if (NULL == fGlobalTracks) { Fatal("CbmAnaConversion::Init","No GlobalTrack array!"); }

   InitHistograms();
   
   
   testint = 0;

   return kSUCCESS;
}

void CbmAnaConversion::InitHistograms()
{
	fHistoList.clear();
	
	fhGammaZ = new TH1D("fhGammaZ", "fhGammaZ;Z [cm];Entries", 300, 0.0, 300.);
	fhGammaZ_selected = new TH1D("fhGammaZ_selected", "fhGammaZ_selected;Z [cm];Entries", 600, 0.0, 600.);
	
	fTest = new TH2D("fTest", "fTest; X; Y", 200, -100, 100, 400, -200, 200);
	
	fhNofElPrim = new TH1D("fhNofElPrim", "fhNofElPrim;Nof prim El;Entries", 10., -0.5, 9.5);
	fhNofElSec = new TH1D("fhNofElSec", "fhNofElSec;Nof Sec El;Entries", 20., -0.5, 19.5);
	fhNofElAll = new TH1D("fhNofElAll", "fhNofElAll;Nof All El;Entries", 30., -0.5, 29.5);
	
	fhTomography = new TH3D("fhTomography", "Tomography/fhTomography;X;Y;Z", 200, -200., 200., 200, -200., 200., 500, 0., 1000.);
	fhTomography_XZ = new TH2D("fhTomography_XZ", "fhTomography_XZ;X;Z", 1600, -400., 400., 2400, 0., 1200.);
	fhTomography_YZ = new TH2D("fhTomography_YZ", "fhTomography_YZ;Y;Z", 1600, -400., 400., 2400, 0., 1200.);
	fHistoList.push_back(fhTomography);
	fHistoList.push_back(fhTomography_XZ);
	fHistoList.push_back(fhTomography_YZ);
	
	fhTomography_RICH_beampipe = new TH2D("fhTomography_RICH_beampipe", "fhTomography_RICH_beampipe;X;Y", 400, -100., 100., 200, -50., 50.);
	fHistoList.push_back(fhTomography_RICH_beampipe);
	
	
	fhConversion = new TH1D("fhConversion", "fhConversion;Z;# conversions", 1200, 0., 1200.);
	fHistoList.push_back(fhConversion);
	fhConversion_prob = new TH1D("fhConversion_prob", "fhConversion_prob;Z;# conversions", 1200, 0., 1200.);
	fHistoList.push_back(fhConversion_prob);
	
	
	fhInvariantMass_test = new TH1D("fhInvariant", "fhInvariant;mass;#", 1000, 0., 1000.);
	fHistoList.push_back(fhInvariantMass_test);


}

void CbmAnaConversion::Exec(
      Option_t* option)
{
   fEventNum++;

   cout << "CbmAnaConversion, event No. " <<  fEventNum << endl;

   Int_t nofPoints = fRichPoints->GetEntriesFast();
   for (int i = 0; i < nofPoints; i++) {
	   CbmRichPoint* point = (CbmRichPoint*)fRichPoints->At(i);
	   if (point == NULL) continue;
	   //cout << point->GetX() << "\t" << point->GetY() << endl;
	   fTest->Fill(point->GetX(), point->GetY());
   }
   
   
   TomographyMC();


   Int_t nofMcTracks = fMcTracks->GetEntriesFast();
   int countPrimEl = 0;
   int countSecEl = 0;
   int countAllEl  = 0;
   int countGammaEl = 0;
   int countMothers = 0;
   int countPrimPart = 0;
   for (int i = 0; i < nofMcTracks; i++) {

	   CbmMCTrack* mctrack = (CbmMCTrack*)fMcTracks->At(i);
	   if (mctrack == NULL) continue;
	   
	   if (mctrack->GetMotherId() == -1) {
	   		countPrimPart++;
	   }
	   
	   if (mctrack->GetMotherId() == -1 && TMath::Abs( mctrack->GetPdgCode())  == 11) {
		  countPrimEl++;
	   }
	   
	   if (mctrack->GetMotherId() != -1 && TMath::Abs( mctrack->GetPdgCode())  == 11) {
		  countSecEl++;
	   }
	   
	   if (TMath::Abs( mctrack->GetPdgCode())  == 11) {
		   countAllEl++;
	   }

	   if (TMath::Abs( mctrack->GetPdgCode())  == 11) {
		   int motherId = mctrack->GetMotherId();
		   if (motherId == -1) continue;
	   	   CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
	   	   int mcMotherPdg  = -1;
	       if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
	       //cout << mcMotherPdg << endl;

		  if (mcMotherPdg == 22) {
			  TVector3 v;
			  mctrack->GetStartVertex(v);
			  fhGammaZ->Fill(v.Z());
			  countGammaEl++;
		  }
	   }
	   
	   
	   
   }
   
   
	fhNofElPrim->Fill(countPrimEl);
	fhNofElSec->Fill(countSecEl);
	fhNofElAll->Fill(countAllEl);
   
   if(countPrimEl == 2 && countAllEl == 4) {
   		testint++;
   		for (int i = 0; i < nofMcTracks; i++) {
			CbmMCTrack* mctrack = (CbmMCTrack*)fMcTracks->At(i);
			if (TMath::Abs( mctrack->GetPdgCode())  == 11) {
				int motherId = mctrack->GetMotherId();
				if (motherId == -1) continue;
	   			CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
	   			int mcMotherPdg  = -1;
	    		if (NULL != mother) mcMotherPdg = mother->GetPdgCode();
	    
				if (mcMotherPdg == 22) {
					TVector3 v;
					mctrack->GetStartVertex(v);
					fhGammaZ_selected->Fill(v.Z());
		  		}
	   		}
	   }
   }

   cout << "Nof Prim El = " << countPrimEl << endl;
   cout << "Nof Sec El = " << countSecEl << endl;
   cout << "Nof All El = " << countAllEl << endl;
   cout << "Nof Gamma El = " << countGammaEl << endl;
   cout << "Nof McTracks = " << nofMcTracks << endl;
   cout << "Nof PrimPart = " << countPrimPart << endl;
   
   

   Int_t ngTracks = fGlobalTracks->GetEntriesFast();
   for (Int_t i = 0; i < ngTracks; i++) {
       CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(i);
       if(NULL == gTrack) continue;
       int stsInd = gTrack->GetStsTrackIndex();
       int richInd = gTrack->GetRichRingIndex();
       int trdInd = gTrack->GetTrdTrackIndex();
       int tofInd = gTrack->GetTofHitIndex();

       if (stsInd < 0) continue;
       CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(stsInd);
       if (stsTrack == NULL) continue;
       CbmTrackMatchNew* stsMatch  = (CbmTrackMatchNew*) fStsTrackMatches->At(stsInd);
       if (stsMatch == NULL) continue;
       int stsMcTrackId = stsMatch->GetMatchedLink().GetIndex();
       if (stsMcTrackId < 0) continue;
       CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMcTracks->At(stsMcTrackId);
       if (mcTrack1 == NULL) continue;
       int pdg = TMath::Abs(mcTrack1->GetPdgCode());
       int motherId = mcTrack1->GetMotherId();
       double momentum = mcTrack1->GetP();
       stsMatch->GetTrueOverAllHitsRatio();
   }

   // example of how to get refitted momenta at primary vertex
  /*
   CbmL1PFFitter fPFFitter;
   vector<CbmStsTrack> stsTracks;
   stsTracks.resize(1);
   stsTracks[0] = *stsTrack;
   vector<L1FieldRegion> vField;
   vector<float> chiPrim;
   fPFFitter.GetChiToVertex(stsTracks, vField, chiPrim, fKFVertex, 3e6);
   cand.chi2sts = stsTracks[0].GetChiSq() / stsTracks[0].GetNDF();
   cand.chi2Prim = chiPrim[0];
   const FairTrackParam* vtxTrack = stsTracks[0].GetParamFirst();

   vtxTrack->Position(cand.position);
   vtxTrack->Momentum(cand.momentum);*/

}

/*KinematicParams CbmAnaDielectronTask::CalculateKinematicParams(
      const CbmMCTrack* mctrackP,
      const CbmMCTrack* mctrackM)
{
    KinematicParams params;

    TVector3 momP;  //momentum e+
    mctrackP->GetMomentum(momP);
    Double_t energyP = TMath::Sqrt(momP.Mag2() + M2E);
    TLorentzVector lorVecP(momP, energyP);

    TVector3 momM;  //momentum e-
    mctrackM->GetMomentum(momM);
    Double_t energyM = TMath::Sqrt(momM.Mag2() + M2E);
    TLorentzVector lorVecM(momM, energyM);

    TVector3 momPair = momP + momM;
    Double_t energyPair = energyP + energyM;
    Double_t ptPair = momPair.Perp();
    Double_t pzPair = momPair.Pz();
    Double_t yPair = 0.5*TMath::Log((energyPair+pzPair)/(energyPair-pzPair));
    Double_t anglePair = lorVecM.Angle(lorVecP.Vect());
    Double_t theta = 180.*anglePair/TMath::Pi();
    Double_t minv = 2.*TMath::Sin(anglePair / 2.)*TMath::Sqrt(momM.Mag()*momP.Mag());

    params.momentumMag = momPair.Mag();
    params.pt = ptPair;
    params.rapidity = yPair;
    params.minv = minv;
    params.angle = theta;
    return params;
}*/


void CbmAnaConversion::Finish()
{
	Probability();
	
	TCanvas* c = new TCanvas();
	c->SetWindowSize(800, 1600);
	DrawH1(fTest);
	//fhGammaZ->Write();
	
	TCanvas* c2 = new TCanvas();
	c2->SetWindowSize(2000, 1600);
	DrawH1(fhGammaZ_selected);
	
	TCanvas* c3 = new TCanvas();
	c3->SetWindowSize(2000, 1600);
	gPad->Divide(3,1);
	c3->cd(1);
	DrawH1(fhNofElPrim);
	c3->cd(2);
	DrawH1(fhNofElSec);
	c3->cd(3);
	DrawH1(fhNofElAll);
	
	
	//TCanvas* c4 = new TCanvas();
	//c4->SetWindowSize(2000, 1600);
	//DrawH1(fhTomography);
	
	
	// Write histograms to a file
	gDirectory->mkdir("analysis-conversion");
	gDirectory->cd("analysis-conversion");
	for (Int_t i = 0; i < fHistoList.size(); i++){
		fHistoList[i]->Write();
	}
	gDirectory->cd("..");

}


void CbmAnaConversion::TomographyMC()
{
	Int_t nofMcTracks = fMcTracks->GetEntriesFast();
	for (int i = 0; i < nofMcTracks; i++) {
		CbmMCTrack* mctrack = (CbmMCTrack*)fMcTracks->At(i);
		
		if (mctrack == NULL) continue;
	   
		if (TMath::Abs( mctrack->GetPdgCode())  == 11) { 
			int motherId = mctrack->GetMotherId();
			if (motherId == -1) continue;
			CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
			int mcMotherPdg  = -1;
			if (NULL != mother) mcMotherPdg = mother->GetPdgCode();

			if (mcMotherPdg == 22) {
				TVector3 v;
				mctrack->GetStartVertex(v);
				fhGammaZ->Fill(v.Z());
			  
				fhTomography->Fill(v.X(), v.Y(), v.Z());
				fhTomography_XZ->Fill(v.X(), v.Z());
				fhTomography_YZ->Fill(v.Y(), v.Z());
				if(v.z() >= 220 && v.Z() <= 280) {
					fhTomography_RICH_beampipe->Fill(v.X(), v.Y());
				}
				
				fhConversion->Fill(v.z());
			}
		}
	}
}

/*
void CbmAnaConversion::TomographyReco()
{
	Int_t ngTracks = fGlobalTracks->GetEntriesFast();
	for (int i = 0; i < ngTracks; i++) {
		CbmGlobalTrack* gTrack = (CbmGlobalTrack*) fGlobalTracks->At(i);
		
		if (gTrack == NULL) continue;
	   
		if (TMath::Abs( gTrack->GetPdgCode())  == 11) {
			int motherId = gTrack->GetMotherId();
			if (motherId == -1) continue;
			CbmGlobalTrack* mother = (CbmGlobalTrack*) fGlobalTracks->At(motherId);
			int mcMotherPdg  = -1;
			if (NULL != mother) mcMotherPdg = mother->GetPdgCode();

			if (mcMotherPdg == 22) {
				TVector3 v;
				gTrack->GetStartVertex(v);
			  
				fhTomography->Fill(v.X(), v.Y(), v.Z());
				fhTomography_XZ->Fill(v.X(), v.Z());
				fhTomography_YZ->Fill(v.Y(), v.Z());
			}
		}
	}

}
*/



void CbmAnaConversion::Probability() 
{
	for(int i=1; i<=fhConversion->GetNbinsX(); i++) {
		int temp = 0;
		for(int run=1; run<=i; run++ ) {
			temp += fhConversion->GetBinContent(run);
		}
		fhConversion_prob->SetBinContent(i, temp);
	}
}



void CbmAnaConversion::InvariantMassTest() 
{
/*	Int_t nofMcTracks = fMcTracks->GetEntriesFast();
	for (int i = 0; i < nofMcTracks; i++) {
		CbmMCTrack* mctrack = (CbmMCTrack*)fMcTracks->At(i);
		
		if (mctrack == NULL) continue;
	   
		if (TMath::Abs( mctrack->GetPdgCode())  == 22) { 
			int motherId = mctrack->GetMotherId();
			if (motherId == -1) continue;
			CbmMCTrack* mother = (CbmMCTrack*) fMcTracks->At(motherId);
			int mcMotherPdg  = -1;
			if (NULL != mother) mcMotherPdg = mother->GetPdgCode();

			if (mcMotherPdg == 111) {	// pdf code 111 = pi0
				TVector3 v;
				mctrack->GetStartVertex(v);
				fhGammaZ->Fill(v.Z());
			}
		}
	}
*/
}



int CbmAnaConversion::GetTest()
{
	return testint;
}


int CbmAnaConversion::GetNofEvents()
{
	return fEventNum;
}



ClassImp(CbmAnaConversion)

