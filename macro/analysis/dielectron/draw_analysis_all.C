/** draw_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 3.0
 **/

void draw_analysis_all(){

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
    cbmlibs();

    gSystem->Load("libAnalysis");

    Bool_t useMvd = false;

    std::string dir = "/lustre/cbm/user/ebelolap/aug11/sep12/10gev/";
    std::string setup ="100field/nomvd";
    std::string fnRho0 = dir + setup + "/rho0/analysis.pi_misid_0.0.all.root";
    std::string fnOmega = dir + setup + "/omega/analysis.pi_misid_0.0.all.root";
    std::string fnPhi = dir + setup + "/phi/analysis.pi_misid_0.0.all.root";
    std::string fnOmegaD = dir + setup + "/omegadalitz/analysis.pi_misid_0.0.all.root";

    std::string outputDir = "results/10gev/all/" + setup + "/ideal_pid/";

    CbmAnaDielectronTaskDrawAll *draw = new CbmAnaDielectronTaskDrawAll();
    draw->DrawHistosFromFile(fnRho0, fnOmega, fnPhi, fnOmegaD, outputDir, useMvd);
}
