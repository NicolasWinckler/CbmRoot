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

    std::string setup ="70field/nomvd";
    std::string fnRho0 = "/lustre/cbm/user/ebelolap/aug11/sep12/25gev/"+setup+"/rho0/analysis.all.root";
    std::string fnOmega = "/lustre/cbm/user/ebelolap/aug11/sep12/25gev/"+setup+"/omega/analysis.all.root";
    std::string fnPhi = "/lustre/cbm/user/ebelolap/aug11/sep12/25gev/"+setup+"/phi/analysis.all.root";
    std::string fnOmegaD = "/lustre/cbm/user/ebelolap/aug11/sep12/25gev/"+setup+"/omegadalitz/analysis.all.root";

    std::string outputDir = "results/all/" + setup + "/";

    CbmAnaDielectronTaskDrawAll *draw = new CbmAnaDielectronTaskDrawAll();
    draw->DrawHistosFromFile(fnRho0, fnOmega, fnPhi, fnOmegaD, outputDir, useMvd);
}
