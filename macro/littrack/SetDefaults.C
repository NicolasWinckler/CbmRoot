void defaultGeometrie(TString setup, TString& caveGeom, TString& targetGeom, 
		      TString& pipeGeom, TString& shieldGeom, TString& mvdGeom, 
		      TString& stsGeom, TString& richGeom, TString& muchGeom, 
		      TString& trdGeom, TString& tofGeom, TString& ecalGeom, 
		      TString& fieldMap, TString& magnetGeom
		      )
{
  if (setup == "muon") {
    cout << "Initializing the default muon geometry." << endl;
    caveGeom   = "cave.geo";
    targetGeom = "target_au_250mu.geo";
    pipeGeom   = "pipe_much.geo";
    shieldGeom = "shield_standard.geo";
    mvdGeom    = "mvd/mvd_v07a.geo";
    stsGeom    = "sts/sts_v11a.geo";
    muchGeom   = "much/much_v11a.geo";
    richGeom   = "";
    trdGeom    = "";
    tofGeom    = "tof/tof_v07a.geo";
    ecalGeom   = "";
    fieldMap   = "field_v10e";
    magnetGeom = "passive/magnet_v09m.geo";
  } else if (setup == "electron") {
    cout << "Initializing the default electron geometry." << endl;
    caveGeom   = "cave.geo";
    targetGeom = "target_au_250mu.geo";
    pipeGeom   = "pipe_standard.geo";
    shieldGeom = "";
    mvdGeom    = "mvd/mvd_v07a.geo";
    stsGeom    = "sts/sts_v11a.geo";
    muchGeom   = ""; 
    richGeom   = "rich/rich_v08a.geo";
    trdGeom    = "trd/trd_v12a.geo";
    tofGeom    = "tof/tof_v07a.geo";
    ecalGeom   = "";//"ecal_FastMC.geo";
    fieldMap   = "field_v10e";
    magnetGeom = "passive/magnet_v09e.geo";
  }
}

void defaultGenerator(TString setup, TString& inputDir, TString& urqmdFile, 
		      TString& urqmd, TString& unigen, Int_t& nofMuonsPlus, 
		      Int_t& nofMuonsMinus, Int_t& nofElectrons, 
		      Int_t& nofPositrons, Int_t& nofPionsPlus, 
		      Int_t& nofPionsMinus, Int_t& nofJPsiToMuons, 
		      Int_t& nofJPsiToElectrons, Int_t& nofAuIons)
{

  inputDir = gSystem->Getenv("VMCWORKDIR");
  urqmdFile = "/input/urqmd.auau.25gev.centr.root";
  urqmdFile = inputDir + urqmdFile;
  urqmd = "yes";
  unigen = "yes";

  if (setup == "muon") {
    cout << "Initializing the default generator for muon setup." << endl;
    nofMuonsPlus = 0;
    nofMuonsMinus = 0;
    nofElectrons = 0;
    nofPositrons = 0;
    nofPionsPlus = 0;
    nofPionsMinus = 0;
    nofJPsiToMuons = 5;
    nofJPsiToElectrons = 0;
    nofAuIons = 0;
  } else if (setup == "electron") {
    cout << "Initializing the default generator for electron setup." << endl;
    nofMuonsPlus = 0;
    nofMuonsMinus = 0;
    nofElectrons = 0;
    nofPositrons = 0;
    nofPionsPlus = 0;
    nofPionsMinus = 0;
    nofJPsiToMuons = 0;
    nofJPsiToElectrons = 5;
    nofAuIons = 0;
  }
}

void defaultOutput(TString setup, TString fileNr, TString& mcDir, 
		   TString& resultDir, TString& mcFile, TString& parFile,
		   TString& globalRecoFile, TString& globalHitsFile, 
		   TString& globalTracksFile);
{
  cout << "Setup the normal output directories and filnames." << endl;
  
  if ( setup == "electron" && mcDir == "" ) {
    mcDir = "./events_electron/";
  } else if ( setup == "muon" && mcDir == "" ) {
    mcDir = "./events_muon/";
  }

  if ( setup == "electron" && resultDir == "" ) {
    resultDir = "./results_electron/";
  } else if ( setup == "muon" && resultDir == "" ) {
    resultDir = "./results_muon/";
  }

  //Monte Carlo file name  
  mcFile = mcDir + "mc." + fileNr + ".root";
  //Parameter file name
  parFile = mcDir + "param." + fileNr + ".root";
  // Output file with reconstructed tracks and hits
  globalRecoFile = mcDir + "global.reco." + fileNr + ".root";
  // File with reconstructed STS tracks, STS, MUCH, TRD and TOF hits and digis
  globalHitsFile = mcDir + "global.hits." + fileNr + ".root";
  // Output file with global tracks
  globalTracksFile = mcDir + "global.tracks." + fileNr + ".root";
}

void defaultRecoSettings(TString& globalTrackingType, TString& stsHitProducerType, 
			 TString& trdHitProducerType, TString& muchHitProducerType)
{
  // Reconstruction parameters
  globalTrackingType = "branch"; // Global tracking type
  stsHitProducerType = "real"; // STS hit producer type: real, ideal
  trdHitProducerType = "smearing"; // TRD hit producer type: smearing, digi, clustering
  muchHitProducerType = "advanced"; // MUCH hit producer type: simple, advanced
}

void defaultNormalization(Int_t& normStsPoints, Int_t& normTrdPoints, 
			  Int_t& normMuchPoints, Int_t& normTofPoints, 
			  Int_t& normTrdHits, Int_t& normMuchHits, 
			  Int_t& normTofHits)
{
  // Normalization for efficiency
  normStsPoints = 4;
  normTrdPoints = 6;
  normMuchPoints = 16;
  normTofPoints = 1;
  normTrdHits = 6;
  normMuchHits = 16;
  normTofHits = 1;
}

void defaultDigiParameter(TString& parDir, TString& muchDigiFile, 
			  TList* parFileList)
{
  parDir = TString(gSystem->Getenv("VMCWORKDIR")) + TString("/parameters");
  muchDigiFile = parDir + "/much/much_v11a.digi.root"; // MUCH digi file

  TObjString stsDigiFile = parDir + "/sts/sts_v11a.digi.par"; // STS digi file
  TObjString trdDigiFile = parDir + "/trd/trd_v12b.digi.par"; // TRD digi file
  parFileList->Add(&stsDigiFile);
  parFileList->Add(&trdDigiFile);
}
  
