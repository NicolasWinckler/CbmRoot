using namespace std;
void draw_analysis_study()
{
   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();

   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   gSystem->Load("libAnalysis");

   //std::string fileName = "analysis.pi_misid_0.0001.all.root";
   std::string fileName = "analysis.all.root";
   Bool_t useMvd = false;

   //std::string outputDir = "results/25gev/100field/nomvd/pi_miss_study/";
   std::string outputDir = "results/25gev/study/";

   vector<string> files;
   files.push_back("/lustre/cbm/user/ebelolap/aug11/sep12/25gev/100field/nomvd/rho0/" + fileName);
   files.push_back("/lustre/cbm/user/ebelolap/aug11/sep12/25gev/100field/mvd/rho0/" + fileName);
   files.push_back("/lustre/cbm/user/ebelolap/aug11/sep12/25gev/70field/nomvd/rho0/" + fileName);
   files.push_back("/lustre/cbm/user/ebelolap/aug11/sep12/25gev/70field/mvd/rho0/" + fileName);

   vector<string> filesMean;
   string dir = "/u/slebedev/JUL09/trunk/macro/analysis/dielectron/results/25gev/";
   filesMean.push_back(string(dir + "100field/nomvd/all/real_pid/mean_hist.root"));
   filesMean.push_back(string(dir + "100field/mvd/all/real_pid/mean_hist.root"));
   filesMean.push_back(string(dir + "70field/nomvd/all/real_pid/mean_hist.root"));
   filesMean.push_back(string(dir + "70field/mvd/all/real_pid/mean_hist.root"));

   vector<string> studyNames;
   studyNames.push_back("100field/nomvd");
   studyNames.push_back("100field/mvd");
   studyNames.push_back("70field/nomvd");
   studyNames.push_back("70field/mvd");

//   vector<string> files;
//   string dir = "/lustre/cbm/user/ebelolap/aug11/sep12/25gev/100field/nomvd/rho0/";
//   files.push_back(dir + "analysis.pi_misid_0.01.all.root");
//   files.push_back(dir + "analysis.pi_misid_0.002.all.root");
//   files.push_back(dir + "analysis.pi_misid_0.001.all.root");
//   files.push_back(dir + "analysis.pi_misid_0.0002.all.root");
//   files.push_back(dir + "analysis.pi_misid_0.0001.all.root");
//   files.push_back(dir + "analysis.pi_misid_0.0.all.root");
//
//   vector<string> filesMean;
//   string dir = "/u/slebedev/JUL09/trunk/macro/analysis/dielectron/results/25gev/100field/nomvd/all/";
//   filesMean.push_back(string(dir + "pi_misid_0_01/mean_hist.root"));
//   filesMean.push_back(string(dir + "pi_misid_0_002/mean_hist.root"));
//   filesMean.push_back(string(dir + "pi_misid_0_001/mean_hist.root"));
//   filesMean.push_back(string(dir + "pi_misid_0_0002/mean_hist.root"));
//   filesMean.push_back(string(dir + "pi_misid_0_0001/mean_hist.root"));
//   filesMean.push_back(string(dir + "pi_misid_0_0/mean_hist.root"));
//
//   vector<string> studyNames;
//   studyNames.push_back("1/100");
//   studyNames.push_back("1/500");
//   studyNames.push_back("1/1000");
//   studyNames.push_back("1/5000");
//   studyNames.push_back("1/10000");
//   studyNames.push_back("ideal");

   CbmAnaLmvmDrawStudy *draw = new CbmAnaLmvmDrawStudy();
   draw->DrawFromFile(files, filesMean, studyNames, outputDir);
}
