#include "CbmRichElectronsQa.h"

#include "FairTrackParam.h"
#include "CbmGlobalTrack.h"
#include "CbmRichRing.h"
#include "CbmRichRingMatch.h"
#include "CbmTrdTrack.h"
#include "CbmTrackMatch.h"
#include "FairRootManager.h"
#include "FairMCPoint.h"
#include "CbmMCTrack.h"
#include "CbmTofHit.h"
#include "CbmTofPoint.h"

//#include "../../littrack/utils/CbmLitUtils.cxx"
//#include "../utils/CbmRichDrawElectronsQa.cxx"

#include "TString.h"
#include "TSystem.h"
#include "TCanvas.h"

#include <iostream>
#include <vector>
#include <map>
#include <cmath>

class CbmStsTrack;

using std::cout;
using std::endl;
using std::map;
using std::vector;
using std::fabs;

//------------  standard constructor (with verbosity level)  ---------------------------------
CbmRichElectronsQa::CbmRichElectronsQa(const char *name, const char *title, Int_t verbose)
  :FairTask(name)
{
	fEventNum = 1;

///RICH
	fNofMCRings = 0;
    fNofAccRings = 0;
	fNofTrueFoundRings = 0;
    fNofTrueMatchRings = 0;
	fNofTrueElIdRings = 0;

	fhMCRings = new TH1D("fhMCRings", "MC RICH e+ and e- rings;momentum, GeV/c;Entries", 30,0,15);
	fhAccRings = new TH1D("fhAccRings", "Accepted RICH e+ and e- rings;momentum, GeV/c;Entries", 30,0,15);;
	fhTrueFoundRings = new TH1D("fhTrueFoundRings", "True Found RICH e+ and e- rings;momentum, GeV/c;Entries", 30,0,15);
	fhTrueMatchRings = new TH1D("fhTrueMatchRings", "True Ring-track RICH e+ and e- rings;momentum, GeV/c;Entries", 30,0,15);
	fhTrueElIdRings = new TH1D("fhTrueElIdRings", "True Identified RICH e+ and e- rings;momentum, GeV/c;Entries", 30,0,15);

///GLOBAL

	fNofAccRichTrdGlobal = 0;
	fNofAccRichTrdTofGlobal = 0;
	fNofTrueMatchStsRichGlobal = 0;
	fNofTrueMatchStsRichTrdGlobal = 0;
	fNofTrueMatchStsRichTrdTofGlobal = 0;

	fhAccRichTrdGlobal = new TH1D("fhAccRichTrdGlobal", "Accepted RICH-TRD;momentum, GeV/c;Entries", 30,0,15);;
	fhAccRichTrdTofGlobal = new TH1D("fhAccRichTrdTofGlobal", "Accepted RICH-TRD-TOF;momentum, GeV/c;Entries", 30,0,15);;
	fhTrueMatchStsRichGlobal = new TH1D("fhTrueMatchStsRichGlobal", "True STS RICH match, Global;momentum, GeV/c;Entries", 30,0,15);;
	fhTrueMatchStsRichTrdGlobal = new TH1D("fhTrueMatchStsRichTrdGlobal", "True STS RICH TRD match, Global;momentum, GeV/c;Entries", 30,0,15);;
	fhTrueMatchStsRichTrdTofGlobal = new TH1D("fhTrueMatchStsRichTrdTofGlobal", "True STS RICH TRD TOF match, Global;momentum, GeV/c;Entries", 30,0,15);;

/// ELECTRON IDENTIFICATION
	fNofTrueIdRich = 0;
	fNofAccPi = 0;
	fNofPiasElRich = 0;
	fNofTrueIdRichTrd = 0;;
	fNofPiasElRichTrd = 0;
	fNofTrueIdRichTrdTof = 0;
	fNofPiasElRichTrdTof = 0;

	fhTrueIdRich = new TH1D("fhTrueIdRich", "True identified e+ and e- by RICH;momentum, GeV/c;Entries", 30,0,15);
	fhAccPi = new TH1D("fhAccPi", "Accepted pions;momentum, GeV/c;Entries", 30,0,15);
	fhPiasElRich = new TH1D("fhPiasElRich", "Pi identified as el by RICH;momentum, GeV/c;Entries", 30,0,15);
    fhTrueIdRichTrd = new TH1D("fhTrueIdRichTrd", "True identified as el by RICH and TRD;momentum, GeV/c;Entries", 30,0,15);
	fhPiasElRichTrd = new TH1D("fhPiasElRichTrd", "Pi identified as el by RICH and TRD;momentum, GeV/c;Entries", 30,0,15);
	fhTrueIdRichTrdTof = new TH1D("fhTrueIdRichTrdTof", "True identified as el by RICH-TRD-TOF;momentum, GeV/c;Entries", 30,0,15);
	fhPiasElRichTrdTof = new TH1D("fhPiasElRichTrdTof", "Pi identified as el by RICH-TRD-TOF;momentum, GeV/c;Entries", 30,0,15);

/// DIFFERENCE BETWEEN ELECTRONS AND PIONS
	fhAaxisEl= new TH1D("fhAaxisEl", "fhAaxisEl;A axis, cm;Yield", 30,3,8);
	fhAaxisPi= new TH1D("fhAaxisPi", "fhAaxisPi;A axis, cm;Yield", 30,3,8);
	fhBAxisEl= new TH1D("fhBAxisEl", "fhBAxisEl;B axis, cm;Yield", 30,3,8);
	fhBAxisPi= new TH1D("fhBAxisPi", "fhBAxisPi;B axis, cm;Yield", 30,3,8);
	fhAaxisCorEl= new TH1D("fhAaxisCorEl", "fhAaxisCorEl;A axis, cm;Yield", 30,3,8);
	fhAaxisCorPi= new TH1D("fhAaxisCorPi", "fhAaxisCorPi;A axis, cm;Yield", 30,3,8);
	fhBAxisCorEl= new TH1D("fhBAxisCorEl", "fhBAxisCorEl;B axis, cm;Yield", 30,3,8);
	fhBAxisCorPi= new TH1D("fhBAxisCorPi", "fhBAxisCorPi;B axis, cm;Yield", 30,3,8);
	fhDistEl= new TH1D("fhDistEl", "fhDistEl;ring-track distance, cm;Yield", 30,0,3);
	fhDistPi= new TH1D("fhDistPi", "fhDistPi;ring-track distance, cm;Yield", 30,0,3);
	fhNofHitsEl= new TH1D("fhNofHitsEl", "fhNofHitsEl;nof hits;Yield", 30,0,45);
	fhNofHitsPi= new TH1D("fhNofHitsPi", "fhNofHitsPi;nof hits;Yield", 30,0,45);
	fhChi2El= new TH1D("fhChi2El", "fhChi2El;chi2;Yield", 30,0,1.);
	fhChi2Pi= new TH1D("fhChi2Pi", "fhChi2Pi;chi2;Yield", 30,0,1.);
	fhRadPosEl= new TH1D("fhRadPosEl", "fhRadPosEl;radial position, cm;Yield", 30,0,150);
	fhRadPosPi= new TH1D("fhRadPosPi", "fhRadPosPi;radial position, cm;Yield", 30,0,150);
	fhAaxisVsMomEl= new TH2D("fhAaxisVsMomEl", "fhAaxisVsMomEl;momentum, GeV/c;A axis, cm",30,0,15, 30,3,8);
	fhAaxisVsMomPi= new TH2D("fhAaxisVsMomPi", "fhAaxisVsMomPi;momentum, GeV/c;A axis, cm",30,0,15, 30,3,8);
	fhBAxisVsMomEl= new TH2D("fhBAxisVsMomEl", "fhBAxisVsMomEl;momentum, GeV/c;B axis, cm",30,0,15, 30,3,8);
	fhBAxisVsMomPi= new TH2D("fhBAxisVsMomPi", "fhBAxisVsMomPi;momentum, GeV/c;B axis, cm",30,0,15, 30,3,8);
	fhPhiVsRadAngEl= new TH2D("fhPhiVsRadAngEl", "fhPhiVsRadAngEl;phi, rad;rad angle, rad", 50, -2. ,2.,50, 0. , 6.3);
	fhPhiVsRadAngPi= new TH2D("fhPhiVsRadAngPi", "fhPhiVsRadAngPi;phi, rad;rad angle, rad", 50, -2. ,2.,50, 0. , 6.3);
	fhTrdAnnEl= new TH1D("fhTrdAnnEl", "fhTrdAnnEl;ANN output;Yield", 50,-1.2,1.2);
	fhTrdAnnPi= new TH1D("fhTrdAnnPi", "fhTrdAnnPi;ANN output;Yield", 50,-1.2,1.2);

	fOutElandPi.open("ann_el_pi.txt");
	SetDefaultParameters();
}


