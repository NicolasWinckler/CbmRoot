/*  Description: This macro train the ANN for fake
     rejection algorithm.

    Author : Semen Lebedev
    E-mail : S.Lebedev@gsi.de
*/
void run_train_ann_select(
      Int_t nEvents = 200)
{
   Int_t iVerbose = 0;

   TString script = TString(gSystem->Getenv("SCRIPT"));
   TString parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");

   gRandom->SetSeed(10);

   TString inFile1 = "", inFile2 = "", inFile3 = "", parFile = "", outFile ="";

   Bool_t isRichTrackingOn = false;
   if (script != "yes") {
   TString inFile1 ="/d/cbm02/slebedev/rich/JUL09/mc.0000.root";
   TString inFile2 ="/d/cbm02/slebedev/rich/JUL09/reco.0000.root";
   TString parFile ="/d/cbm02/slebedev/rich/JUL09/param.0000.root";
   TString outFile ="/d/cbm02/slebedev/rich/JUL09/richannselect.0000.root";
   }

   TString stsDigiFile = "sts_standard.digi.par";

   gDebug = 0;

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   FairRunAna *run = new FairRunAna();
   if (inFile1 != "") run->SetInputFile(inFile2);
   if (inFile2 != "") run->AddFriend(inFile1);
   if (outFile != "") run->SetOutputFile(outFile);

   CbmRichRingQa* richQa = new CbmRichRingQa();
   run->AddTask(richQa);

   // -----  Parameter database   --------------------------------------------
   TString stsDigi = gSystem->Getenv("VMCWORKDIR");
   stsDigi += "/parameters/sts/";
   stsDigi += stsDigiFile;
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   FairParAsciiFileIo* parIo2 = new FairParAsciiFileIo();
   parIo1->open(parFile.Data());
   parIo2->open(stsDigi.Data(), "in");
   rtdb->setFirstInput(parIo1);
   rtdb->setSecondInput(parIo2);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();

   run->Init();
   run->Run(0, nEvents);
}
