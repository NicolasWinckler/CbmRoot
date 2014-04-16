/** draw_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2011
 * @version 3.0
 **/

void draw_analysis_all() {
    gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
    loadlibs();
    gSystem->Load("libAnalysis");

    Bool_t useMvd = false;
    std::string dir = "/Users/slebedev/Development/cbm/data/lmvm/apr14_rich/8gev/rich_pmtreduced_30_u/trd4/tofv13/1.0field/nomvd/";

    std::string fileName = "analysis.auau.8gev.centr.all.root";
    //std::string fileName = "analysis.pimisid.0.0.auau.8gev.centr.all.root";
    std::string outputDir = dir + "results_all/";

  /*  std::string script = std::string(gSystem->Getenv("SCRIPT"));
    if (script == "yes"){
       dir = std::string(gSystem->Getenv("DIR"));
       fileName = std::string(gSystem->Getenv("FILE_NAME"));
       outputDir = std::string(gSystem->Getenv("OUTPUT_DIR"));
    }
*/
    std::string fnRho0 = dir + "rho0/" + fileName;
    std::string fnOmega = dir +"omegaepem/" + fileName;
    std::string fnPhi = dir + "phi/" + fileName;
    std::string fnOmegaD = dir +"omegadalitz/" + fileName;

    CbmAnaDielectronTaskDrawAll *draw = new CbmAnaDielectronTaskDrawAll();
    draw->DrawHistosFromFile(fnRho0, fnOmega, fnPhi, fnOmegaD, outputDir, useMvd);
}
