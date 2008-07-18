/* $Id: CbmEcalHitProducingFastMC.C,v 1.2 2006/06/22 08:34:07 kharlov Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalHitProducingFastMC.C,v $
 * Revision 1.2  2006/06/22 08:34:07  kharlov
 * Change output file name to a meaningful one
 *
 * Revision 1.1  2006/06/22 07:04:52  kharlov
 * Separate scripts for fast and full MC are provided
 *
 */

void CbmEcalHitProducingFastMC(const TString infile = "testFastMC.mc.root")
{
  //Macro to run ECAL hit producer, fast MC version
 
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
  outfile.ReplaceAll(".root",".hits.root");
  fRun->SetInputFile(infile);
  fRun->SetOutputFile(outfile);

  CbmEcalHitProducerFastMC* hp= new CbmEcalHitProducerFastMC("EcalHitProducer");
  fRun->AddTask(hp);

  fRun->Init();
  fRun->Run();
  delete hp;

} 
