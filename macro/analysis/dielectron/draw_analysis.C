/** draw_analysis.C
 * @author Elena Lebedeva <e.lebedeva@gsi.de>
 * @since 2010
 * @version 2.0
 **/

void draw_analysis(){

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
    cbmlibs();

    gSystem->Load("libAnalysis");

    std::string setup = "100field/nomvd";
    Bool_t useMvd = true;
    Bool_t drawSignificance = true;
    std::string fileName = "/lustre/cbm/user/ebelolap/aug11/sep12/25gev/"+setup+"/rho0/analysis.all.root";
    std::string outputDir = "results/25gev/rho0/" + setup + "/real_pid/";

    CbmAnaDielectronTaskDraw *draw = new CbmAnaDielectronTaskDraw();
    draw->DrawHistFromFile(fileName, outputDir, useMvd);//, drawSignificance);
}
