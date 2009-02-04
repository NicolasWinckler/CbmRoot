/*
 Description: testing macro
 Input: reconstructed TRD tracks and identified (add ANN method to reco macro)

 Author : Semeon Lebedev
 E-mail : S.Lebedev@gsi.de
 */
#include <vector>

void trd_elid_test()
{
    // ----  Load libraries   -------------------------------------------------
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gSystem->Load("libGeoBase");
	gSystem->Load("libParBase");
	gSystem->Load("libBase");
	gSystem->Load("libCbmBase");
	gSystem->Load("libField");
	gSystem->Load("libGen");
	gSystem->Load("libPassive");
	gSystem->Load("libMvd");
	gSystem->Load("libSts");
	gSystem->Load("libRich");
	gSystem->Load("libTrd");
	// ------------------------------------------------------------------------

	TH1D* hNofHitsEl = new TH1D("hNofHitsEl", "hNofHitsEl", 12, 0, 12.1);
	TH1D* hNofHitsPi = new TH1D("hNofHitsPi", "hNofHitsPi", 12, 0, 12.1);
	TH1D* hPidANNEl[7];
	TH1D* hPidANNPi[7];
	char histTitle[50], histName[50];
	for (Int_t i = 0; i < 7; i++){
		sprintf(histName, "hPidANNEl%d", i);
		sprintf(histTitle, "%d hits in track", 12 - i);
		hPidANNEl[i] = new TH1D(histName, histTitle, 100, -1.2, 1.2);
		sprintf(histName, "hPidANNPi%d", i);
		sprintf(histTitle, "%d hits in track", 12 - i);
		hPidANNPi[i] = new TH1D(histName, histTitle, 100, -1.2, 1.2);
	}

	Double_t nofElTracks = 0, nofPiTracks = 0;

	char fileMC[200], fileRec[200];
	for (int iFile = 0; iFile <= 0; iFile++) {
		sprintf(fileMC, "/d/cbm02/slebedev/trd/FEB09/mom.piel.%.4i.mc.root", iFile);
		cout << fileMC << endl;
		TFile *f1 = new TFile(fileMC, "R");
		TTree* t1 = (TTree*) f1->Get("cbmsim");
		TFolder *fd1 = f1->Get("cbmroot");
		TClonesArray* fListStack = (TClonesArray*) fd1->FindObjectAny("MCTrack");
		t1->SetBranchAddress("MCTrack", &fListStack);
		TClonesArray *pointsTRD = (TClonesArray*) fd1->FindObjectAny("TRDPoint");
		t1->SetBranchAddress(pointsTRD->GetName(), &pointsTRD);

		sprintf(fileRec, "/d/cbm02/slebedev/trd/FEB09/mom.piel.%.4i.reco.root",	iFile);
		cout << fileRec << endl;
		TFile *f = new TFile(fileRec, "R");
		TTree* t = f->Get("cbmsim");
		TFolder *fd = f->Get("cbmout");
		TClonesArray *tracksTRD = (TClonesArray*) fd->FindObjectAny("TRDTrack");
		t->SetBranchAddress(tracksTRD->GetName(), &tracksTRD);
		TClonesArray *matchTRD = (TClonesArray*) fd->FindObjectAny("TRDTrackMatch");
		t->SetBranchAddress(matchTRD->GetName(), &matchTRD);
		TClonesArray *hitsTRD = (TClonesArray*) fd->FindObjectAny("TRDHit");
		t->SetBranchAddress(hitsTRD->GetName(), &hitsTRD);

		CbmTrdTrack *trdtrack = NULL;
		CbmTrdHit *trdhit = NULL;
		CbmMCTrack* mctrack = NULL;
		Int_t nEvents = t->GetEntries();
		cout << " nEvents =" << nEvents << endl;
		//Int_t nEvents=5000;
		for (Int_t ievent = 0; ievent < nEvents; ievent++) {
			cout << "ievent = " << ievent << endl;

			t1->GetEntry(ievent);
			t->GetEntry(ievent);

			Int_t NofTrdTracks = tracksTRD->GetEntries();

			for (Int_t iTrdTrack = 0; iTrdTrack < NofTrdTracks; iTrdTrack++) {

				CbmTrdTrack* trdtrack = (CbmTrdTrack*) tracksTRD->At(iTrdTrack);
				Int_t nHits = trdtrack->GetNofTrdHits();

				if (nHits < 0) {
					continue;
				}

				CbmTrdTrackMatch* match = (CbmTrdTrackMatch*) matchTRD->At(iTrdTrack);
				Int_t iMC = match->GetMCTrackID();
				if (iMC == -1)continue;
				if (iMC > fListStack->GetEntriesFast())	continue;

				mctrack = (CbmMCTrack*) fListStack->At(iMC);
				Int_t partPdg = TMath::Abs(mctrack->GetPdgCode());
				Double_t momMC = mctrack->GetP();
				Double_t PtMC = mctrack->GetPt();
				Double_t Pz = mctrack->GetPz();
				//cout<<PtMC << endl;
				Double_t motherId = mctrack->GetMotherId();

				if (partPdg == 50000050)continue;

				///only primary tracks
				if (motherId != -1)	continue;

				Double_t pidANN = trdtrack->GetPidANN();

				if (partPdg == 11) {
					hNofHitsEl->Fill(nHits);
					nofElTracks++;
				}

				if (partPdg == 211) {
					hNofHitsPi->Fill(nHits);
					nofPiTracks++;
				}

				if (nHits < 6) continue;
				if (nHits > 12){
					cout << "-E- nHits = " << nHits << endl;
					continue;
				}
				Int_t indexAr = 12 - nHits;
				if (partPdg == 11) hPidANNEl[indexAr]->Fill(pidANN);
				if (partPdg == 211) hPidANNPi[indexAr]->Fill(pidANN);

			}//iTrdTrack

		} //event
		f->Close();
		f1->Close();
	}//iFile
	cout << "nofPiTracks = " << nofPiTracks << endl;
	cout << "nofElTracks = " << nofElTracks << endl;

	hNofHitsEl->SetLineWidth(3);
    hNofHitsPi->SetLineWidth(3);
    hNofHitsEl->SetLineStyle(2);
    for (Int_t i = 0; i < 7; i++){
		hPidANNEl[i]->SetLineWidth(3);
		hPidANNPi[i]->SetLineWidth(3);
		hPidANNEl[i]->SetLineStyle(2);
    }

    c1 = new TCanvas("c1","c1",10,10,800,800);
    c1->Divide(2,2);
    c1->cd(1);
    hNofHitsEl->Draw();
    hNofHitsPi->Draw("same");
	gPad->SetLogy(true);

    for (Int_t i = 0; i < 3; i++){
    	c1->cd(i+2);
    	hPidANNEl[i]->Draw();
    	hPidANNPi[i]->Draw("same");
    	gPad->SetLogy(true);
    }

    c2 = new TCanvas("c2","c2",10,10,800,800);
    c2->Divide(2,2);
    for (Int_t i = 0; i < 4; i++){
    	c2->cd(i+1);
    	hPidANNEl[i+3]->Draw();
    	hPidANNPi[i+3]->Draw("same");
    	gPad->SetLogy(true);
    }
}
