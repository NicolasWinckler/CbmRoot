/** draw_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 3.0
 **/

void draw_analysis_all() {
    gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
    loadlibs();
    gSystem->Load("libAnalysis");

    Bool_t useMvd = true;
    std::string dir = "/hera/cbm/users/andrey/mc/dielectron/nov12/8gev/0.7field/mvd/";
    std::string fileName = "analysis.auau.8gev.centr.all.root";
    std::string fnRho0 = dir + "rho0/" + fileName;
    std::string fnOmega = dir +"omegaepem/" + fileName;
    std::string fnPhi = dir + "phi/" + fileName;
    std::string fnOmegaD = dir +"omegadalitz/" + fileName;
    std::string outputDir = dir + "results/";

    CbmAnaDielectronTaskDrawAll *draw = new CbmAnaDielectronTaskDrawAll();
    draw->DrawHistosFromFile(fnRho0, fnOmega, fnPhi, fnOmegaD, outputDir, useMvd);
}
