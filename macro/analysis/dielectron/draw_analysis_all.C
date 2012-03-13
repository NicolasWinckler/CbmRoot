/** draw_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 3.0
 **/

void draw_analysis(){

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
    cbmlibs();

    gSystem->Load("libAnalysis");

    Bool_t useMvd = true;

    std::string setup ="100field/nomvd";
    std::string fnRho0 = "/lustre/cbm/user/ebelolap/aug11/mar12/25gev/"+setup+"/rho0/analysis.all.root";
    std::string fnOmega = "/lustre/cbm/user/ebelolap/aug11/mar12/25gev/"+setup+"/omega/analysis.all.root";
    std::string fnPhi = "/lustre/cbm/user/ebelolap/aug11/mar12/25gev/"+setup+"/phi/analysis.all.root";
    std::string fnOmegaD = "/lustre/cbm/user/ebelolap/aug11/mar12/25gev/"+setup+"/omegadalitz/analysis.all.root";

    CbmAnaDielectronTaskDrawAll *draw = new CbmAnaDielectronTaskDrawAll();
    draw->DrawHistosFromFile(fnRho0, fnOmega, fnPhi, fnOmegaD, false);
}
