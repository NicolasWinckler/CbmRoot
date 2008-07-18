/* $Id: CbmEcalAnalysis.C,v 1.3 2006/04/25 08:20:51 kharlov Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalAnalysis.C,v $
 * Revision 1.3  2006/04/25 08:20:51  kharlov
 * Full chain of ECAL simulation from MCpoints to Hits and analysis
 *
 * Revision 1.2  2006/02/17 23:41:48  sgorboun
 * the order of loading of libraries changed
 *
 * Revision 1.1  2006/02/03 16:03:23  kharlov
 * ECAL analysis task
 *
 */

void CbmEcalAnalysis(const TString infile = "test.mc.root")
{
  //Macro to run ECAL analysis.
 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libField");
  gSystem->Load("libBase");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");

  CbmRunAna *fRun = new CbmRunAna();
  CbmRuntimeDb *rtdb = fRun->GetRuntimeDb();
  CbmParRootFileIo *io1 = new CbmParRootFileIo();
  io1->open("parfiles/testparams.root");
  rtdb->setFirstInput(io1);

  TString outfile=infile;
  outfile.ReplaceAll(".root",".analysis.root");
  fRun->SetInputFile(infile);
  fRun->SetOutputFile(outfile);

  CbmEcalAnalysisDS* hp= new CbmEcalAnalysisDS("EcalAnalysis","Ecal analysis");
  hp->SetFullEnergyFlag(0);
  fRun->AddTask(hp);

  fRun->Init();
  fRun->Run();
  delete hp;

} 