CbmRichElectronsQa::~CbmRichElectronsQa()
{

}

void CbmRichElectronsQa::SetDefaultParameters()
{
	fMinNofHitsInRichRing = 5;
	fMinNofTrdHits = 8;
	fTrdAnnCut = -1.;
	fRichAnnCut = -1.;
	fUseRichAnn = false;
	fMeanA = -1.;
	fMeanB = -1.;
	fRmsA = -1.;
	fRmsB = -1.;
	fRmsCoeff = -1.;
	fDistCut = -1.;
}

Bool_t CbmRichElectronsQa::SetParameters()
{
	if (fTrdAnnCut == -1.){
		cout << "-E- You must set TrdAnnCut, current value is " << fTrdAnnCut<<endl;
		return false;
	}
	if (fUseRichAnn && fRichAnnCut == -1.){
		cout << "-E- You must set RichAnnCut, because you are using RICH ANN, current value is " << fRichAnnCut<<endl;
		return false;
	}
	if (fMeanA == -1.){
		cout << "-E- You must set MeanA, current value is " << fMeanA<<endl;
		return false;
	}
	if (fMeanB == -1.){
		cout << "-E- You must set MeanB, current value is " << fMeanB<<endl;
		return false;
	}
	if (fRmsA == -1.){
		cout << "-E- You must set RmsA, current value is " << fRmsA<<endl;
		return false;
	}
	if (fRmsB == -1.){
		cout << "-E- You must set RmsB, current value is " << fRmsB<<endl;
		return false;
	}
	if (fRmsCoeff == -1.){
		cout << "-E- You must set RmsCoeff, current value is " << fRmsCoeff<<endl;
		return false;
	}
	if (fDistCut == -1.){
		cout << "-E- You must set fDistCut, current value is " << fDistCut<<endl;
		return false;
	}

	if (fUseRichAnn){
		if (fRichGeoType != "compact" && fRichGeoType != "large"){
			fRichGeoType = "compact";
			cout << "-E- CbmRichElectronsQa::SetParameters UNKNOWN geometry,  " <<
			"Set default parameters for "<< fRichGeoType << " RICH geometry"<<endl;
			return false;
		}
		TString richANNFile = gSystem->Getenv("VMCWORKDIR");
		if (fRichGeoType == "compact"){
			richANNFile += "/parameters/rich/el_id_ann_weights_rich_compact.txt";
		}else if (fRichGeoType == "large"){
			richANNFile += "/parameters/rich/el_id_ann_weights_rich.txt";
		}

		fElIdAnn = new CbmRichElectronIdAnn(0, richANNFile);
		fElIdAnn->Init();
	}
	return true;
}


