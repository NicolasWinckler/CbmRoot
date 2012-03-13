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

    Bool_t useMvd = true;
    Bool_t drawSignificance = true;

    std::string fileName = "/lustre/cbm/user/ebelolap/aug11/mar12/25gev/70field/nomvd/rho0/analysis.all.root";

    CbmAnaDielectronTaskDraw *draw = new CbmAnaDielectronTaskDraw();
    draw->DrawHistosFromFile(fileName, useMvd);//, drawSignificance);
}
