#include <vector>

void trd_elid_qa_table()
{
	gStyle->SetPalette(1,0);
    gStyle->SetHistLineWidth(3);
	Int_t nofFiles = 10;

    std::vector<Double_t> fPiELossSum;
    std::vector<Double_t> fElELossSum;
    std::vector<Double_t> fElNofZeroTR;

    fPiELossSum.resize(nofFiles);
    fElELossSum.resize(nofFiles);
    fElNofZeroTR.resize(nofFiles);

	char recoFileName[150];
	for (Int_t iFile = 0; iFile < nofFiles; iFile++){
        sprintf(recoFileName,"/d/cbm02/slebedev/trd/JUL09/st/piel.%.4i.reco.root",iFile);
		TFile* file = new TFile(recoFileName);

		fPiELossSum[iFile] = fhPiELossSum->GetMean();
		fElELossSum[iFile] = fhElELossSum->GetMean();
		fElNofZeroTR[iFile] = fhElNofZeroTR->GetMean();

	}

}
