void glue_electrons_qa ()
{
    TString filePath = "/lustre/cbm/user/ebelolap/oct10/urqmd_rho0/25gev/100_field/real/mytask.analysis";//.0000.root";
    TString outFileName = filePath +".all.25_02_2011.root";
    Int_t nEvents = 200;   // number of files to be added


    Int_t nofGoodFiles = 0;
    for(Int_t i = 0; i < nEvents; i++) {
        char str[4];
        sprintf(str, "%04d", i);
        TString fileName = filePath + "." + TString(str) + ".root";
        cout << "-I- " + fileName << endl;
        TFile* file = new TFile(fileName);
        if (file->IsZombie()) continue;


        nofGoodFiles++;
        file->Close();
    }

    Double_t scale = 1./(Double_t) nofGoodFiles;

    fh_mc_signal_pty->Scale(scale);

///WRITE HISTOGRAMMS TO OUTPUT FILE
    TFile* outFile = new TFile(outFileName, "RECREATE");
    fh_mc_signal_pty->Write();

    cout <<"nofGoodFiles = " << nofGoodFiles << endl;

} 
