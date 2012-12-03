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
    std::string dir = "/hera/cbm/users/andrey/mc/dielectron/nov12/8gev/0.7field/nomvd/phi/";
    std::string fileName = dir + "analysis.auau.8gev.centr.all.root";
    std::string outputDir = dir + "results/all/";

    CbmAnaDielectronTaskDraw *draw = new CbmAnaDielectronTaskDraw();
    draw->DrawHistFromFile(fileName, outputDir, useMvd);//, drawSignificance);
}
