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
    std::string setup = "100field/mvd";
   // std::string fileName = "analysis.pi_misid_0.0001.all.root";
    std::string fileName = "analysis.all.root";

    Bool_t drawSignificance = true;
    std::string fileName2 = "/lustre/cbm/user/ebelolap/aug11/sep12/25gev/"+setup+"/rho0/" + fileName;

    std::string outputDir = "results/25gev/" + setup + "/rho0/real_pid/";
    //std::string outputDir = "results/25gev/" + setup + "/rho0/pi_misid_0_0001/";


    CbmAnaDielectronTaskDraw *draw = new CbmAnaDielectronTaskDraw();
    draw->DrawHistFromFile(fileName2, outputDir, useMvd);//, drawSignificance);
}