InitStatus CbmRichElectronsQa::Init()
{
	cout << "InitStatus CbmRichElectronsQa::Init()"<<endl;
	// Get and check CbmRootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if (!ioman) {
		cout << "-E- CbmRichElectronsQa::Init: " << "RootManager not instantised!"
				<< endl;
		return kERROR;
	}

	// Get hit Array
	fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
	if ( !fRichHits) {
		cout << "-W- CbmRichElectronsQa::Init: No RichHit array!" << endl;
	}

	// Get RichRing Array
	fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
	if ( !fRichRings) {
		cout << "-E- CbmRichElectronsQa::Init: No RichRing array!" << endl;
		return kERROR;
	}

	// Get MC Point array
	fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
	if ( !fRichPoints) {
		cout << "-E- CbmRichElectronsQa::Init: No RichPoint array!" << endl;
		return kERROR;
	}

	// Get MC Point array
	fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
	if ( !fMCTracks) {
		cout << "-E- CbmRichElectronsQa::Init: No MCTrack array!" << endl;
		return kERROR;
	}

	// Get RichRingMatch array
	fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
	if ( !fRichRingMatches) {
		cout << "-E- CbmRichElectronsQa::Init: No RichRingMatch array!" << endl;
		return kERROR;
	}

	// Get RichProjection array
	fRichProj = (TClonesArray*) ioman->GetObject("RichProjection");
	if ( !fRichProj) {
		cout << "-E- CbmRichElectronsQa::Init: No RichProjection array!" << endl;
		return kERROR;
	}

	// get TrackMatch array
	fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
	if ( !fStsTrackMatches) {
		cout << "-E- CbmRichElectronsQa::Init: No track match array!"<< endl;
		return kERROR;
	}

	// Get global track array
	fGlobalTracks = (TClonesArray*) ioman->GetObject("GlobalTrack");
	if ( !fGlobalTracks) {
		cout << "-W- CbmRichElectronsQa::Init: No global track array!" << endl;
		return kERROR;
	}


	fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
	if ( !fStsTracks) {
		cout << "-E- CbmRichElectronsQa::Init: No  STSTrack array!"<< endl;
		return kERROR;
	}


	fTrdTracks = (TClonesArray*) ioman->GetObject("TrdTrack");
	if ( !fTrdTracks) {
		cout << "-W- CbmRichElectronsQa::Init: No TrdTrack array!" << endl;
		return kERROR;
	}
	fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
	if ( !fTrdTrackMatches) {
		cout << "-W- CbmRichElectronsQa::Init: No TrdTrackMatch array!" << endl;
		return kERROR;
	}

	fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
	if ( !fTofPoints) {
		cout << "-W- CbmRichElectronsQa::Init: No TOFPoint array!" << endl;
		return kERROR;
	}

	fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
	if ( !fTofHits) {
		cout << "-W- CbmRichElectronsQa::Init: No TOFHit array!" << endl;
		return kERROR;
	}

	Bool_t isSetOk = SetParameters();

	if (!isSetOk) return kFATAL;

	return kSUCCESS;
}

void CbmRichElectronsQa::Exec(Option_t* option)
{
    AccTracks();
    FoundRichRings();
    RingTrackMatch();
    GlobalTracksMatchEff();
    GlobalTracksElIdEff();
    DiffElandPi();

    Double_t eventNum = (Double_t) fEventNum;

    cout << endl <<"----CbmRichElectronsQa---- event # " << fEventNum << endl;
    cout <<"You set the following cuts:"<< endl;
	cout << "RichAnnCut="<<fRichAnnCut << ";  ";
	cout << "UseRichAnn=" <<fUseRichAnn <<";  ";
	cout << "TrdAnnCut=" <<fTrdAnnCut <<";  ";
	cout << "MeanA=" <<fMeanA <<";  ";
	cout << "MeanB=" <<fMeanB <<";  ";
	cout << "RmsA=" <<fRmsA <<";  ";
	cout << "RmsB=" <<fRmsB <<";  ";
	cout << "RmsCoeff=" <<fRmsCoeff <<";  ";
	cout << "DistCut=" <<fDistCut <<endl;

    cout << "----RICH----" << endl;
	cout << "fNofMCRings = " << fNofMCRings <<
		", per event " << fNofMCRings/eventNum << endl;
	cout << "fNofAccRings = " << fNofAccRings<<
		", per event " << fNofAccRings/eventNum <<
		", Eff = " << (Double_t)fNofAccRings / fNofMCRings << endl;
	cout << "fNofTrueFoundRings = "<< fNofTrueFoundRings <<
		", per event " << fNofTrueFoundRings/eventNum <<
		", Eff = " << (Double_t)fNofTrueFoundRings/ fNofAccRings << endl;
	cout << "fTrueMatchRings = " << fNofTrueMatchRings <<
		", per event " << fNofTrueMatchRings/eventNum <<
		", Eff = " << (Double_t) fNofTrueMatchRings / fNofTrueFoundRings <<
		", Eff (acc) = " << (Double_t) fNofTrueMatchRings / fNofAccRings <<endl;
	cout << "fTrueElIdRings = " << fNofTrueElIdRings <<
		", per event " << fNofTrueElIdRings/eventNum <<
		", Eff = " << (Double_t) fNofTrueElIdRings / fNofTrueMatchRings <<
		", Eff (acc) = " << (Double_t) fNofTrueElIdRings / fNofAccRings << endl;

	cout << "----GLOBAL----"<< endl;
	cout << "fNofAccRichTrdGlobal = " <<  fNofAccRichTrdGlobal<<
		", per event " << fNofAccRichTrdGlobal/eventNum <<
		", Eff = " << (Double_t) fNofAccRichTrdGlobal /  fNofAccRings <<endl;
	cout << "fNofAccRichTrdTofGlobal = " <<  fNofAccRichTrdTofGlobal<<
		", per event " << fNofAccRichTrdTofGlobal/eventNum <<
	    ", Eff = " << (Double_t) fNofAccRichTrdTofGlobal / fNofAccRichTrdGlobal  <<
		", Eff (acc.ring) = " << (Double_t) fNofAccRichTrdTofGlobal /  fNofAccRings <<endl;

	cout << "fNofTrueMatchStsRichGlobal = " << fNofTrueMatchStsRichGlobal <<
		", per event " << fNofTrueMatchStsRichGlobal/eventNum <<
		", Eff = " << (Double_t) fNofTrueMatchStsRichGlobal / fNofTrueFoundRings <<
		", Eff (acc. rings) = " << (Double_t) fNofTrueMatchStsRichGlobal / fNofAccRings <<endl;
	cout << "fNofTrueMatchStsRichTrdGlobal = " << fNofTrueMatchStsRichTrdGlobal <<
		", per event " << fNofTrueMatchStsRichTrdGlobal/eventNum <<
		", Eff = " << (Double_t) fNofTrueMatchStsRichTrdGlobal /  fNofTrueMatchStsRichGlobal<<
		", Eff (acc. rings) = " << (Double_t) fNofTrueMatchStsRichTrdGlobal / fNofAccRings <<endl;
	cout << "fNofTrueMatchStsRichTrdTofGlobal = " << fNofTrueMatchStsRichTrdTofGlobal <<
		", per event " << fNofTrueMatchStsRichTrdTofGlobal/eventNum <<
		" Eff = " << (Double_t) fNofTrueMatchStsRichTrdTofGlobal /  fNofTrueMatchStsRichTrdGlobal<<
		" Eff (acc. rings) = " << (Double_t) fNofTrueMatchStsRichTrdTofGlobal / fNofAccRings <<endl;

	cout << "----ELECTRON IDENTIFICATION----"<< endl;
	cout << "fNofTrueIdRich = " << fNofTrueIdRich <<
		", per event " << fNofTrueIdRich/eventNum <<
		", Eff (acc. rings) = " << (Double_t) fNofTrueIdRich / fNofAccRings <<endl;

	cout << "fNofAccPi = " << fNofAccPi <<
		", per event " << fNofAccPi/eventNum <<endl;

	cout << "fNofPiasElRich = " << fNofPiasElRich <<
		", per event " << fNofPiasElRich/eventNum <<
		", pion supression = " << (Double_t) fNofAccPi / fNofPiasElRich <<endl;

	cout << "fNofTrueIdRichTrd = " << fNofTrueIdRichTrd <<
		", per event " << fNofTrueIdRichTrd/eventNum <<
		", Eff (acc. rings) = " << (Double_t)  fNofTrueIdRichTrd/fNofAccRings  <<endl;

	cout << "fNofPiasElRichTrd = " << fNofPiasElRichTrd <<
		", per event " << fNofPiasElRichTrd/eventNum <<
		", pion supression = " << (Double_t) fNofAccPi / fNofPiasElRichTrd <<endl;

	cout << "fNofTrueIdRichTrdTof = " << fNofTrueIdRichTrdTof <<
		", per event " << fNofTrueIdRichTrdTof/eventNum <<
		", Eff (acc. rings) = " << (Double_t)  fNofTrueIdRichTrdTof/fNofAccRings  <<endl;

	cout << "fNofPiasElRichTrdTof = " << fNofPiasElRichTrdTof <<
		", per event " << fNofPiasElRichTrdTof/eventNum <<
		", pion supression = " << (Double_t) fNofAccPi / fNofPiasElRichTrdTof <<endl;

	fEventNum++;
}

