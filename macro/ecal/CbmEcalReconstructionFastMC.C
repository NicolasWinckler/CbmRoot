/* $Id: CbmEcalReconstructionFastMC.C,v 1.2 2006/09/15 12:50:52 turany Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalReconstructionFastMC.C,v $
 * Revision 1.2  2006/09/15 12:50:52  turany
 * load the base before the field
 *
 * Revision 1.1  2006/06/22 14:02:17  kharlov
 * First upload of reconstruction classes for Full MC
 *
 */

void CbmEcalReconstructionFastMC(const TString infile = "testFastMC.mc.hits.root")
{
  //Macro to run ECAL hit producer, fast MC version
 
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libGlobal");
  gSystem->Load("libKF");

  FairRunAna *fRun = new FairRunAna();
  FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo *io1 = new FairParRootFileIo();
  io1->open("parfiles/testparams.root");
  rtdb->setFirstInput(io1);

  TString outfile=infile;
  outfile.ReplaceAll(".hits.root",".recparticles.root");
  fRun->SetInputFile(infile);
  fRun->SetOutputFile(outfile);

  CbmEcalReconstructionFastMC* hp= new CbmEcalReconstructionFastMC("EcalHitReconstructionFastMC");
  fRun->AddTask(hp);

  fRun->Init();
  fRun->Run();
  delete hp;

} 
