/*
Class  : CbmRichRingQa
Description : Quality checks or ring finders: efficiency calculation etc.
Author : Semen Lebedev (S.Lebedev@gsi.de)
*/

#include "CbmRichRingQa.h"

#include "CbmRichPoint.h"
#include "CbmRichHit.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"

#include "FairRootManager.h"
#include "CbmMCTrack.h"
#include "FairTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmMCTrack.h"
#include "CbmTrackMatch.h"

#include <map>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::map;
using std::vector;

// -----   Default constructor   -------------------------------------------
CbmRichRingQa::CbmRichRingQa() :FairTask("RichRingQa")
{

}
// -------------------------------------------------------------------------

//------------  standard constructor (with verbosity level)  ---------------------------------
CbmRichRingQa::CbmRichRingQa(const char *name, const char *title, Int_t verbose)
  :FairTask(name)
{
    // verbosity level
    fVerbose = verbose;

    fNofHitsInRingCut = 7; /// minumum number of hits in ring

    // count events
    fEventNumber = 0;

    fNofAllRings = 0;
    fNofMcElRings = 0;
    fNofMcPiRings = 0;
    fNofAccElRings = 0;
    fNofAccPiRings = 0;

    fNofRecElRings = 0;
    fNofRecPiRings = 0;
    fNofCloneRings = 0;
    fNofFakeRings = 0;

    fNofTrueElBadFit = 0;

    fh_RecElRingsMom = new TH1D("fh_RecElRingsMom","fh_RecElRingsMom",40,0,10);
    fh_AccElRingsMom = new TH1D("fh_AccElRingsMom","fh_AccElRingsMom",40,0,10);

    fh_RecElRingsRadPos = new TH1D("fh_RecElRingsRadPos","fh_RecElRingsRadPos",40,0,150);
    fh_AccElRingsRadPos = new TH1D("fh_AccElRingsRadPos","fh_AccElRingsRadPos",40,0,150);

    fh_RecElRingsNofHits = new TH1D("fh_RecElRingsNofHits","fh_RecElRingsNofHits",20,0,40);
    fh_AccElRingsNofHits = new TH1D("fh_AccElRingsNofHits","fh_AccElRingsNofHits",20,0,40);

    fh_RecElRingsBoverA = new TH1D("fh_RecElRingsBoverA","fh_RecElRingsBoverA",20,0,1);
    fh_AccElRingsBoverA = new TH1D("fh_AccElRingsBoverA","fh_AccElRingsBoverA",20,0,1);

/// Difference Fake and True rings histogramms BEGIN
    fh_FakeNofHits = new TH1D("fh_FakeNofHits","Number of hits in ring;Nof hits in ring;Yield",50,0,50);
    fh_TrueElNofHits= new TH1D("fh_TrueElNofHits","Number of hits in ring;Nof hits in ring;Yield",50,0,50);

    fh_FakeAngle = new TH1D("fh_FakeAngle","Biggest angle in ring;Angle, rad;Yield",50,0,6.5);
    fh_TrueElAngle = new TH1D("fh_TrueElAngle","Biggest angle in ring;Angle, rad;Yield",50,0,6.5);

    fh_FakeNofHitsOnRing= new TH1D("fh_FakeNofHitsOnRing","Number of hits on ring;Nof hits on ring;Yield",50,0,50);
    fh_TrueElNofHitsOnRing= new TH1D("fh_TrueElNofHitsOnRing","Number of hits on ring;Nof hits on ring;Yield",50,0,50);

    fh_FakeChi2 = new TH1D("fh_FakeChi2","Chi2;Chi2;Yield",50,0.,1.);
    fh_TrueElChi2 = new TH1D("fh_TrueElChi2","Chi2;Chi2;Yield",50,0.,1.);

    fh_FakeRadPos = new TH1D("fh_FakeRadPos","Radial position;Radial position, cm;Yield",200,0,200);;
    fh_TrueElRadPos = new TH1D("fh_TrueElRadPos","Radial position;Radial position, cm;Yield",200,0,200);;

    fh_FakeRadius = new TH1D("fh_FakeRadius","Radius;Radius, cm;Yield",90,0,9);;
    fh_TrueElRadius = new TH1D("fh_TrueElRadius","Radius;Radius, cm;Yield",90,0,9);;
/// Difference Fake and True rings histogramms END

    fh_WrongMatchElDistance = new TH1D("fh_WrongMatchElDistance","Distance between track and ring center",50,0,5);
    fh_TrueMatchElDistance = new TH1D("fh_TrueMatchElDistance","Distance between track and ring center",50,0,5);
    fh_TrueMatchElMom = new TH1D("fh_TrueMatchElMom","fh_TrueMatchElMom",40,0,10);

    //hits distribution (x,y)
    fh_HitsXY = new TH2D("fh_HitsXY","Hits distribution (x,y), hits/cm^2/event;X, cm;Y, cm",400,-200,200,500,-250,250);
    //number of hits per event
    fh_NhitsPerEvent = new TH1D("fh_NhitsPerEvent","Number of hits per event",100,1000,3000);
    //number of projections per event
    fh_NprojPerEvent = new TH1D("fh_NprojPerEvent","Number of projections per event",50,200,450);
    //foutFakeAndTrue.open("ann_fake_and_true.txt",std::ios_base::app);

   foutFakeAndTrue.open("ann_fake_and_true.txt");
}

  // -----   Destructor   ----------------------------------------------------