void CbmRichElectronsQa::AccTracks()
{
    map<Int_t, Int_t> fRingMap;
    fRingMap.clear();

	Int_t nRichHits = fRichHits->GetEntriesFast();

	/// Loop over Rich hits
	for (Int_t iHit=0; iHit < nRichHits; iHit++) {
		CbmRichHit* hit = (CbmRichHit*) fRichHits->At(iHit);
		if ( !hit)	continue;

		Int_t iPoint = hit->GetRefId();

		if (iPoint < 0)	continue; // Fake or background hit
		//Get the MC Point corresponding to the hit

		FairMCPoint* point = (FairMCPoint*) fRichPoints->At(iPoint);
		if ( !point) continue;

		//Get the MC Track ID corresponding to the MC Point
		Int_t iMCTrack = point->GetTrackID();
		// Get the MC Track corresponding to the ID
		CbmMCTrack* track = (CbmMCTrack*)fMCTracks->At(iMCTrack);
		if ( !track) continue;

		Int_t iMother = track->GetMotherId();
		if (iMother == -1) continue;

		fRingMap[iMother]++;
	} /// Loop over Rich hits
    map<Int_t, Int_t>::iterator it;
	///Loop for all MC rings
	for (it=fRingMap.begin(); it!=fRingMap.end(); it++) {
		CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(it->first);
		if (!track) {
			cout << "-E- CbmRichElectronsQa::AccRichRings() no track"<< it->first<<endl;
			continue;
		}

		Double_t momentum = track->GetP();
		Int_t trackID = it->first;

		Bool_t isProj = DoesRingHaveProjection(trackID);

		Int_t gcode = track->GetPdgCode();
		Int_t motherId = track->GetMotherId();

		if ((TMath::Abs(gcode) == 11) && motherId == -1){///primary electron rings
			fNofMCRings++;
			fhMCRings->Fill(momentum);
		    if (isProj && it->second >= fMinNofHitsInRichRing) { // accepted electron rings
				fNofAccRings++;
				fhAccRings->Fill(momentum);

				if (track->GetNPoints(kTRD) >= fMinNofTrdHits){// accepted RICH + TRD
					fNofAccRichTrdGlobal++;
					fhAccRichTrdGlobal->Fill(momentum);
					if (track->GetNPoints(kTOF) >= 1){ // accepted RICH + TRD + TOF
						fNofAccRichTrdTofGlobal++;
						fhAccRichTrdTofGlobal->Fill(momentum);
					}
				}

			}
		}//primary electron rings


	} ///Loop for all MC rings


	/// pions acceptance
	Int_t nMCTracks = fMCTracks->GetEntriesFast();

	for (Int_t iMCTrk=0; iMCTrk < nMCTracks; iMCTrk++) {
		CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMCTrk);
		if ( !mcTrack)	continue;

		Double_t momentum = mcTrack->GetP();
		Bool_t isProj = DoesRingHaveProjection(iMCTrk);
		Int_t gcode = mcTrack->GetPdgCode();

		if (TMath::Abs(gcode) == 211){
		    if (isProj) {
				fNofAccPi++;
				fhAccPi->Fill(momentum);
			}
		}
	}// iMCTrk
}

