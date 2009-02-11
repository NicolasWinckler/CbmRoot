/* $Id: CbmEcalHitProducingFullMC.C,v 1.6 2006/09/15 12:50:52 turany Exp $ */

/* History of cvs commits:
 *
 * $Log: CbmEcalHitProducingFullMC.C,v $
 * Revision 1.6  2006/09/15 12:50:52  turany
 * load the base before the field
 *
 * Revision 1.5  2006/07/29 00:31:50  prokudin
 * New way of parameter files processing
 *
 * Revision 1.4  2006/07/23 23:46:53  prokudin
 * Modified according to new hit producer
 *
 * Revision 1.3  2006/07/13 15:58:20  prokudin
 * New hitproducer
 *
 * Revision 1.2  2006/06/22 08:34:07  kharlov
 * Change output file name to a meaningful one
 *
 * Revision 1.1  2006/06/22 07:04:52  kharlov
 * Separate scripts for fast and full MC are provided
 *
 * Revision 1.1  2006/04/25 08:20:51  kharlov
 * Full chain of ECAL simulation from MCpoints to Hits and analysis
 *
 */

void CbmEcalHitProducingFullMC(const TString infile = "test.mc.root")
{
  //Macro to run ECAL hit producer, full MC version
 
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

  FairRunAna *fRun = new FairRunAna();
  FairRuntimeDb *rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo *io1 = new FairParRootFileIo();

  TString outfile=infile;
  outfile.ReplaceAll(".root",".hits.root");
  fRun->SetInputFile(infile);
  io1->open(gFile);
  rtdb->setFirstInput(io1);
  fRun->SetOutputFile(outfile);

  CbmEcalHitProducer* hp= new CbmEcalHitProducer("EcalHitProducer",0,"ecal_FullMC.geo");
  hp->SetMCPoints2SumHits();
  fRun->AddTask(hp);

  fRun->Init();
  fRun->Run();
  delete hp;

} 
