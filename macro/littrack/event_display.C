void event_display()
{
	TString dir  = "/home/d/andrey/std_1e+/";
	// MC transport file
	TString mcFile = dir + "mc.0000.root";
	// Parameter file
	TString parFile = dir + "param.0000.root";

	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
	basiclibs();
	gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/cbmrootlibs.C");
	cbmrootlibs();
	gSystem->Load("libEve");
	gSystem->Load("libEventDisplay");

	FairRunAna *run= new FairRunAna();

	run->SetInputFile(mcFile);
	run->SetOutputFile("test.root");
	run->LoadGeometry();

	FairEventManager *fMan= new FairEventManager();
	FairMCTracks *Track = new FairMCTracks ("Monte-Carlo Tracks");
	FairMCPointDraw *RichPoint = new FairMCPointDraw ("RichPoint",kOrange,  kFullSquare);
	FairMCPointDraw *TofPoint = new FairMCPointDraw ("TOFPoint",kBlue,  kFullSquare);
	FairMCPointDraw *TrdPoint = new FairMCPointDraw ("TRDPoint",kTeal,  kFullSquare);
	FairMCPointDraw *MuchPoint = new FairMCPointDraw ("MuchPoint",kRed,  kFullSquare);
	FairMCPointDraw *ECalPoint = new FairMCPointDraw ("ECALPoint",kYellow,  kFullSquare);
	FairMCPointDraw *RefPlanePoint = new FairMCPointDraw ("RefPlanePoint",kPink,  kFullSquare);
	FairMCPointDraw *STSPoint = new FairMCPointDraw ("STSPoint",kCyan,  kFullSquare);

	fMan->AddTask(Track);
	fMan->AddTask(RichPoint);
	fMan->AddTask(ECalPoint);
	fMan->AddTask(TofPoint);
	fMan->AddTask(TrdPoint);
	fMan->AddTask(MuchPoint);
	fMan->AddTask(RefPlanePoint);
	fMan->AddTask(STSPoint);


	// -----  Parameter database   --------------------------------------------
	FairRuntimeDb* rtdb = run->GetRuntimeDb();
	FairParRootFileIo* parIo1 = new FairParRootFileIo();
	parIo1->open(parFile.Data());
	rtdb->setFirstInput(parIo1);
	rtdb->setOutput(parIo1);
	rtdb->saveOutput();
	// ------------------------------------------------------------------------

	fMan->Init();
}
