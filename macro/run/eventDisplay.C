

eventDisplay()
{

  TString  InputFile     ="data/test.mc.root";
  TString  ParFile       ="data/params.root";


  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();  
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libCbmBase");
  gSystem->Load("libCbmData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");
  gSystem->Load("libMvd");
  gSystem->Load("libSts");
  gSystem->Load("libRich");
  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libEve");
  gSystem->Load("libEventDisplay");
                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();

  fRun->SetInputFile(InputFile.Data());
  fRun->SetOutputFile("data/test.root");

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *RichPoint =   new FairMCPointDraw ("RichPoint",kOrange,  kFullSquare);
  FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TOFPoint",kBlue,  kFullSquare);
  FairMCPointDraw *TrdPoint= new FairMCPointDraw ("TRDPoint",kTeal,  kFullSquare);
  FairMCPointDraw *ECalPoint =    new FairMCPointDraw ("ECALPoint",kYellow,  kFullSquare);
  FairMCPointDraw *RefPlanePoint = new FairMCPointDraw ("RefPlanePoint",kPink,  kFullSquare);
  FairMCPointDraw *STSPoint = new FairMCPointDraw ("STSPoint",kCyan,  kFullSquare);
                                                               
  fMan->AddTask(Track);
  
  fMan->AddTask(RichPoint);
  fMan->AddTask(ECalPoint);   
  fMan->AddTask(TofPoint);   
  fMan->AddTask(TrdPoint);
  fMan->AddTask(RefPlanePoint);
  fMan->AddTask(STSPoint);
 
    
  fMan->Init();                     
  


}