void CbmRichElectronsQa::FoundRichRings()
{
    Int_t nMatches = fRichRingMatches->GetEntriesFast();
    vector<Int_t> clone;
    clone.clear();
    for (Int_t iMatches = 0; iMatches < nMatches; iMatches++){
    	CbmRichRingMatch* match   = (CbmRichRingMatch*)fRichRingMatches->At(iMatches);
        if (!match) continue;

        CbmRichRing* ring = (CbmRichRing*)fRichRings->At(iMatches);
        if (!ring) continue;

        Int_t lTrueHits = match->GetNofTrueHits();
        Int_t lWrongHits = match->GetNofWrongHits();
        Int_t lFakeHits = match->GetNofFakeHits();
        Int_t lMCHits = match->GetNofMCHits();
        Int_t lFoundHits = lTrueHits + lWrongHits + lFakeHits;
        Double_t percOfTrueHits = 0;
        if (lFoundHits >= 3){
        	percOfTrueHits = (Double_t)lTrueHits / (Double_t)lFoundHits;
        }

        Int_t trackID = match->GetMCTrackID();
        if (trackID > fMCTracks->GetEntriesFast() || trackID < 0) continue;
        CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(trackID);
        if (!track)continue;

        Int_t gcode = track->GetPdgCode();
        Double_t momentum = track->GetP();
        Int_t motherId = track->GetMotherId();
        Bool_t isProj = DoesRingHaveProjection(trackID);

        ring->SetRecFlag(-1);

        ///fake rings
        if (percOfTrueHits < 0.5){
            ring->SetRecFlag(1);
        }else{///true rings
            if (TMath::Abs(gcode) == 11 &&
                 motherId == -1 &&
                 lMCHits >= fMinNofHitsInRichRing &&
                 isProj ){ ///primary electron rings

                ring->SetRecFlag(3);
            }
            /// search for clone rings
            for (UInt_t ci = 0; ci < clone.size(); ci++)
                if (trackID == clone[ci]){
                	ring->SetRecFlag(2);
                	break;
                }
            clone.push_back(trackID);
        }///true rings

        Int_t recFlag = ring->GetRecFlag();

        if (recFlag == 3){
        	fNofTrueFoundRings++;
        	fhTrueFoundRings->Fill(momentum);
        }

    }///loop over all found rings
}

void CbmRichElectronsQa::RingTrackMatch()
{
    Int_t nMatches = fRichRingMatches->GetEntriesFast();
    Int_t nTraksMatch = fStsTrackMatches->GetEntriesFast();

    for (Int_t iMatches = 0; iMatches < nMatches; iMatches++){
    	CbmRichRingMatch* match = (CbmRichRingMatch*)fRichRingMatches->At(iMatches);
        if (!match)continue;

        CbmRichRing* ring = (CbmRichRing*)fRichRings->At(iMatches);
        if (!ring) continue;

        Int_t recFlag = ring->GetRecFlag();
        Int_t trackID = match->GetMCTrackID();
        ///get momentum
        if (trackID > fMCTracks->GetEntriesFast() || trackID < 0) continue;
        CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(trackID);
        if (!track) continue;

        Double_t momentum = track->GetP();

        Int_t ringTrackId = ring->GetTrackID();
        if (ringTrackId == -1) continue;

        if (ringTrackId > fGlobalTracks->GetEntriesFast() || ringTrackId < 0) continue;
        CbmGlobalTrack* gtrack = (CbmGlobalTrack*)fGlobalTracks->At(ringTrackId);
        if (gtrack->GetStsTrackIndex() == -1) continue;
        CbmTrackMatch* stsMatch  = (CbmTrackMatch*) fStsTrackMatches->At(gtrack->GetStsTrackIndex());

        if (!stsMatch) continue;
        Int_t ringMCTrackId = stsMatch->GetMCTrackId();

        Bool_t isRichEl = IsRichElectron(ring, momentum);

        if (recFlag == 3){
            if (trackID == ringMCTrackId){///true match
            	fNofTrueMatchRings++;
                fhTrueMatchRings->Fill(momentum);
                if (isRichEl ){
                	fNofTrueElIdRings++;
                	fhTrueElIdRings->Fill(momentum);
                }
            }
        }
    }///loop over matches
}


Bool_t CbmRichElectronsQa::DoesRingHaveProjection(Int_t trackId){
    //search for projection with certain TrackID
	Bool_t isProj = false;
	Int_t nProj = fRichProj->GetEntriesFast();

	for (Int_t iProj = 0; iProj < nProj; iProj++) {
		FairTrackParam* proj = (FairTrackParam*)fRichProj->At(iProj);
		if (!proj) {
			cout << "-E- no projection"<< iProj<<endl;
			continue;
		}

		CbmGlobalTrack* gtrack = (CbmGlobalTrack*)fGlobalTracks->At(iProj);
		if (gtrack->GetStsTrackIndex() == -1) continue;
		CbmTrackMatch* trackMatch = (CbmTrackMatch*)fStsTrackMatches->At(gtrack->GetStsTrackIndex());

		if (!trackMatch) cout << "-E- no matched track!: fake?"<< endl;
		if (!trackMatch) continue;
		if (trackMatch->GetMCTrackId() == trackId && proj->GetX()!= 0 &&  proj->GetY()!= 0){
			isProj = true;
			break;
		}
	}
	return isProj;
}

