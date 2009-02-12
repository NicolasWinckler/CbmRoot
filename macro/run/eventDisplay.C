

eventDisplay()
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();  
  gSystem->Load("libEve");
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
//  gSystem->Load("libTrd");
  gSystem->Load("libTof");
  gSystem->Load("libEcal");
  gSystem->Load("libEventDisplay");
  gSystem->Load("libGeom");
                                     
  // -----   Reconstruction run   -------------------------------------------
  FairRunAna *fRun= new FairRunAna();
  fRun->SetInputFile("data/test.mc.root");
  //fRun->SetInputFile("../dsk/sim_dsk.root");
 // fRun->SetInputFile("../dsk/sim_dsk.g4native.root");
  fRun->SetOutputFile("test.root");
  fRun->LoadGeometry();

  FairEventManager *fMan= new FairEventManager();
  FairMCTracks *Track =  new FairMCTracks ("Monte-Carlo Tracks");
  FairMCPointDraw *RichPoint =   new FairMCPointDraw ("RichPoint",kOrange,  kFullSquare);
  FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TOFPoint",kBlue,  kFullSquare);
  //FairMCPointDraw *TrdPoint= new FairMCPointDraw ("TRDPoint",kTeal,  kFullSquare);
  FairMCPointDraw *ECalPoint =    new FairMCPointDraw ("ECALPoint",kYellow,  kFullSquare);
  FairMCPointDraw *RefPlanePoint = new FairMCPointDraw ("RefPlanePoint",kPink,  kFullSquare);
  FairMCPointDraw *STSPoint = new FairMCPointDraw ("STSPoint",kCyan,  kFullSquare);
                                                               
  fMan->AddTask(Track);
  
  fMan->AddTask(RichPoint);
  fMan->AddTask(ECalPoint);   
  fMan->AddTask(TofPoint);   
  //fMan->AddTask(TrdPoint);
  fMan->AddTask(RefPlanePoint);
  fMan->AddTask(STSPoint);
 
    
  fMan->Init();                     
  


}