CbmRichRingQa::~CbmRichRingQa()
{


}

InitStatus CbmRichRingQa::Init()
{
    cout << "InitStatus CbmRichRingQa::Init()" << endl;
	// Get and check CbmRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		cout << "-E- CbmRichRingQa::Init: " << "RootManager not instantised!"<< endl;
		return kERROR;
	}

	// Get hit Array
	fHits = (TClonesArray*) ioman->GetObject("RichHit");
	if (!fHits) {
		cout << "-W- CbmRichRingQa::Init: No RichHit array!" << endl;
	}

	// Get RichRing Array
	fRings = (TClonesArray*) ioman->GetObject("RichRing");
	if (!fRings) {
		cout << "-E- CbmRichRingQa::Init: No RichRing array!" << endl;
		return kERROR;
	}

	// Get MC Point array
	fPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if (!fPoints) {
		cout << "-E- CbmRichRingQa::Init: No RichPoint array!" << endl;
		return kERROR;
	}

	// Get MC Point array
	fTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if (!fTracks) {
		cout << "-E- CbmRichRingQa::Init: No MCTrack array!" << endl;
		return kERROR;
	}

	// Get RichRingMatch array
	fMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if (!fMatches) {
		cout << "-E- CbmRichRingQa::Init: No RichRingMatch array!" << endl;
		return kERROR;
	}

	// Get RichProjection array
	fProj = (TClonesArray*) ioman->GetObject("RichProjection");
	if (!fProj) {
		cout << "-E- CbmRichRingQa::Init: No RichProjection array!" << endl;
		//return kERROR;
	}

	// get TrackMatch array
	fTrackMatch = (TClonesArray*) ioman->GetObject("STSTrackMatch");
	if (!fTrackMatch) {
		cout << "-E- CbmRichRingQa::Init: No track match array!" << endl;
		//return kERROR;
	}

	// Get global track array
	gTrackArray = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if (!gTrackArray) {
		cout << "-W- CbmRichRingQa::Init: No global track array!" << endl;
		//return kERROR;
	}

    fFitCOP = new CbmRichRingFitterCOP();
    fFitCOP->Init();
    fFitEllipse = new CbmRichRingFitterEllipseTau(0,0,"compact");
    fFitEllipse->Init();
    fSelectImpl = new CbmRichRingSelectImpl();
    fSelectImpl->Init();

    return kSUCCESS;

}

  //-----------------------------------------------------------------------------

// -----   Execution of Task   ---------------------------------------------

