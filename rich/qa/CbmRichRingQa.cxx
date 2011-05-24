/** CbmRichRingQa.cxx
 * @author Semen Lebedev <s.lebedev@gsi.de>
 * @since 2005
 * @version 3.0
 **/

#include "CbmRichRingQa.h"

#include "CbmRichPoint.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"
#include "CbmRichRingFitterCOP.h"
#include "CbmRichRingSelectImpl.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmMCTrack.h"
#include "CbmTrackMatch.h"

#include "TCanvas.h"
#include "TH1D.h"

#include <map>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::map;
using std::vector;

CbmRichRingQa::CbmRichRingQa() :FairTask("RichRingQa")
{

}

CbmRichRingQa::CbmRichRingQa(
      const char *name,
      const char *title,
      Int_t verbose)
  :FairTask(name)
{
    fVerbose = verbose;
    fNofHitsInRingCut = 7; /// minimum number of hits in ring
    fQuota = 0.6;

    fEventNumber = 0;

    fNofAccElRings = 0;
    fNofRecElRings = 0;
    fNofCloneRings = 0;
    fNofFakeRings = 0;

/// Difference Fake and True rings histograms BEGIN
    fh_FakeNofHits = new TH1D("fh_FakeNofHits","Number of hits in ring;Nof hits in ring;Yield",50,0,50);
    fh_TrueElNofHits= new TH1D("fh_TrueElNofHits","Number of hits in ring;Nof hits in ring;Yield",50,0,50);

    fh_FakeAngle = new TH1D("fh_FakeAngle","Biggest angle in ring;Angle, rad;Yield",50,0,6.5);
    fh_TrueElAngle = new TH1D("fh_TrueElAngle","Biggest angle in ring;Angle, rad;Yield",50,0,6.5);

    fh_FakeNofHitsOnRing= new TH1D("fh_FakeNofHitsOnRing","Number of hits on ring;Nof hits on ring;Yield",50,0,50);
    fh_TrueElNofHitsOnRing= new TH1D("fh_TrueElNofHitsOnRing","Number of hits on ring;Nof hits on ring;Yield",50,0,50);

    fh_FakeChi2 = new TH1D("fh_FakeChi2","Chi2;Chi2;Yield",50,0.,1.);
    fh_TrueElChi2 = new TH1D("fh_TrueElChi2","Chi2;Chi2;Yield",50,0.,1.);

    fh_FakeRadPos = new TH1D("fh_FakeRadPos","Radial position;Radial position, cm;Yield",150,0,150);
    fh_TrueElRadPos = new TH1D("fh_TrueElRadPos","Radial position;Radial position, cm;Yield",150,0,150);;

    fh_FakeRadius = new TH1D("fh_FakeRadius","Radius;Radius, cm;Yield",90,0,9);;
    fh_TrueElRadius = new TH1D("fh_TrueElRadius","Radius;Radius, cm;Yield",90,0,9);;
/// Difference Fake and True rings histograms END

   //foutFakeAndTrue.open("ann_fake_and_true.txt",std::ios_base::app);
   foutFakeAndTrue.open("ann_fake_and_true.txt");
}

CbmRichRingQa::~CbmRichRingQa()
{


}

