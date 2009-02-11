/* $Id: CbmEcalUHitProducing.C,v 1.4 2006/09/15 12:50:52 turany Exp $ */
/*
 * $Log: CbmEcalUHitProducing.C,v $
 * Revision 1.4  2006/09/15 12:50:52  turany
 * load the base before the field
 *
 * Revision 1.3  2006/07/29 00:31:50  prokudin
 * New way of parameter files processing
 *
 * Revision 1.2  2006/07/23 23:46:53  prokudin
 * Modified according to new hit producer
 *
 * Revision 1.1  2006/07/13 15:59:08  prokudin
 * New hitproducer
 **/
/*
 * summable hits -> unsummable hits
 */
void CbmEcalUHitProducing(const TString infile = "test.mc.hits.root")
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
  hp->SetSumHits2Hits();
  fRun->AddTask(hp);

  fRun->Init();
  fRun->Run();
  delete hp;

} 
