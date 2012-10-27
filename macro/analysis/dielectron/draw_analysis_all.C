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

    Bool_t useMvd = true;
    std::string dir = "/lustre/cbm/user/ebelolap/aug11/sep12/25gev/";
    std::string setup ="100field/mvd";
    //std::string fileName = "analysis.pi_misid_0.0001.all.root";
    std::string fileName = "analysis.all.root";

    std::string fnRho0 = dir + setup + "/rho0/" + fileName;
    std::string fnOmega = dir + setup + "/omega/" + fileName;
    std::string fnPhi = dir + setup + "/phi/" + fileName;
    std::string fnOmegaD = dir + setup + "/omegadalitz/" + fileName;

    //std::string outputDir = "results/25gev/" + setup + "/all/pi_misid_0_0001/";
    std::string outputDir = "results/25gev/" + setup + "/all/real_pid/";

    CbmAnaDielectronTaskDrawAll *draw = new CbmAnaDielectronTaskDrawAll();
    draw->DrawHistosFromFile(fnRho0, fnOmega, fnPhi, fnOmegaD, outputDir, useMvd);
}
