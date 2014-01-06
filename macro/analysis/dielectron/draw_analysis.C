/** draw_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 2.0
 **/

void draw_analysis() {
    gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
    loadlibs();
    gSystem->Load("libAnalysis");

    Bool_t useMvd = false;
    Bool_t drawSignificance = true;
    std::string dir = "/Users/slebedev/Development/cbm/data/lmvm/nov13/25gev/trd/1.0field/nomvd/rho0/";
    std::string fileName = dir + "analysis.auau.25gev.centr.all.root";
    std::string outputDir = dir + "results_ana/realpid/";
  // std::string outputDir = "";

    CbmAnaDielectronTaskDraw *draw = new CbmAnaDielectronTaskDraw();
    draw->DrawHistFromFile(fileName, outputDir, useMvd);//, drawSignificance);
}