InitStatus CbmRichRingQa::Init()
{
   cout << "InitStatus CbmRichRingQa::Init()" << endl;
	FairRootManager* ioman = FairRootManager::Instance();
	if (NULL == ioman) { Fatal("CbmRichRingQa::Init","CbmRootManager is not instantiated"); }

	fHits = (TClonesArray*) ioman->GetObject("RichHit");
	if (NULL == fHits) { Fatal("CbmRichRingQa::Init","No RichHit array!"); }

	fRings = (TClonesArray*) ioman->GetObject("RichRing");
	if (NULL == fRings) { Fatal("CbmRichRingQa::Init","No RichRing array!"); }

	fPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if (NULL == fPoints) { Fatal("CbmRichRingQa::Init","No RichPoint array!"); }

	fTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if (NULL == fTracks) { Fatal("CbmRichRingQa::Init","No MCTrack array!");}

	fMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if (NULL == fMatches) { Fatal("CbmRichRingQa::Init","No RichRingMatch array!");}

	fProj = (TClonesArray*) ioman->GetObject("RichProjection");
	if (NULL == fProj) { Fatal("CbmRichRingQa::Init","No RichProjection array!");}

	fTrackMatch = (TClonesArray*) ioman->GetObject("StsTrackMatch");
	if (NULL == fTrackMatch) { Fatal("CbmRichRingQa::Init","No track match array!");}

	gTrackArray = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if (NULL == gTrackArray) { Fatal("CbmRichRingQa::Init","No global track array!");}

	fFitCOP = new CbmRichRingFitterCOP();
	fFitCOP->Init();
   fSelectImpl = new CbmRichRingSelectImpl();
   fSelectImpl->Init();

   return kSUCCESS;
}

void CbmRichRingQa::Exec(
      Option_t* option)
{
	fEventNumber++;
   cout<<"CbmRichRingQa Event No. "<< fEventNumber << endl;

   map<Int_t, Int_t>::iterator it;//iterator to fRingMap
   fRingMap.clear();
   Int_t nRichHits = fHits->GetEntriesFast();

   for (Int_t iHit=0; iHit < nRichHits; iHit++) {
      CbmRichHit* hit = (CbmRichHit*) fHits->At(iHit);
      if ( NULL == hit )continue;

      Int_t iPoint = hit->GetRefId();
      if ( iPoint < 0 ) continue;

      FairMCPoint* point = (FairMCPoint*) fPoints->At(iPoint);
      if ( NULL == point ) continue;

      Int_t iMCTrack = point->GetTrackID();
      if ( iMCTrack < 0 ) continue;
      CbmMCTrack* track = (CbmMCTrack*)fTracks->At(iMCTrack);
      if ( NULL == track ) continue;

      Int_t iMother = track->GetMotherId();
      if (iMother == -1) continue;

      fRingMap[iMother]++;
    }// iHit

    ///Loop for all MC rings
    for (it=fRingMap.begin(); it!=fRingMap.end(); it++) {
       CbmMCTrack* track = (CbmMCTrack*) fTracks->At(it->first);
       if ( NULL == track) continue;

       Double_t momentum = track->GetP();
       Int_t trackID = it->first;
       Int_t gcode = TMath::Abs(track->GetPdgCode());
       Int_t motherId = track->GetMotherId();

       if (gcode == 11 && motherId == -1) {//primary electron rings
          if (it->second >= fNofHitsInRingCut){
             fNofAccElRings++;
          }
       }
    }

    EfficiencyCalc();
    DiffFakeTrueCircle();

    Double_t elEff = 0.;
    if (fNofAccElRings != 0.) elEff = 100.*(Double_t)fNofRecElRings/(Double_t)fNofAccElRings;
    cout << "El efficiency = "<< elEff << "% (" << (Double_t)fNofRecElRings/fEventNumber <<
          "/" << (Double_t)fNofAccElRings/fEventNumber << ")" << endl;

    cout << "fNofCloneRings = " << (Double_t)fNofCloneRings/fEventNumber<< " per event" << endl;
    cout << "fNofFakeRings = " << (Double_t)fNofFakeRings/fEventNumber << " per event" << endl;
}