void CbmRichElectronsQa::GlobalTracksMatchEff()
{
	Int_t nGlTracks = fGlobalTracks->GetEntriesFast();

	for (Int_t iTrack=0; iTrack<nGlTracks; iTrack++) {
		CbmGlobalTrack* gTrack = (CbmGlobalTrack*)fGlobalTracks->At(iTrack);
///STS and RICH
		Int_t stsIndex = gTrack->GetStsTrackIndex();
		if (stsIndex == -1)continue;
		Int_t richIndex = gTrack->GetRichRingIndex();
		if (richIndex == -1) continue;

		CbmStsTrack* stsTrack = (CbmStsTrack*)fStsTracks->At(stsIndex);
		if (!stsTrack) continue;
		CbmRichRing* richRing = (CbmRichRing*)fRichRings->At(richIndex);
		if (!richRing) continue;

		CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)fStsTrackMatches->At(stsIndex);
		if (!stsTrackMatch) continue;
		CbmRichRingMatch* richRingMatch = (CbmRichRingMatch*)fRichRingMatches->At(richIndex);
		if (!richRingMatch) continue;

		Int_t mcIdSts = stsTrackMatch->GetMCTrackId();
		Int_t mcIdRich = richRingMatch->GetMCTrackID();

		CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(mcIdSts);
        if (!track) continue;
        Int_t pdg = TMath::Abs(track->GetPdgCode());
        Int_t motherId = track->GetMotherId();
        Double_t momentum = track->GetP();
        if (pdg != 11 || motherId != -1) continue;//only primary electrons

		if (richRing->GetRecFlag() == 3 && mcIdSts == mcIdRich && mcIdRich != -1){
			fNofTrueMatchStsRichGlobal++;
        	fhTrueMatchStsRichGlobal->Fill(momentum);
		}
///TRD
		Int_t trdIndex = gTrack->GetTrdTrackIndex();

		if (trdIndex == -1) {
			continue;
		}
		CbmTrdTrack* trdTrack = (CbmTrdTrack*)fTrdTracks->At(trdIndex);
		if (!trdTrack)continue;
		CbmTrackMatch* trdTrackMatch = (CbmTrackMatch*)fTrdTrackMatches->At(trdIndex);
		if (!trdTrackMatch)continue;
		Int_t mcIdTrd = trdTrackMatch->GetMCTrackId();

		if (richRing->GetRecFlag() == 3 &&
			mcIdSts == mcIdRich &&
			mcIdSts == mcIdTrd &&
			mcIdRich != -1){
	        	fNofTrueMatchStsRichTrdGlobal++;
	        	fhTrueMatchStsRichTrdGlobal->Fill(momentum);
		}
///TOF
		Int_t tofIndex = gTrack->GetTofHitIndex();
		if (tofIndex == -1) continue;
		CbmTofHit* tofHit = (CbmTofHit*)fTofHits->At(tofIndex);
		if (!tofHit)continue;
		Int_t tofPointIndex = tofHit->GetRefId();
		if (tofPointIndex < 0) continue;
		FairMCPoint* tofPoint = (FairMCPoint*)fTofPoints->At(tofPointIndex);
		if (!tofPoint)continue;
		Int_t mcIdTof = tofPoint->GetTrackID();
		if (richRing->GetRecFlag() == 3 &&
			mcIdSts == mcIdRich &&
			mcIdSts == mcIdTrd &&
			mcIdSts == mcIdTof &&
			mcIdRich != -1){
	        	fNofTrueMatchStsRichTrdTofGlobal++;
	        	fhTrueMatchStsRichTrdTofGlobal->Fill(momentum);
		}
	}// global tracks
}

void CbmRichElectronsQa::GlobalTracksElIdEff()
{
	Int_t nGlTracks = fGlobalTracks->GetEntriesFast();

	for (Int_t iTrack=0; iTrack<nGlTracks; iTrack++) {

		CbmGlobalTrack* gTrack = (CbmGlobalTrack*)fGlobalTracks->At(iTrack);
		///STS and RICH
		Int_t stsIndex = gTrack->GetStsTrackIndex();
		if (stsIndex == -1)continue;
		Int_t richIndex = gTrack->GetRichRingIndex();
		if (richIndex == -1) continue;

		CbmStsTrack* stsTrack = (CbmStsTrack*)fStsTracks->At(stsIndex);
		if (!stsTrack) continue;
		CbmRichRing* richRing = (CbmRichRing*)fRichRings->At(richIndex);
		if (!richRing) continue;

		CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)fStsTrackMatches->At(stsIndex);
		if (!stsTrackMatch) continue;
		CbmRichRingMatch* richRingMatch = (CbmRichRingMatch*)fRichRingMatches->At(richIndex);
		if (!richRingMatch) continue;

		Int_t mcIdSts = stsTrackMatch->GetMCTrackId();
		Int_t mcIdRich = richRingMatch->GetMCTrackID();

		CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(mcIdSts);
        if (!track) continue;
        Int_t pdg = TMath::Abs(track->GetPdgCode());
        Int_t motherId = track->GetMotherId();
        Double_t momentum = track->GetP();

		if (pdg == 11 && motherId == -1 && richRing->GetRecFlag() == 3 &&
				mcIdSts == mcIdRich && mcIdRich != -1 &&
				IsRichElectron(richRing, momentum)){
        	fNofTrueIdRich++;
        	fhTrueIdRich->Fill(momentum);
		}

		/// pions suppression
		if ( pdg == 211 &&  mcIdRich != -1 && IsRichElectron(richRing, momentum)){
        	fNofPiasElRich++;
        	fhPiasElRich->Fill(momentum);
		}

///TRD
		Int_t trdIndex = gTrack->GetTrdTrackIndex();
		if (trdIndex == -1) {
			continue;
		}
		CbmTrdTrack* trdTrack = (CbmTrdTrack*)fTrdTracks->At(trdIndex);
		if (!trdTrack)continue;
		CbmTrackMatch* trdTrackMatch = (CbmTrackMatch*)fTrdTrackMatches->At(trdIndex);
		if (!trdTrackMatch)continue;
		Int_t mcIdTrd = trdTrackMatch->GetMCTrackId();

		if (pdg == 11 && motherId == -1 && richRing->GetRecFlag() == 3 &&
				mcIdSts == mcIdRich && mcIdRich != -1 &&
				IsRichElectron(richRing, momentum)&& IsTrdElectron(trdTrack)){
        	fNofTrueIdRichTrd++;
        	fhTrueIdRichTrd->Fill(momentum);
		}

		/// pions suppression
		if ( pdg == 211 &&  mcIdRich != -1 && IsRichElectron(richRing, momentum) &&
				IsTrdElectron(trdTrack)){
        	fNofPiasElRichTrd++;
        	fhPiasElRichTrd->Fill(momentum);
		}

///TOF
		Int_t tofIndex = gTrack->GetTofHitIndex();
		if (tofIndex == -1)	continue;
		CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
		if (!tofHit)continue;
		Int_t iPoint = tofHit->GetRefId();
		CbmTofPoint* tofPoint = (CbmTofPoint*) fTofPoints->At(iPoint);
		Int_t mcIdTof = tofPoint->GetTrackID();

		if (pdg == 11 && motherId == -1 && richRing->GetRecFlag() == 3
				&& mcIdSts == mcIdRich && mcIdRich != -1 &&
				IsRichElectron(richRing, momentum) && IsTrdElectron(trdTrack)
				&& IsTofElectron(gTrack, momentum)) {
			fNofTrueIdRichTrdTof++;
			fhTrueIdRichTrdTof->Fill(momentum);
		}

		/// pions suppression
		if (pdg == 211 && mcIdRich != -1 && IsRichElectron(richRing, momentum)
				&& IsTrdElectron(trdTrack) && IsTofElectron(gTrack, momentum)) {
			fNofPiasElRichTrdTof++;
			fhPiasElRichTrdTof->Fill(momentum);
		}
	}// global tracks
}