// -------------------------------------------------------------------------
void CbmRichRingQa::Exec(Option_t* option)
{
	fEventNumber++;
    cout<<"CbmRichRingQa Event No. "<< fEventNumber << endl;

    CbmRichHit* hit    = NULL;
    FairMCPoint* point  = NULL;
    CbmMCTrack* track  = NULL;

    Int_t iPoint       = 0;
    Int_t iMCTrack     = 0;
    Int_t iMother      = 0;

    map<Int_t, Int_t>::iterator it;//iterator to fRingMap
    map<Int_t, CbmRichRing >::iterator itMapWithHits;

    //calculate number of hits in MC ring to
    //compare its with number of hits in found ring
    fRingMap.clear();
    fRingMapWithHits.clear();

    Int_t nRichHits = fHits->GetEntriesFast();
    fh_NhitsPerEvent->Fill(nRichHits);
    //fh_NprojPerEvent->Fill(fProj->GetEntriesFast());

    /// Loop over Rich hits
    for (Int_t iHit=0; iHit < nRichHits; iHit++) {
        hit = (CbmRichHit*) fHits->At(iHit);
        if ( ! hit ) {
        cout << "-E- CbmRichMatchRings::Exec: "
            << "No Hit " << iHit << endl;
        continue;
        }

        fh_HitsXY->Fill(hit->GetX(), hit->GetY());
        iPoint = hit->GetRefId();

        if ( iPoint < 0 )continue; // Fake or background hit
        //Get the MC Point corresponding to the hit

        point = (FairMCPoint*) fPoints->At(iPoint);
        if ( ! point ) {
        cout << "-E- CbmRichRingQa::Exec() "
            << "Empty MCPoint " << iPoint << " from Hit " << iHit
            << endl;
        continue;
        }
        //Get the MC Track ID corresponding to the MC Point
        iMCTrack = point->GetTrackID();
        // Get the MC Track corresponding to the ID
        track   = (CbmMCTrack*)fTracks->At(iMCTrack);
        if ( ! track ) {
        cout << "-E- CbmRichRingQa::Exec "
            << "Empty MCtrack " << iMCTrack
            << endl;
			continue;
        }
        iMother = track->GetMotherId();
        if (iMother == -1) {
           // cout << "-E- MotherID of Cherenkov photon = -1" << endl;
            continue;
        }
        fRingMap[iMother]++;
        fRingMapWithHits[iMother].AddHit(iHit);
    }  /// Loop over Rich hits


    ///Loop for all MC rings
    for (it=fRingMap.begin(); it!=fRingMap.end(); it++) {
        track = (CbmMCTrack*) fTracks->At(it->first);
        if (!track){
            cout << "-E- CbmRichRingQa::Exec() no track"<<
            it->first<<endl;
            continue;
        }

        Double_t momentum = track->GetP();
        Int_t trackID = it->first;

        Bool_t isProj = DoesRingHaveProjection(trackID);

        Int_t gcode = track->GetPdgCode();
        Int_t motherId = track->GetMotherId();

        fNofAllRings++;

        if ((TMath::Abs(gcode) == 11) && motherId == -1) {///primary electron rings
            fNofMcElRings++;
            if (isProj && it->second >= fNofHitsInRingCut){
                fNofAccElRings++;
                fh_AccElRingsMom->Fill(momentum);
                fh_AccElRingsNofHits->Fill(it->second);
            }
        }

        if ((TMath::Abs(gcode) == 211)) {///pion rings
            fNofMcPiRings++;
            if (isProj && it->second >= fNofHitsInRingCut) fNofAccPiRings++;
        }
    }  ///Loop for all MC rings

    for (itMapWithHits=fRingMapWithHits.begin(); itMapWithHits!=fRingMapWithHits.end(); itMapWithHits++) {
        track = (CbmMCTrack*) fTracks->At(itMapWithHits->first);
        Double_t momentum = track->GetP();
        if (!track){
            cout << "-E- CbmRichRingQa::Exec() no track"<< itMapWithHits->first<<endl;
            continue;
        }
        Int_t trackID = itMapWithHits->first;
        Bool_t isProj = DoesRingHaveProjection(trackID);
        Int_t gcode = track->GetPdgCode();
        Int_t motherId = track->GetMotherId();
        if ((TMath::Abs(gcode) == 11)&& motherId == -1) {///primary electron rings
            if (isProj && itMapWithHits->second.GetNofHits() >= fNofHitsInRingCut){
                fFitCOP->DoFit(&(itMapWithHits->second));
                fFitEllipse->DoFit(&(itMapWithHits->second));
                fh_AccElRingsRadPos->Fill(itMapWithHits->second.GetRadialPosition());
                Double_t bOverA = itMapWithHits->second.GetBaxis()/itMapWithHits->second.GetAaxis();
                fh_AccElRingsBoverA->Fill(bOverA);
            }
        }
    }

    EfficiencyCalc();
    DiffFakeTrueCircle();
    RingTrackMatchEff();

   cout <<  "fNofAllRings="<< fNofAllRings <<
           "  per Event = " << (Double_t)fNofAllRings/fEventNumber<< endl <<

           "fNofMcElRings="<< fNofMcElRings<<
           "  per Event = " << (Double_t)fNofMcElRings/fEventNumber<< endl <<

           "fNofMcPiRings="<< fNofMcPiRings <<
           "  per Event = " << (Double_t)fNofMcPiRings/fEventNumber<< endl <<

           "fNofAccElRings="<<fNofAccElRings <<
           "  per Event = " << (Double_t)fNofAccElRings/fEventNumber<< endl <<

           "fNofAccPiRings="<<fNofAccPiRings <<
           "  per Event = " << (Double_t)fNofAccPiRings/fEventNumber<< endl <<

           "fNofRecElRings = " << fNofRecElRings <<
           "  per Event = " << (Double_t)fNofRecElRings/fEventNumber<< endl <<

           "fNofRecPiRings = " << fNofRecPiRings <<
           "  per Event = " << (Double_t)fNofRecPiRings/fEventNumber<< endl <<

           "fNofCloneRings = " << fNofCloneRings <<
           "  per Event = " << (Double_t)fNofCloneRings/fEventNumber<<
           ", "<< 100.*(Double_t)fNofCloneRings/fNofAllRings << "%" << endl <<

           "fNofFakeRings = " << fNofFakeRings <<
           "  per Event = " << (Double_t)fNofFakeRings/fEventNumber<<
           ", "<< 100.*(Double_t)fNofFakeRings/fNofAllRings << "%" << endl;

     Double_t elEff = 0.;
     if (fNofAccElRings != 0.)
        elEff = (Double_t)fNofRecElRings/(Double_t)fNofAccElRings;
     cout << "El efficiency = "<< elEff << endl;

     Double_t piEff = 0.;
     if (fNofAccPiRings != 0.)
        piEff = (Double_t)fNofRecPiRings/(Double_t)fNofAccPiRings;
     cout << "Pi efficiency = "<<piEff << endl;

     cout << "NofTrueMatchEl = "<<fh_TrueMatchElDistance->GetEntries() <<
            "  per Event = " << (Double_t)fh_TrueMatchElDistance->GetEntries()/fEventNumber<< endl;

     cout << "NofWrongMatchEl = "<<fh_WrongMatchElDistance->GetEntries() <<
            "  per Event = " << (Double_t)fh_WrongMatchElDistance->GetEntries()/fEventNumber<< endl;

     Double_t matchRatio = 0.;
     if (fh_TrueMatchElDistance->GetEntries()!=0.)
         matchRatio = (Double_t)fh_WrongMatchElDistance->GetEntries() /
                      (Double_t)fh_TrueMatchElDistance->GetEntries();
     cout <<"Ratio (NofWrongMatchEl / NofTrueMatchEl) = "<< matchRatio<< endl;

     cout << "fNofTrueElBadFit = " << fNofTrueElBadFit << endl;
}