Double_t CbmRichRingQa::GetStsMomentum(CbmRichRing * ring)
{
    Int_t ringTrackId = ring->GetTrackID();

    if (ringTrackId == -1) return 0.0;

    CbmGlobalTrack* gtrack = (CbmGlobalTrack*)gTrackArray->At(ringTrackId);
    if (gtrack->GetStsTrackIndex() == -1) return 0.0;
    CbmTrackMatch* stsMatch = (CbmTrackMatch*) fTrackMatch->At(gtrack->GetStsTrackIndex());
    if (!stsMatch){
        cout << "-E- CbmRichRingQa::GetStsMomentum() no matched track!: fake?"<< endl;
        return 0.0;
    }

    CbmMCTrack* track = (CbmMCTrack*) fTracks->At(stsMatch->GetMCTrackId());
    if (!track) {
        cout << "-E- CbmRichRingQa::GetStsMomentum() No track " << endl;
        return 0.0;
    }

    return track->GetP();
}

void CbmRichRingQa::EfficiencyCalc()
{
    Int_t nMatches = fMatches->GetEntriesFast();
    cout<<"Number of found rings = "<<nMatches<<endl;

    vector<Int_t> clone;
    clone.clear();

    for (Int_t iMatches = 0; iMatches < nMatches; iMatches++){
       CbmRichRingMatch* match = (CbmRichRingMatch*)fMatches->At(iMatches);
       if (NULL == match) continue;

       CbmRichRing* ring = (CbmRichRing*)fRings->At(iMatches);
       if (NULL == ring) continue;

       Int_t lTrueHits = match->GetNofTrueHits();
       Int_t lWrongHits = match->GetNofWrongHits();
       Int_t lFakeHits = match->GetNofFakeHits();
       Int_t lMCHits = match->GetNofMCHits();
       Int_t lFoundHits = lTrueHits + lWrongHits + lFakeHits;
       Double_t lPercTrue = 0;
       if (lFoundHits >= 3){
          lPercTrue = (Double_t)lTrueHits / (Double_t)lFoundHits;
       }

       Int_t trackID = match->GetMCTrackID();
       if (trackID > fTracks->GetEntriesFast() || trackID < 0) continue;
       CbmMCTrack* track = (CbmMCTrack*) fTracks->At(trackID);
       if (NULL == track) continue;

       Int_t gcode = TMath::Abs(track->GetPdgCode());
       Double_t momentum = track->GetP();
       Int_t motherId = track->GetMotherId();

       ring->SetRecFlag(-1);

       if (lPercTrue < fQuota){
          ring->SetRecFlag(1);
       }else{// true rings
          if (gcode == 11 && motherId == -1 && lMCHits >= fNofHitsInRingCut){
             ring->SetRecFlag(3);
          }
          // search for clone rings
          for (UInt_t ci = 0; ci < clone.size(); ci++)
             if (trackID == clone[ci]) ring->SetRecFlag(2);
          clone.push_back(trackID);
       }// true rings

       Int_t recFlag = ring->GetRecFlag();
       if (recFlag ==2) fNofCloneRings++;
       if (recFlag == 1) fNofFakeRings++;
       if (recFlag == 3) fNofRecElRings++;
    }// iMatches
}