Bool_t CbmRichElectronsQa::IsRichElectron(CbmRichRing* ring, Double_t momentum)
{

    if (fUseRichAnn == false){
    	Double_t axisA = ring->GetAaxis();
    	Double_t axisB = ring->GetBaxis();
    	Double_t dist = ring->GetDistance();
		if ( fabs(axisA-fMeanA) < fRmsCoeff*fRmsA &&
		   fabs(axisB-fMeanB) < fRmsCoeff*fRmsB && dist < fDistCut) return true;
		return false;
    }
///
//    if (fUseRichAnn == false){
//        if(momentum > 5.5) return false;
//
//		if (momentum < 0.5  && dist < 2.) return true;
//		if (momentum >= 0.5 && momentum < 2. ){
//	    	if (dist < (7./3.-2./3.* momentum)) return true;
//		}
//		if (momentum >= 2. && dist < 1.) return true;
//
//        if(ring->GetSelectionNN() < -0.5) return false;
//
//		if (axisA < (fMeanA - fRmsCoeff*fRmsA) ||
//		    axisB < (fMeanB - fRmsCoeff*fRmsB)) return false;
//
//		return false;
//    }

    if (fElIdAnn->DoSelect(ring, momentum) > fRichAnnCut) return true;
    return false;
}

Bool_t CbmRichElectronsQa::IsTrdElectron(CbmTrdTrack* trdTrk)
{
	Double_t ann = trdTrk->GetPidANN();
	if (ann > fTrdAnnCut) return true;

    return false;
}

Bool_t CbmRichElectronsQa::IsTofElectron(CbmGlobalTrack* gTrack, Double_t momentum)
{
	Double_t trackLength = gTrack->GetLength() / 100.;

	// Calculate time of flight from TOF hit
	Int_t tofIndex = gTrack->GetTofHitIndex();
	CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
	if (NULL == tofHit)
		return false;
	Double_t time = 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m

	// Calculate mass squared
	Double_t mass2 = TMath::Power(momentum, 2.) * (TMath::Power(time/trackLength, 2) - 1);
	if (momentum >= 1.) {
		if (mass2 < (0.01 + (momentum - 1.) * 0.09))
			return true;
	} else {
		if (mass2 < 0.)
			return true;//fTofM2
	}
	return false;
}

void CbmRichElectronsQa::DiffElandPi()
{
	Int_t nGlTracks = fGlobalTracks->GetEntriesFast();

	for (Int_t iTrack=0; iTrack<nGlTracks; iTrack++) {

		CbmGlobalTrack* gTrack = (CbmGlobalTrack*)fGlobalTracks->At(iTrack);
		///STS and RICH
		Int_t stsIndex = gTrack->GetStsTrackIndex();
		if (stsIndex == -1)continue;
		Int_t richIndex = gTrack->GetRichRingIndex();
		if (richIndex == -1) continue;

		CbmStsTrack* stsTrack = (CbmStsTrack*)fStsTracks->At(stsIndex);
		if (!stsTrack) continue;
		CbmRichRing* ring = (CbmRichRing*)fRichRings->At(richIndex);
		if (!ring) continue;

		CbmTrackMatch* stsTrackMatch = (CbmTrackMatch*)fStsTrackMatches->At(stsIndex);
		if (!stsTrackMatch) continue;
		CbmRichRingMatch* richRingMatch = (CbmRichRingMatch*)fRichRingMatches->At(richIndex);
		if (!richRingMatch) continue;

		Int_t mcIdSts = stsTrackMatch->GetMCTrackId();
		Int_t mcIdRich = richRingMatch->GetMCTrackID();

		CbmMCTrack* track = (CbmMCTrack*) fMCTracks->At(mcIdSts);
        if (!track) continue;
        Int_t pdg = TMath::Abs(track->GetPdgCode());
        Int_t motherId = track->GetMotherId();
        Double_t momentum = track->GetP();

        Double_t axisA = ring->GetAaxis();
        Double_t axisB = ring->GetBaxis();
        Double_t axisACor = ring->GetAaxisCor();
        Double_t axisBCor= ring->GetBaxisCor();
        Double_t phi = ring->GetPhi();
        Double_t radAngle = ring->GetRadialAngle();
        Double_t chi2 = ring->GetChi2()/ring->GetNDF();
        Double_t radPos = ring->GetRadialPosition();
        Double_t nofHits = ring->GetNofHits();
        Double_t dist = ring->GetDistance();

        if (TMath::IsNaN(axisA) || TMath::IsNaN(axisB)) continue;
        //electrons
		if (pdg == 11 && motherId == -1 && ring->GetRecFlag() == 3 &&
				mcIdSts == mcIdRich && mcIdRich != -1){
			fhAaxisEl->Fill(axisA);
			fhBAxisEl->Fill(axisB);
			fhAaxisCorEl->Fill(axisACor);
			fhBAxisCorEl->Fill(axisBCor);
			fhDistEl->Fill(dist);
			fhNofHitsEl->Fill(nofHits);
			fhChi2El->Fill(chi2);
			fhRadPosEl->Fill(radPos);
			fhAaxisVsMomEl->Fill(momentum, axisA);
			fhBAxisVsMomEl->Fill(momentum, axisB);
			fhPhiVsRadAngEl->Fill(phi, radAngle);
			fOutElandPi << axisA << " " << axisB<< " " << phi <<
				" " << radAngle << " " << chi2 << " " << radPos <<
				" " << nofHits << " " << dist << " " << momentum << " " << 1 << endl;
		}

		/// pions
		if ( pdg == 211 &&  mcIdRich != -1){
			fhAaxisPi->Fill(axisA);
			fhBAxisPi->Fill(axisB);
			fhAaxisCorPi->Fill(axisACor);
			fhBAxisCorPi->Fill(axisBCor);
			fhDistPi->Fill(dist);
			fhNofHitsPi->Fill(nofHits);
			fhChi2Pi->Fill(chi2);
			fhRadPosPi->Fill(radPos);
			fhAaxisVsMomPi->Fill(momentum, axisA);
			fhBAxisVsMomPi->Fill(momentum, axisB);
			fhPhiVsRadAngPi->Fill(phi, radAngle);
			fOutElandPi << axisA << " " << axisB<< " " << phi <<
				" " << radAngle << " " << chi2 << " " << radPos <<
				" " << nofHits << " " << dist << " " << momentum << " "<< -1 << endl ;
		}

///TRD
		Int_t trdIndex = gTrack->GetTrdTrackIndex();
		if (trdIndex == -1) continue;
		CbmTrdTrack* trdTrack = (CbmTrdTrack*)fTrdTracks->At(trdIndex);
		if (!trdTrack)continue;
		CbmTrackMatch* trdTrackMatch = (CbmTrackMatch*)fTrdTrackMatches->At(trdIndex);
		if (!trdTrackMatch)continue;
		Int_t mcIdTrd = trdTrackMatch->GetMCTrackId();

		//electrons
		if (pdg == 11 && motherId == -1 && ring->GetRecFlag() == 3 &&
				mcIdSts == mcIdRich && mcIdRich != -1){
			fhTrdAnnEl->Fill(trdTrack->GetPidANN());
		}

		/// pions
		if ( pdg == 211 &&  mcIdRich != -1){
			fhTrdAnnPi->Fill(trdTrack->GetPidANN());

		}

	}// global tracks

}

