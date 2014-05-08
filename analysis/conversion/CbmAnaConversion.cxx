/**
 * \file CbmAnaConversion.cxx
 *
 * \author Tariq Mahmoud <t.mahmoud@gsi.de>
 * \date 2012
 **/

#include "CbmAnaConversion.h"

#include "TH1D.h"
#include "TH1.h"
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

   return kSUCCESS;
}

void CbmAnaConversion::InitHistograms()
{
	fhGammaZ = new TH1D("fhGammaZ", "fhGammaZ;Z [cm];Entries", 100, 0.0, 100.);

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
	   //cout << point->GetX() << endl;
   }


   Int_t nofMcTracks = fMcTracks->GetEntriesFast();
   int countPrimEl = 0;
   int countAllEl  = 0;
   int countGammaEl = 0;
   for (int i = 0; i < nofMcTracks; i++) {

	   CbmMCTrack* mctrack = (CbmMCTrack*)fMcTracks->At(i);
	   if (mctrack == NULL) continue;
	   if (mctrack->GetMotherId() == -1 && TMath::Abs( mctrack->GetPdgCode())  == 11) {
		  countPrimEl++;
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
	       cout << mcMotherPdg << endl;

		  if (mcMotherPdg == 22) {
			  TVector3 v;
			  mctrack->GetStartVertex(v);
			  fhGammaZ->Fill(v.Z());
			  countGammaEl++;
		  }
	   }
   }

   cout << "Nof Prim El = " << countPrimEl << endl;
   cout << "Nof All El = " << countAllEl << endl;
   cout << "Nof Gamma El = " << countGammaEl << endl;


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
	TCanvas* c = new TCanvas();
	DrawH1(fhGammaZ);
	//fhGammaZ->Write();

}



ClassImp(CbmAnaConversion)

