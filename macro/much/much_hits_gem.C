/**
 * Performs reconstruction of simulated events
 * in the CBM MuCh setup: STS + MUCH.
 *
 * @author M.Ryzhinskiy m.ryzhinskiy@gsi.de
 * @param inFile         Input transport file
 * @param digiFile       Input file containing segmentation parameters
 * @param outFile        Output file for reconstruction data
 * @param nEvents        Number of events to process
 */
void much_hits_gem(TString inFile = "",//"data/mc.standard.500.root",
      TString digiFile = "",
      TString outFile = "",//"data/hits.standard.500.root",
      Int_t nEvents = 5)
{
   TString inputdir = gSystem->Getenv("VMCWORKDIR");
   if (inFile == "") {
      inFile = "data/Jpsi.auau.25gev.centr.mc.root";
   }
   if (digiFile == "") {
      digiFile = inputdir + "/parameters/much/much_standard.digi.root";
   }
   if (outFile == "") {
      outFile = "data/Jpsi.auau.25gev.centr.muchhits.root";
   }

   // ========================================================================
   //          Adjust this part according to your requirements

   // Verbosity level (0=quiet, 1=event level, 2=track level, 3=debug)
   Int_t iVerbose = 1;

   // ----  Load libraries   -------------------------------------------------
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
   //  gSystem->Load("libMvd");
   gSystem->Load("libSts");
   gSystem->Load("libRich");
   gSystem->Load("libTrd");
   gSystem->Load("libTof");
   gSystem->Load("libEcal");
   gSystem->Load("libGlobal");
   gSystem->Load("libKF");
   gSystem->Load("libL1");
   gSystem->Load("libMuch");
   // ------------------------------------------------------------------------

   // -----   Reconstruction run   -------------------------------------------
   FairRunAna *fRun= new FairRunAna();
   fRun->SetInputFile(inFile);
   fRun->SetOutputFile(outFile);
   // ------------------------------------------------------------------------


   // -----  Parameter database   --------------------------------------------
   FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
   FairParRootFileIo*  parIo1 = new FairParRootFileIo();
   parIo1->open(gFile);
   rtdb->setFirstInput(parIo1);
   rtdb->setOutput(parIo1);
   rtdb->saveOutput();
   fRun->LoadGeometry();
   // ------------------------------------------------------------------------

   // ---  MuCh digitizer ----------------------------------------------------
//   CbmMuchDigitizeSimpleGem* digitize = new CbmMuchDigitizeSimpleGem("MuchDigitizeSimpleGem", digiFile, iVerbose);
   CbmMuchDigitizeAdvancedGem* digitize = new CbmMuchDigitizeAdvancedGem("MuchDigitizeAdvancedGem", digiFile, iVerbose);
   digitize->SetSpotRadius(0.15);
   digitize->SetQThreshold(3);
   digitize->SetQMaximum(500000);
   digitize->SetNADCChannels(256);
   fRun->AddTask(digitize);
   // ------------------------------------------------------------------------

   // ---  MuCh hit finder ---------------------------------------------------
//   CbmMuchFindHitsSimpleGem* findHits = new CbmMuchFindHitsSimpleGem("MuchFindHitsSimpleGem", digiFile, iVerbose);
   CbmMuchFindHitsAdvancedGem* findHits = new CbmMuchFindHitsAdvancedGem("MuchFindHitsAdvancedGem", digiFile, iVerbose);
   findHits->SetAlgorithm(4);

   // ============================== Parameters for the 1st station ==========================================
   const Int_t iStation = 0; // Station number for which parameters should be set
   const Int_t nLimits = 2; // Number of limits (number of contributed MC-points) for the cluster in the given station
   const Int_t nDim = nLimits+1;
   Double_t distancesSt1Pad2[nDim] = {nLimits, 50, 70}; // Distances for the 2-pad clusters
   Double_t distancesSt1Pad3[nDim] = {nLimits, 60, 80}; // Distances for the 3-pad clusters
   Double_t distancesSt1Pad4[nDim] = {nLimits, 70, 90}; // Distances for the 4-pad clusters
   Int_t chargesSt1Pad2[nDim] = {nLimits, 40, 55};   // Charges for the 2-pad clusters
   Int_t chargesSt1Pad3[nDim] = {nLimits, 68, 78};   // Charges for the 3-pad clusters
   Int_t chargesSt1Pad4[nDim] = {nLimits, 65, 87};   // Charges for the 4-pad clusters

   findHits->SetClusterDistanceLimits(iStation, 2, distancesSt1Pad2); // Set mean distances for 2-pad cluster
   findHits->SetClusterCharges(iStation, 2, chargesSt1Pad2); // Set mean charges for 2-pad cluster
   findHits->SetClusterDistanceLimits(iStation, 3, distancesSt1Pad3); // Set mean distances for 3-pad cluster
   findHits->SetClusterCharges(iStation, 3, chargesSt1Pad3); // Set mean charges for 3-pad cluster
   findHits->SetClusterDistanceLimits(iStation, 4, distancesSt1Pad4); // Set mean distances for 4-pad cluster
   findHits->SetClusterCharges(iStation, 4, chargesSt1Pad4); // Set mean charges for 4-pad cluster
   // ========================================================================================================

   // ============================== Parameters for the 2nd station ==========================================
   // Set parameters for the 2nd station (if needed)
   // ========================================================================================================

   // ============================== Parameters for the 3rd station ==========================================
   // Set parameters for the 3rd station (if needed)
   // ========================================================================================================

   // ============================== Parameters for the 4th station ==========================================
   // Set parameters for the 4th station (if needed)
   // ========================================================================================================

   // ============================== Parameters for the 5th station ==========================================
   // Set parameters for the 5th station (if needed)
   // ========================================================================================================

   // ============================== Parameters for the 6th station ==========================================
   // Set parameters for the 6th station (if needed)
   // ========================================================================================================

   fRun->AddTask(findHits);
   // ------------------------------------------------------------------------

   // -----   Initialize and run   -------------------------------------------
   fRun->Init();
   fRun->Run(0,nEvents);
   // ------------------------------------------------------------------------

   cout << " Test passed" << endl;
   cout << " All ok " << endl;
   exit(0);
}