void CbmRichRingQa::DiffFakeTrueCircle()
{
	Int_t nMatches = fMatches->GetEntriesFast();

	for (Int_t iMatches = 0; iMatches < nMatches; iMatches++) {
	   CbmRichRingMatch* match = (CbmRichRingMatch*) fMatches->At(iMatches);
		if (NULL == match) {
			cout << "-E- CbmRichRingQa::DiffFakeTrueCOP() no match" << iMatches	<< endl;
			continue;
		}
		CbmRichRing* ring = (CbmRichRing*) fRings->At(iMatches);
		if (NULL == ring) {
			cout << "-E- CbmRichRingQa::DiffFakeTrueCOP() no ring" << iMatches	<< endl;
			continue;
		}
		fFitCOP->DoFit(ring);

		Int_t recFlag = ring->GetRecFlag();
		Double_t angle = fSelectImpl->GetAngle(ring);
		Int_t hitsOnRing = fSelectImpl->GetNofHitsOnRingCircle(ring);
		Double_t chi2 = ring->GetChi2() / ring->GetNDF();
		Int_t nHits = ring->GetNofHits();
		Double_t radPos = ring->GetRadialPosition();
		Double_t radius = ring->GetRadius();

	   if (recFlag == 1) fh_FakeNofHits->Fill(nHits);
	   if (recFlag == 3) fh_TrueElNofHits->Fill(nHits);

	   if (recFlag == 1) fh_FakeAngle->Fill(angle);
	   if (recFlag == 3)  fh_TrueElAngle->Fill(angle);

	   if (recFlag == 1) fh_FakeNofHitsOnRing->Fill(hitsOnRing);
	   if (recFlag == 3) fh_TrueElNofHitsOnRing->Fill(hitsOnRing);

	   if (recFlag == 1) fh_FakeRadPos->Fill(radPos);
	   if (recFlag == 3) fh_TrueElRadPos->Fill(radPos);

	   if (recFlag == 1) fh_FakeChi2->Fill(chi2);
	   if (recFlag == 3) fh_TrueElChi2->Fill(chi2);

	   if (recFlag == 1) fh_FakeRadius->Fill(radius);
	   if (recFlag == 3) fh_TrueElRadius->Fill(radius);

		if (TMath::IsNaN(nHits)	|| TMath::IsNaN(angle) || TMath::IsNaN(hitsOnRing) ||
			TMath::IsNaN(radPos) || TMath::IsNaN(radius)|| TMath::IsNaN(chi2)) continue;

		if (nHits < 3) continue;
		if (chi2 > 10000000000.) continue;

		if (recFlag == 1)
			foutFakeAndTrue << nHits << " " << angle << " "	<< hitsOnRing << " " <<
            radPos << " " << radius << " " << chi2 << " " << -1 << endl;

		if (recFlag == 3)
			foutFakeAndTrue << nHits << " " << angle << " "	<< hitsOnRing << " " <<
            radPos << " " << radius << " " << chi2 << " " << 1 << endl;
	}// iMatches
}

void CbmRichRingQa::FinishTask()
{
    TDirectory *current = gDirectory;
    TDirectory *rich = current->mkdir("RichRingQaHist");
    rich->cd();

/// Difference Fake and True rings histograms BEGIN
    fh_FakeNofHits->Scale(1./fh_FakeNofHits->Integral());
    fh_TrueElNofHits->Scale(1./fh_TrueElNofHits->Integral());
    fh_FakeNofHits->Write();
    fh_TrueElNofHits->Write();

    fh_FakeAngle->Scale(1./fh_FakeAngle->Integral());
    fh_TrueElAngle->Scale(1./fh_TrueElAngle->Integral());
    fh_FakeAngle->Write();
    fh_TrueElAngle->Write();

    fh_FakeNofHitsOnRing->Scale(1./fh_FakeNofHitsOnRing->Integral());
    fh_TrueElNofHitsOnRing->Scale(1./fh_TrueElNofHitsOnRing->Integral());
    fh_FakeNofHitsOnRing->Write();
    fh_TrueElNofHitsOnRing->Write();

    fh_FakeRadPos->Scale(1./fh_FakeRadPos->Integral());
    fh_TrueElRadPos->Scale(1./fh_TrueElRadPos->Integral());
    fh_FakeRadPos->Write();
    fh_TrueElRadPos->Write();

    fh_FakeChi2->Scale(1./fh_FakeChi2->Integral());
    fh_TrueElChi2->Scale(1./fh_TrueElChi2->Integral());
    fh_FakeChi2->Write();
    fh_TrueElChi2->Write();

    fh_FakeRadius->Scale(1./fh_FakeRadius->Integral());
    fh_TrueElRadius->Scale(1./fh_TrueElRadius->Integral());
    fh_FakeRadius->Write();
    fh_TrueElRadius->Write();
/// Difference Fake and True rings histograms END

    rich->cd();
    current->cd();

    delete fFitCOP;
    delete fSelectImpl;
}

ClassImp(CbmRichRingQa)
