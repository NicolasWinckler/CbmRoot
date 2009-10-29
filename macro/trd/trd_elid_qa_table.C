#include <vector>

void trd_elid_qa_table()
{
	gROOT->LoadMacro("trd_elid_qa_draw.C");
	gStyle->SetPalette(1,0);
    gStyle->SetHistLineWidth(3);
	Int_t nofFiles = 10;

    std::vector<Double_t> fPiELossSum;
    std::vector<Double_t> fElELossSum;
    std::vector<Double_t> fElNofZeroTR;
    std::vector<Double_t> fSigma;
    std::vector<Double_t> fMP;

    fPiELossSum.resize(nofFiles);
    fElELossSum.resize(nofFiles);
    fElNofZeroTR.resize(nofFiles);
    fSigma.resize(nofFiles);
    fMP.resize(nofFiles);

	char recoFileName[150];
	double* par;
//fill arrays
	for (Int_t iFile = 0; iFile < nofFiles; iFile++){
        sprintf(recoFileName,"/d/cbm02/slebedev/trd/JUL09/reco/param3/st/piel.%.4i.reco.root",iFile);
		TFile* file = new TFile(recoFileName);

		fPiELossSum[iFile] = fhPiELossSum->GetMean();
		fElELossSum[iFile] = fhElELossSum->GetMean();
		fElNofZeroTR[iFile] = fhElNofZeroTR->GetMean();
		fhPiELoss->Fit("landau");
		TF1 *fitFcn = fhPiELoss->GetFunction("landau");
		fSigma[iFile] = fitFcn->GetParameter(2);
		fMP[iFile] = fitFcn->GetParameter(1);;
	}

//print arrays
	cout << endl;
	cout <<"------------------------------------------------------------------" << endl;
	cout.width(2);
	cout << "# ";
	cout.width(15);
	cout << "pi eloss sum ";
	cout.width(15);
	cout << "el eloss sum ";
	cout.width(15);
	cout << "el-pi eloss ";
	cout.width(15);
	cout << "nof zero TR ";
	cout.width(15);
	cout << "Sigma ";
	cout.width(15);
	cout << "MP " << endl;
	for (Int_t iFile = 0; iFile < nofFiles; iFile++){
		cout.precision(3);
		cout.width(2);
		cout << iFile << " ";
		cout.width(15);
		cout << fPiELossSum[iFile] << " ";
		cout.width(15);
		cout << fElELossSum[iFile] << " ";
		cout.width(15);
		cout << fElELossSum[iFile] - fPiELossSum[iFile] << " ";
		cout.width(15);
		cout << fElNofZeroTR[iFile] << " ";
		cout.width(15);
		cout << fSigma[iFile]<< " ";
		cout.width(15);
		cout << fMP[iFile] << " " << endl;
	}

}