Bool_t CbmRichRingQa::DoesRingHaveProjection(Int_t trackId){
	return true;
	//search for projection with such TrackID
    Bool_t isProj = false;
    Int_t nProj = fProj->GetEntriesFast();

    for (Int_t iProj = 0; iProj < nProj; iProj++){
    	FairTrackParam* proj   = (FairTrackParam*)fProj->At(iProj);
        if (!proj){
            cout << "-E- no projection"<< iProj<<endl;
            continue;

        }
        CbmGlobalTrack* gtrack = (CbmGlobalTrack*)gTrackArray->At(iProj);
        if (gtrack->GetStsTrackIndex() == -1) continue;
        CbmTrackMatch* trackMatch = (CbmTrackMatch*)fTrackMatch->At(gtrack->GetStsTrackIndex());

        if (!trackMatch) cout << "-E- no matched track!: fake?"<< endl;
        if (!trackMatch) continue;
        if (trackMatch->GetMCTrackId() == trackId && proj->GetX()!= 0 &&  proj->GetY()!= 0){
            isProj = true;
            break;
        }
    }
    return isProj;
}

Double_t CbmRichRingQa::GetStsMomentum(CbmRichRing * ring)
{
    Int_t ringTrackId = ring->GetTrackID();

    if (ringTrackId == -1) return 0.0;

    CbmGlobalTrack* gtrack = (CbmGlobalTrack*)gTrackArray->At(ringTrackId);
    if (gtrack->GetStsTrackIndex() == -1) return 0.0;
    CbmTrackMatch* stsMatch  =
            (CbmTrackMatch*) fTrackMatch->At(gtrack->GetStsTrackIndex());
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
    CbmRichRingMatch *match;
    CbmMCTrack* track;
    CbmRichRing* ring;

    Int_t nMatches = fMatches->GetEntriesFast();
    cout<<"Number of found rings = "<<nMatches<<endl;

    vector<Int_t> clone;
    clone.clear();

    for (Int_t iMatches = 0; iMatches < nMatches; iMatches++){
        match   = (CbmRichRingMatch*)fMatches->At(iMatches);
        if (!match) continue;

        ring = (CbmRichRing*)fRings->At(iMatches);
        if (!ring) continue;


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
        track = (CbmMCTrack*) fTracks->At(trackID);
        if (!track) {
            cout << "-E- CbmRichRingQa::EfficiencyCalc(). No track "
            << trackID <<endl;
            continue;
        }
        Int_t gcode = track->GetPdgCode();
        Double_t momentum = track->GetP();
        Int_t motherId = track->GetMotherId();
        Bool_t isProj = DoesRingHaveProjection(trackID);

        ring->SetRecFlag(-1);

        ///fake rings
        if (lPercTrue < 0.5){
            ring->SetRecFlag(1);
        }else{///true rings

            if (TMath::Abs(gcode) == 11 &&
                 motherId == -1 &&
                 lMCHits >= fNofHitsInRingCut &&
                 isProj ){ ///primary electron rings

                ring->SetRecFlag(3);
            }
            if ( TMath::Abs(gcode) == 211 &&
                lMCHits >= fNofHitsInRingCut &&
                isProj ){ ///pion rings

                ring->SetRecFlag(4);
            }
            /// search for clone rings
            for (UInt_t ci = 0; ci < clone.size(); ci++)
                if (trackID == clone[ci]) ring->SetRecFlag(2);
            clone.push_back(trackID);

        }///true rings

        Int_t recFlag = ring->GetRecFlag();

        ///clone ring
        if (recFlag ==2){
            fNofCloneRings++;
        }

        ///fake ring
        if (recFlag == 1){
            fNofFakeRings++;
        }

        ///primary electrons
        if (recFlag == 3){
            fNofRecElRings++;
            if (ring->GetAaxis() > 10 || ring->GetAaxis() < 0) fNofTrueElBadFit++;
            fh_RecElRingsMom->Fill(momentum);
            fh_RecElRingsNofHits->Fill(lMCHits);
            fh_RecElRingsRadPos->Fill(fRingMapWithHits[trackID].GetRadialPosition());
            Double_t bOverA = fRingMapWithHits[trackID].GetBaxis()/fRingMapWithHits[trackID].GetAaxis();
            fh_RecElRingsBoverA->Fill(bOverA);
        }

        ///pions
        if (recFlag == 4){
            fNofRecPiRings++;
        }
    }///loop over all found rings

}

