/**
 * \file global_sim.C
 * \brief Macro runs simulation for "electron" or "muon" setups of CBM.
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2010
 **/

#include <iostream>
using std::cout;
using std::endl;


void global_sim_new(Int_t nEvents = 5, TString setup = "electron",
		    TString mcDir = "", TString fileNr="0000",
		    TString definitionFile= "",
		    TString definitionDir= "./",
		    )
{
  
  // Geometries
  TString caveGeom;
  TString targetGeom;
  TString pipeGeom;
  TString shieldGeom;
  TString mvdGeom;
  TString stsGeom; 
  TString muchGeom; 
  TString richGeom;
  TString trdGeom;
  TString tofGeom;
  TString ecalGeom;
  TString magnetGeom;
  TString fieldMap;

  // Generators
  TString inputDir;
  TString urqmdFile;
  TString urqmd;
  TString unigen;
  Int_t nofMuonsPlus;
  Int_t nofMuonsMinus;
  Int_t nofElectrons;
  Int_t nofPositrons;
  Int_t nofPionsPlus;
  Int_t nofPionsMinus;
  Int_t nofJPsiToMuons;
  Int_t nofJPsiToElectrons;
  Int_t nofAuIons;

  TString mcFile; 
  TString parFile;
  TString resultDir;
  TString globalRecoFile;
  TString globalHitsFile;
  TString globalTracksFile;

  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/SetDefaults.C");
  defaultGeometrie(setup, caveGeom, targetGeom, pipeGeom, shieldGeom, mvdGeom, 
		   stsGeom, richGeom, muchGeom, trdGeom, tofGeom, ecalGeom, 
		   fieldMap, magnetGeom);

  defaultGenerator(setup, inputDir, urqmdFile, urqmd, unigen, nofMuonsPlus, 
		   nofMuonsMinus, nofElectrons, nofPositrons, nofPionsPlus, 
		   nofPionsMinus, nofJPsiToMuons, nofJPsiToElectrons, nofAuIons);

  defaultOutput(setup, fileNr, mcDir, resultDir, mcFile, parFile, 
		    globalRecoFile, globalHitsFile, globalTracksFile);

  if (definitionFile != "" ) {
    definitionFile = definitionDir + definitionFile;
    gROOT->LoadMacro(definitionFile);
    defineGeometrie(setup, caveGeom, targetGeom, pipeGeom, shieldGeom, mvdGeom, 
		    stsGeom, richGeom, muchGeom, trdGeom, tofGeom, ecalGeom, 
		    fieldMap, magnetGeom);

    defineGenerator(inputDir, urqmdFile, urqmd, unigen, nofMuonsPlus, nofMuonsMinus,
		    nofElectrons, nofPositrons, nofPionsPlus, nofPionsMinus,
		    nofJPsiToMuons, nofJPsiToElectrons, nofAuIons);

    defineOutput(setup, mcDir, mcFile, parFile, fileNr);

  }
  
  // -----   Magnetic field   -----------------------------------------------
  Double_t fieldZ = 50.; // field center z position
  Double_t fieldScale = 1.; // field scaling factor
  
  TStopwatch timer;
  timer.Start();
  
  gROOT->LoadMacro("$VMCWORKDIR/macro/littrack/loadlibs.C");
  loadlibs();

  FairRunSim* fRun = new FairRunSim();
  fRun->SetName("TGeant3"); // Transport engine
  fRun->SetOutputFile(mcFile); // Output file
  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  
  fRun->SetMaterials("media.geo"); // Materials
  //	fRun->SetStoreTraj(kTRUE);

  if ( caveGeom != "" ) {
    FairModule* cave = new CbmCave("CAVE");
    cave->SetGeometryFileName(caveGeom);
    fRun->AddModule(cave);
    cout << "    --- " << caveGeom << endl;
  }
  
  if ( pipeGeom != "" ) {
    FairModule* pipe = new CbmPipe("PIPE");
    pipe->SetGeometryFileName(pipeGeom);
    fRun->AddModule(pipe);
    cout << "    --- " << pipeGeom << endl;
  }
  
  if ( shieldGeom != "" ) {
    FairModule* shield = new CbmShield("SHIELD");
    shield->SetGeometryFileName(shieldGeom);
    fRun->AddModule(shield);
    cout << "    --- " << shieldGeom << endl;
  }
  
  if ( targetGeom != "" ) {
    FairModule* target = new CbmTarget("Target");
    target->SetGeometryFileName(targetGeom);
    fRun->AddModule(target);
    cout << "    --- " << targetGeom << endl;
  }
  
  if ( magnetGeom != "" ) {
    FairModule* magnet = new CbmMagnet("MAGNET");
    magnet->SetGeometryFileName(magnetGeom);
    fRun->AddModule(magnet);
    cout << "    --- " << magnetGeom << endl;
  }
  
  if ( mvdGeom != "" ) {
    FairDetector* mvd = new CbmMvd("MVD", kTRUE);
    mvd->SetGeometryFileName(mvdGeom);
    fRun->AddModule(mvd);
  }
  
  if ( stsGeom != "" ) {
    FairDetector* sts = new CbmSts("STS", kTRUE);
    sts->SetGeometryFileName(stsGeom);
    fRun->AddModule(sts);
    cout << "    --- " << stsGeom << endl;
  }
  
  if ( richGeom != "" ) {
    FairDetector* rich = new CbmRich("RICH", kTRUE);
    rich->SetGeometryFileName(richGeom);
    fRun->AddModule(rich);
  }
  
  if ( muchGeom != "" ) {
    FairDetector* much = new CbmMuch("MUCH", kTRUE);
    much->SetGeometryFileName(muchGeom);
    fRun->AddModule(much);
    cout << "    --- " << muchGeom << endl;
  }
  
  if ( trdGeom != "" ) {
    FairDetector* trd = new CbmTrd("TRD",kTRUE );
    trd->SetGeometryFileName(trdGeom);
    fRun->AddModule(trd);
    cout << "    --- " << trdGeom << endl;
  }
  
  if ( tofGeom != "" ) {
    FairDetector* tof = new CbmTof("TOF", kTRUE);
    tof->SetGeometryFileName(tofGeom);
    fRun->AddModule(tof);
    cout << "    --- " << tofGeom << endl;
  }
  
  if ( ecalGeom != "" ) {
    FairDetector* ecal = new CbmEcal("ECAL", kTRUE, ecalGeom.Data());
    fRun->AddModule(ecal);
  }
  // ------------------------------------------------------------------------
  
  // -----   Create magnetic field   ----------------------------------------
  CbmFieldMap* magField = new CbmFieldMapSym2(fieldMap);
  magField->SetPosition(0., 0., fieldZ);
  magField->SetScale(fieldScale);
  fRun->SetField(magField);
  // ------------------------------------------------------------------------
  
  // ------------------------------------------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  
  if (urqmd == "yes" && unigen == "yes") {
    CbmUnigenGenerator*  urqmdGen = new CbmUnigenGenerator(urqmdFile);
    primGen->AddGenerator(urqmdGen);
  }
  
  if (urqmd == "yes" && unigen != "yes") {
    FairUrqmdGenerator* urqmdGen = new FairUrqmdGenerator(urqmdFile);
    primGen->AddGenerator(urqmdGen);
  }
  
  if (nofJPsiToMuons > 0) {
    CbmLitPolarizedGenerator *polGen;
    polGen = new CbmLitPolarizedGenerator(443, nofJPsiToMuons);
    polGen->SetDistributionPt(0.176);        // 25 GeV
    polGen->SetDistributionY(1.9875,0.228);  // 25 GeV
    polGen->SetRangePt(0.,3.);
    polGen->SetRangeY(1.,3.);
    polGen->SetBox(0);
    polGen->SetRefFrame(CbmLitPolarizedGenerator::kHelicity);
    polGen->SetDecayMode(CbmLitPolarizedGenerator::kDiMuon);
    polGen->SetAlpha(0);
    polGen->Init();
    primGen->AddGenerator(polGen);
  }
  
  if (nofJPsiToElectrons > 0) {
    CbmLitPolarizedGenerator *polGen;
    polGen = new CbmLitPolarizedGenerator(443, nofJPsiToElectrons);
    polGen->SetDistributionPt(0.176);        // 25 GeV
    polGen->SetDistributionY(1.9875,0.228);  // 25 GeV
    polGen->SetRangePt(0.,3.);
    polGen->SetRangeY(1.,3.);
    polGen->SetBox(0);
    polGen->SetRefFrame(CbmLitPolarizedGenerator::kHelicity);
    polGen->SetDecayMode(CbmLitPolarizedGenerator::kDiElectron);
    polGen->SetAlpha(0);
    polGen->Init();
    primGen->AddGenerator(polGen);
  }
  
  if (nofMuonsPlus > 0) {
    FairBoxGenerator* boxGen = new FairBoxGenerator(-13, nofMuonsPlus);
    boxGen->SetPRange(1.5, 16.);
    boxGen->SetPhiRange(0.,360.);
    boxGen->SetThetaRange(2.5, 25.);
    boxGen->SetCosTheta();
    boxGen->Init();
    primGen->AddGenerator(boxGen);
  }
  
  if (nofMuonsMinus > 0) {
    FairBoxGenerator* boxGen = new FairBoxGenerator(13, nofMuonsMinus);
    boxGen->SetPRange(1.5, 16.);
    boxGen->SetPhiRange(0.,360.);
    boxGen->SetThetaRange(2.5, 25.);
    boxGen->SetCosTheta();
    boxGen->Init();
    primGen->AddGenerator(boxGen);
  }
  
  if (nofElectrons > 0) {
    FairBoxGenerator* boxGen = new FairBoxGenerator(11, nofElectrons);
    boxGen->SetPtRange(0.,3.);
    //		boxGen->SetPRange(30, 30);
    boxGen->SetPhiRange(0.,360.);
    boxGen->SetThetaRange(2.5,25.);
    boxGen->SetCosTheta();
    boxGen->Init();
    primGen->AddGenerator(boxGen);
  }
  
  if (nofPositrons > 0) {
    FairBoxGenerator* boxGen = new FairBoxGenerator(-11, nofPositrons);
    boxGen->SetPtRange(0.,3.);
    //		boxGen->SetPRange(30, 30);
    boxGen->SetPhiRange(0.,360.);
    boxGen->SetThetaRange(2.5,25.);
    boxGen->SetCosTheta();
    boxGen->Init();
    primGen->AddGenerator(boxGen);
  }
  
  if (nofPionsPlus > 0) {
    FairBoxGenerator* boxGen = new FairBoxGenerator(211, nofPionsPlus);
    //		boxGen1->SetPtRange(0.,3.);
    boxGen->SetPRange(1., 2.);
    boxGen->SetPhiRange(0.,360.);
    boxGen->SetThetaRange(2.5,25.);
    boxGen->SetCosTheta();
    boxGen->Init();
    primGen->AddGenerator(boxGen);
  }
  
  if (nofPionsMinus > 0) {
    FairBoxGenerator* boxGen = new FairBoxGenerator(-211, nofPionsMinus);
    //		boxGen->SetPtRange(0.,3.);
    boxGen->SetPRange(1, 2.);
    boxGen->SetPhiRange(0.,360.);
    boxGen->SetThetaRange(2.5,25.);
    boxGen->SetCosTheta();
    boxGen->Init();
    primGen->AddGenerator(boxGen);
  }
  
  if (nofAuIons >0) {
    FairIonGenerator *AuGen = new FairIonGenerator(79, 197, 79, nofAuIons, 0., 0., 25., 0., 0., -0.1);
    primGen->AddGenerator(AuGen);
  }
  
  
  fRun->SetGenerator(primGen);
  fRun->Init();
  
  // -----   Runtime database   ---------------------------------------------
  CbmFieldPar* fieldPar = (CbmFieldPar*) rtdb->getContainer("CbmFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  fieldPar->setInputVersion(fRun->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
  // ------------------------------------------------------------------------
  
  // -----   Start run   ----------------------------------------------------
  fRun->Run(nEvents);
  // ------------------------------------------------------------------------
  
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished successfully." << endl;
  cout << "Test passed"<< endl;
  cout << " All ok " << endl;
  cout << "Output file is " << mcFile << endl;
  cout << "Real time used: " << rtime << "s " << endl;
  cout << "CPU time used : " << ctime << "s " << endl << endl << endl;
  // ------------------------------------------------------------------------+
}
