eventDisplay()
{

  TString  InputFile     ="data/test.mc.root";
  TString  ParFile       ="data/params.root";

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
  FairMCPointDraw *TofPoint =    new FairMCPointDraw ("TofPoint",kBlue,  kFullSquare);
  FairMCPointDraw *TrdPoint= new FairMCPointDraw ("TrdPoint",kTeal,  kFullSquare);
  FairMCPointDraw *EcalPoint =    new FairMCPointDraw ("EcalPoint",kYellow,  kFullSquare);
  FairMCPointDraw *RefPlanePoint = new FairMCPointDraw ("RefPlanePoint",kPink,  kFullSquare);
  FairMCPointDraw *StsPoint = new FairMCPointDraw ("StsPoint",kCyan,  kFullSquare);
                                                               
  fMan->AddTask(Track);
  
  fMan->AddTask(RichPoint);
  fMan->AddTask(EcalPoint);   
  fMan->AddTask(TofPoint);   
  fMan->AddTask(TrdPoint);
  fMan->AddTask(RefPlanePoint);
  fMan->AddTask(StsPoint);
 
    
  fMan->Init(1,4,10000);


}