void CbmRichRingQa::DiffFakeTrue()
{

}

void CbmRichRingQa::DiffFakeTrueCircle()
{
    CbmRichRingMatch *match;
	CbmRichRing* ring;
	Int_t nMatches = fMatches->GetEntriesFast();

	for (Int_t iMatches = 0; iMatches < nMatches; iMatches++) {
		match = (CbmRichRingMatch*) fMatches->At(iMatches);
		if (!match) {
			cout << "-E- CbmRichRingQa::DiffFakeTrueCOP() no match" << iMatches	<< endl;
			continue;
		}
		ring = (CbmRichRing*) fRings->At(iMatches);
		if (!ring) {
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

		if (TMath::IsNaN(nHits)
				|| TMath::IsNaN(angle) || TMath::IsNaN(hitsOnRing)
				|| TMath::IsNaN(radPos) || TMath::IsNaN(radius)
				|| TMath::IsNaN(chi2))	continue;
		if (nHits < 3) continue;
		if (chi2 > 10000000000.) continue;
		if (recFlag == 1)
			foutFakeAndTrue << nHits << " " << angle << " "
					<< hitsOnRing << " " << radPos << " " << radius << " "
					<< chi2 << " " << -1 << endl;

		if (recFlag == 3)
			foutFakeAndTrue << nHits << " " << angle << " "
			<< hitsOnRing << " " << radPos << " " << radius << " "
			<< chi2 << " " << 1 << endl;

	}///loop over matches
}

void CbmRichRingQa::RingTrackMatchEff()
{
    CbmRichRingMatch *match;
    CbmRichRing* ring;
    CbmMCTrack* track;
    Int_t nMatches = fMatches->GetEntriesFast();
    Int_t nTraksMatch = fTrackMatch->GetEntriesFast();
    cout << "nTraksMatch = " << nTraksMatch<< endl;

    for (Int_t iMatches = 0; iMatches < nMatches; iMatches++){

        match   = (CbmRichRingMatch*)fMatches->At(iMatches);
        if (!match){
            cout << "-E- CbmRichRingQa::RingTrackMatchEff() no match"<<
            iMatches<<endl;
            continue;
        }

        ring = (CbmRichRing*)fRings->At(iMatches);
        if (!ring){
            cout << "-E- CbmRichRingQa::RingTrackMatchEff() no ring"<<
            iMatches<<endl;
            continue;
        }
        Int_t recFlag = ring->GetRecFlag();
        Double_t ringTrackDist = ring->GetDistance();

        Int_t trackID = match->GetMCTrackID();

        ///get momentum
        if (trackID > fTracks->GetEntriesFast() || trackID < 0) continue;
        track = (CbmMCTrack*) fTracks->At(trackID);
        if (!track) {
            cout << "-E- CbmRichRingQa::RingTrackMatchEff(). No track "
            << trackID <<endl;
            continue;
        }
        Double_t momentum = track->GetP();


        Int_t ringTrackId = ring->GetTrackID();
        if (ringTrackId == -1) continue;

        if (ringTrackId > gTrackArray->GetEntriesFast() || ringTrackId < 0) continue;
        CbmGlobalTrack* gtrack = (CbmGlobalTrack*)gTrackArray->At(ringTrackId);
        if (gtrack->GetStsTrackIndex() == -1) continue;
        CbmTrackMatch* stsMatch  =
                (CbmTrackMatch*) fTrackMatch->At(gtrack->GetStsTrackIndex());

        if (!stsMatch) cout << "-E- no matched track!: fake?"<< endl;
        if (!stsMatch) continue;
        Int_t ringMCTrackId = stsMatch->GetMCTrackId();

        if (recFlag == 3){///true primary electron
            //fh_AllMatchDistTrackE->Fill(ringTrackDist);
            if (trackID == ringMCTrackId){///true match
                fh_TrueMatchElDistance->Fill(ringTrackDist);
                fh_TrueMatchElMom->Fill(momentum);
            }
            if (trackID != ringMCTrackId){///wrong match
                fh_WrongMatchElDistance->Fill(ringTrackDist);
            }
        }
    }///loop over matches
}

// -----   Finish Task   ---------------------------------------------------
void CbmRichRingQa::FinishTask()
{

   // fRings->Clear();
    //fPoints->Clear();
   // fTracks->Clear();
   // fHits->Clear();
   // fMatches->Clear();
   // gTrackArray->Clear();

    TDirectory *current = gDirectory;
    TDirectory *rich = current->mkdir("RichRingQaHist");
    rich->cd();

    fh_RecElRingsMom->Write();
    fh_AccElRingsMom->Write();

    fh_RecElRingsRadPos->Write();
    fh_AccElRingsRadPos->Write();

    fh_RecElRingsNofHits->Write();
    fh_AccElRingsNofHits->Write();

    fh_RecElRingsBoverA->Write();
    fh_AccElRingsBoverA->Write();

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

    fh_WrongMatchElDistance->Write();
    fh_TrueMatchElDistance->Write();
    fh_TrueMatchElMom->Write();

    fh_HitsXY->Scale(1./fEventNumber); // hits/cm^2/event
    fh_HitsXY->Write();

    fh_NhitsPerEvent->Write();
    fh_NprojPerEvent->Write();

    rich->cd();
    current->cd();

    ofstream fout;
    fout.open("opt_param_ht.txt",std::ios_base::app);
    fout << (Double_t)fNofCloneRings/fEventNumber<< " " <<
		(Double_t)fNofFakeRings/fEventNumber << " " <<
		(Double_t)fNofRecElRings/(Double_t)fNofAccElRings << endl;

    delete fFitCOP;
    delete fFitEllipse;
    delete fSelectImpl;
}

ClassImp(CbmRichRingQa)
