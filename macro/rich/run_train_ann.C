/*  Description: This macro train the ANN for fake
     rejection algorithm.

    Author : Semen Lebedev
    E-mail : S.Lebedev@gsi.de
*/
void run_train_ann(
      Int_t nEvents = 1000)
{
   gRandom->SetSeed(10);

   TString inFile1 ="/d/cbm06/user/slebedev/rich/mc.0004.root";
   TString inFile2 ="/d/cbm06/user/slebedev/rich/reco.0004.root";
   TString parFile ="/d/cbm06/user/slebedev/rich/param.0004.root";
   TString outFile ="/d/cbm06/user/slebedev/rich/richannselect.0004.root";

   // please specify what you want to run
   // select = train ANN for fake rejection algorithm
   // electron = train ANN for electron identification algorithm
   TString option = "electron"; // or electron

   gDebug = 0;

   gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
   basiclibs();
   gROOT->LoadMacro("$VMCWORKDIR/macro/rich/cbmlibs.C");
   cbmlibs();

   FairRunAna *run = new FairRunAna();
   if (inFile1 != "") run->SetInputFile(inFile2);
   if (inFile2 != "") run->AddFriend(inFile1);
   if (outFile != "") run->SetOutputFile(outFile);

   FairTask* richAnn;
   if (option == "select") richAnn = new CbmRichTrainAnnSelect();
   if (option == "electron") richAnn = new CbmRichTrainAnnElectrons();
   run->AddTask(richAnn);

   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = run->GetRuntimeDb();
   FairParRootFileIo* parIo1 = new FairParRootFileIo();
   parIo1->open(parFile.Data());
   rtdb->setFirstInput(parIo1);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();

   run->Init();
   run->Run(0, nEvents);
}