void CbmRichElectronsQa::SaveToImage()
{
////	if (fImageOutDir !="") return;
//	TCanvas* c = DrawAcc(fhMCRings, fhAccRings, fhAccRichTrdGlobal, fhAccRichTrdTofGlobal);
//	SaveCanvasAsImage(c, fImageOutDir);
//	c = DrawMatchingEff(fhAccRings, fhTrueFoundRings, fhTrueMatchStsRichGlobal,
//			fhTrueMatchStsRichTrdGlobal,fhTrueMatchStsRichTrdTofGlobal);
//	SaveCanvasAsImage(c, fImageOutDir);
//	c = DrawMatchingEff2(fhAccRings, fhTrueFoundRings, fhTrueMatchStsRichGlobal,
//			fhTrueMatchStsRichTrdGlobal, fhTrueMatchStsRichTrdTofGlobal);
//	SaveCanvasAsImage(c, fImageOutDir);
//	c = DrawElidEff(fhAccRings, fhTrueFoundRings, fhTrueIdRich,
//			fhTrueIdRichTrd, fhTrueIdRichTrdTof,
//			fhAccPi, fhPiasElRich, fhPiasElRichTrd, fhPiasElRichTrdTof);
//	SaveCanvasAsImage(c, fImageOutDir);

}

// -----   Finish Task   ---------------------------------------------------
void CbmRichElectronsQa::FinishTask()
{
	SaveToImage();
	fhMCRings->Write();
	fhAccRings->Write();
	fhTrueFoundRings->Write();
	fhTrueMatchRings->Write();
	fhTrueElIdRings->Write();

	fhAccRichTrdGlobal->Write();
	fhAccRichTrdTofGlobal->Write();

	fhTrueMatchStsRichGlobal->Write();
	fhTrueMatchStsRichTrdGlobal->Write();
	fhTrueMatchStsRichTrdTofGlobal->Write();

	fhTrueIdRich->Write();
	fhAccPi->Write();
	fhPiasElRich->Write();
	fhTrueIdRichTrd->Write();
	fhPiasElRichTrd->Write();
	fhTrueIdRichTrdTof->Write();
	fhPiasElRichTrdTof->Write();

// difference between electrons and pions
	fhAaxisEl->Scale(1./fhAaxisEl->Integral());
	fhAaxisPi->Scale(1./fhAaxisPi->Integral());
	fhBAxisEl->Scale(1./fhBAxisEl->Integral());
	fhBAxisPi->Scale(1./fhBAxisPi->Integral());
	fhAaxisCorEl->Scale(1./fhAaxisCorEl->Integral());
	fhAaxisCorPi->Scale(1./fhAaxisCorPi->Integral());
	fhBAxisCorEl->Scale(1./fhBAxisCorEl->Integral());
	fhBAxisCorPi->Scale(1./fhBAxisCorPi->Integral());
	fhDistEl->Scale(1./fhDistEl->Integral());
	fhDistPi->Scale(1./fhDistPi->Integral());
	fhNofHitsEl->Scale(1./fhNofHitsEl->Integral());
	fhNofHitsPi->Scale(1./fhNofHitsPi->Integral());
	fhChi2El->Scale(1./fhChi2El->Integral());
	fhChi2Pi->Scale(1./fhChi2Pi->Integral());
	fhRadPosEl->Scale(1./fhRadPosEl->Integral());
	fhRadPosPi->Scale(1./fhRadPosEl->Integral());
	fhAaxisVsMomEl->Scale(1./fhAaxisVsMomEl->Integral());
	fhAaxisVsMomPi->Scale(1./fhAaxisVsMomPi->Integral());
	fhBAxisVsMomEl->Scale(1./fhBAxisVsMomEl->Integral());
	fhBAxisVsMomPi->Scale(1./fhBAxisVsMomPi->Integral());
	fhPhiVsRadAngEl->Scale(1./fhPhiVsRadAngEl->Integral());
	fhPhiVsRadAngPi->Scale(1./fhPhiVsRadAngPi->Integral());
	fhTrdAnnEl->Scale(1./fhTrdAnnEl->Integral());
	fhTrdAnnPi->Scale(1./fhTrdAnnPi->Integral());

	fhAaxisEl->Write();
	fhAaxisPi->Write();
	fhBAxisEl->Write();
	fhBAxisPi->Write();
	fhAaxisCorEl->Write();
	fhAaxisCorPi->Write();
	fhBAxisCorEl->Write();
	fhBAxisCorPi->Write();
	fhDistEl->Write();
	fhDistPi->Write();
	fhNofHitsEl->Write();
	fhNofHitsPi->Write();
	fhChi2El->Write();
	fhChi2Pi->Write();
	fhRadPosEl->Write();
	fhRadPosPi->Write();
	fhAaxisVsMomEl->Write();
	fhAaxisVsMomPi->Write();
	fhBAxisVsMomEl->Write();
	fhBAxisVsMomPi->Write();
	fhPhiVsRadAngEl->Write();
	fhPhiVsRadAngPi->Write();
	fhTrdAnnEl->Write();;
	fhTrdAnnPi->Write();;
}

ClassImp(CbmRichElectronsQa)

