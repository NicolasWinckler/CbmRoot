// ------------------------------------------------------------------------
// -----                       CbmTrdPhotonAnalysis source file       -----
// -----                       Created  13/02/12  by C.Bergmann       -----
// ------------------------------------------------------------------------

#include "CbmTrdPhotonAnalysis.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"

#include "CbmTrdModule.h"
#include "CbmTrdDigiPar.h"

#include "CbmMCTrack.h"
#include "CbmTrdPoint.h"
#include "CbmTrdDigi.h"
#include "CbmTrdCluster.h"
#include "CbmTrdHit.h"
#include "CbmTrdTrack.h"
#include "CbmGlobalTrack.h"
#include "CbmVertex.h"

#include "TLegend.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TH1I.h"
#include "TH2I.h"
#include "TH3I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TProfile.h"
#include "TFile.h"
#include "TRandom.h"
#include "TMath.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMaterial.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "TPRegexp.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,20,0)
#include "CbmTrdStringToken.h"
#endif

#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <cmath>
using std::cout;
using std::endl;

CbmTrdPhotonAnalysis::CbmTrdPhotonAnalysis()
  : FairTask("TrdPhotonAnalysis",1),
    fMCTracks(NULL),
    fTrdPoints(NULL),
    fTrdDigis(NULL),
    fTrdClusters(NULL),
    fTrdHits(NULL),
    fTrdTracks(NULL),
    fGlobalTracks(NULL),
    fPrimaryVertex(NULL),
    fTrdId(),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fMotherDaughterZBirth(NULL),
    fZBirthAll(NULL),
    fMotherDaughter(NULL),
    fNoDaughters(NULL),
    fZBirthEPfromGamma(NULL),
    fMCPid(NULL),
    fGTPid(NULL),
    fePlusMinusMother(NULL),
    fgammaMother(NULL),
    fgammaDaughter(NULL),
    fZBirth(),
    fGammaHistory(NULL),
    fMCParticleMap(),
    it(),
    fElectronIds(),
    fPositronIds(),
    fGammaIds(),
    fPi0Ids()
{
}

CbmTrdPhotonAnalysis::CbmTrdPhotonAnalysis(const char *name, const char *title, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fMCTracks(NULL),
    fTrdPoints(NULL),
    fTrdDigis(NULL),
    fTrdClusters(NULL),
    fTrdHits(NULL),
    fTrdTracks(NULL),
    fGlobalTracks(NULL),
    fPrimaryVertex(NULL),
    fTrdId(),
    fDigiPar(NULL),
    fModuleInfo(NULL),
    fMotherDaughterZBirth(NULL),
    fZBirthAll(NULL),
    fMotherDaughter(NULL),
    fNoDaughters(NULL),
    fZBirthEPfromGamma(NULL),
    fMCPid(NULL),
    fGTPid(NULL),
    fePlusMinusMother(NULL),
    fgammaMother(NULL),
    fgammaDaughter(NULL),
    fZBirth(),
    fGammaHistory(NULL),
    fMCParticleMap(),
    it(),
    fElectronIds(),
    fPositronIds(),
    fGammaIds(),
    fPi0Ids()
{
}

CbmTrdPhotonAnalysis::~CbmTrdPhotonAnalysis()
{

}

InitStatus CbmTrdPhotonAnalysis::ReInit()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
      (rtdb->getContainer("CbmTrdDigiPar"));
  
  return kSUCCESS;
}

InitStatus CbmTrdPhotonAnalysis::Init()
{
  // cout << endl << endl << " BUUUUUUMMMMMMMMMMMMMMMMMM " << endl << endl;
    FairRootManager *ioman = FairRootManager::Instance();
    if ( ! ioman ) Fatal("Init", "No FairRootManager");
    
    fMCTracks = (TClonesArray*)ioman->GetObject("MCTrack");
  if ( ! fMCTracks ) {
    cout << "-W CbmTrdCluster::Init: No MCTrack array!" << endl;
    cout << "                            Task will be inactive" << endl;
    return kERROR;
  }

    fTrdPoints=(TClonesArray *)ioman->GetObject("TrdPoint");
 
    if ( ! fTrdPoints ) {
      cout << "-W CbmTrdPhotonAnalysis::Init: No TrdPoints array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fTrdDigis=(TClonesArray *)ioman->GetObject("TrdDigi");
 
    if ( ! fTrdDigis ) {
      cout << "-W CbmTrdPhotonAnalysis::Init: No TrdDigis array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR;
    }

    fTrdClusters=(TClonesArray *)ioman->GetObject("TrdCluster");
 
    if ( ! fTrdClusters ) {
      cout << "-W CbmTrdPhotonAnalysis::Init: No TrdClusters array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR;
    }

    fTrdHits=(TClonesArray *)ioman->GetObject("TrdHit");
 
    if ( ! fTrdHits ) {
      cout << "-W CbmTrdPhotonAnalysis::Init: No TrdHits array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fTrdTracks=(TClonesArray *)ioman->GetObject("TrdTrack");
 
    if ( ! fTrdTracks ) {
      cout << "-W CbmTrdPhotonAnalysis::Init: No TrdTracks array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fGlobalTracks=(TClonesArray *)ioman->GetObject("GlobalTrack");
 
    if ( ! fGlobalTracks ) {
      cout << "-W CbmTrdPhotonAnalysis::Init: No GlobalTracks array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    fPrimaryVertex=(TClonesArray *)ioman->GetObject("PrimaryVertex");
 
    if ( ! fPrimaryVertex ) {
      cout << "-W CbmTrdPhotonAnalysis::Init: No PrimaryVertex array!" << endl;
      cout << "                            Task will be inactive" << endl;
      return kERROR;
    }

    return kSUCCESS;

}

void CbmTrdPhotonAnalysis::SetParContainers()
{
    // Get Base Container
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();

    fDigiPar = (CbmTrdDigiPar*)
               (rtdb->getContainer("CbmTrdDigiPar"));

}

void CbmTrdPhotonAnalysis::Exec(Option_t * option)
{

  cout << endl << "================CbmTrdPhotonAnalysis=====================" << endl;
  InitHistos();
  
  CbmMCTrack* mctrack = NULL;
  CbmMCTrack* mctracke = NULL;
  CbmMCTrack* mctrackp = NULL;
  CbmMCTrack* mother = NULL;
  CbmMCTrack* grandmother = NULL;
  CbmMCTrack* daughter = NULL;
  CbmTrdPoint *mcpt = NULL;
  CbmTrdDigi *digi = NULL;
  CbmTrdCluster* cluster = NULL;
  CbmTrdHit* hit = NULL;
  CbmTrdTrack* track = NULL;
  CbmGlobalTrack* gtrack = NULL;
  CbmVertex* pvertex = NULL;


  Int_t nMcTracks(0),
    nTrdPoints(0),
    nTrdDigis(0),
    nTrdClusters(0),
    nTrdHits(0),
    nTrdTracks(0),
    nGlobalTracks(0),
    nPrimaryVertex(0);

  Int_t elecFromPhot = 0;
  Int_t posiFromPhot = 0;
  Int_t motherId = 0;

  std::vector<Int_t> electronMotherIds;
  std::vector<Int_t> positronMotherIds;
  std::vector<Int_t> electronGammaMotherIds;
  std::vector<Int_t> positronGammaMotherIds;
  std::vector<Int_t> electronGammaMotherIdsInMagnet;
  std::vector<Int_t> positronGammaMotherIdsInMagnet;
  /*
    std::map<Int_t, MCParticle*> MCParticleMap;
    std::map<Int_t, MCParticle*>::iterator it;
  */

  if (fMCTracks)
    nMcTracks = fMCTracks->GetEntries();
  if (fTrdPoints)
    nTrdPoints = fTrdPoints->GetEntries();
  if (fTrdDigis)
    nTrdDigis = fTrdDigis->GetEntries();
  if (fTrdClusters)
    nTrdClusters = fTrdClusters->GetEntries();
  if (fTrdHits)
    nTrdHits = fTrdHits->GetEntries();
  if (fTrdTracks)
    nTrdTracks = fTrdTracks->GetEntries();
  if (fGlobalTracks)
    nGlobalTracks = fGlobalTracks->GetEntries();
  /*
    if (fPrimaryVertex)
    nPrimaryVertex = fPrimaryVertex->GetEntries();
  */

  printf("\n MC Tracks:        %i\n MC TRD Points:    %i\n TRD Digis:        %i\n TRD Cluster:      %i\n TRD Hits:         %i\n TRD Tracks:       %i\n Global Tracks:    %i\n Primary Verticis: %i\n", nMcTracks, nTrdPoints, nTrdDigis, nTrdClusters, nTrdHits, nTrdTracks, nGlobalTracks, nPrimaryVertex);
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  // Filling maps and vectors for later usage
  for (Int_t iMcTrack = 0; iMcTrack < nMcTracks; iMcTrack++) {
    mctrack = (CbmMCTrack*) fMCTracks->At(iMcTrack);
    if (mctrack->GetPdgCode() == 11)
      fElectronIds.push_back(iMcTrack);
    if (mctrack->GetPdgCode() == -11)
      fPositronIds.push_back(iMcTrack);
    if (mctrack->GetPdgCode() == 22)
      fGammaIds.push_back(iMcTrack);
    if (mctrack->GetPdgCode() == 111)
      fPi0Ids.push_back(iMcTrack);

    if (fMCParticleMap.find(iMcTrack) == fMCParticleMap.end()) {
      MCParticle* p = new MCParticle;
      fMCParticleMap[iMcTrack] = p;
    }
    fMCParticleMap[iMcTrack]->motherId = mctrack->GetMotherId();
    fMCParticleMap[iMcTrack]->PID = mctrack->GetPdgCode();
    if (mctrack->GetMotherId() >= 0) {
      if (fMCParticleMap.find(mctrack->GetMotherId()) == fMCParticleMap.end()) {
	MCParticle* p = new MCParticle;
	fMCParticleMap[mctrack->GetMotherId()] = p;
      }
      fMCParticleMap[mctrack->GetMotherId()]->daughterIds.push_back(iMcTrack);
    }
  }
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  for (Int_t iMcTrack = 0; iMcTrack < nMcTracks; iMcTrack++) {
    mctrack = (CbmMCTrack*) fMCTracks->At(iMcTrack);
    fMCPid->Fill(PdgToGeant(mctrack->GetPdgCode()));
    if (mctrack->GetPdgCode() == -11) {
      if (mctrack->GetMotherId() >= 0)
	positronMotherIds.push_back(mctrack->GetMotherId());
    }
    if (mctrack->GetPdgCode() == 11) {
      if (mctrack->GetMotherId() >= 0)
	electronMotherIds.push_back(mctrack->GetMotherId());
    }
    if (mctrack->GetPdgCode() == 22)
      fZBirth[0]->Fill(mctrack->GetStartZ());
    if (mctrack->GetPdgCode() == -11)
      fZBirth[1]->Fill(mctrack->GetStartZ());
    if (mctrack->GetPdgCode() == 11)
      fZBirth[2]->Fill(mctrack->GetStartZ());
    if (mctrack->GetPdgCode() == 111)
      fZBirth[3]->Fill(mctrack->GetStartZ());
  
    fZBirthAll->Fill(mctrack->GetStartZ(),PdgToGeant(mctrack->GetPdgCode()));
    motherId = mctrack->GetMotherId();

    if (motherId >= 0) {    
      mother = (CbmMCTrack*)fMCTracks->At(motherId);
      fMotherDaughter->Fill(PdgToGeant(mother->GetPdgCode()),PdgToGeant(mctrack->GetPdgCode()));
      fMotherDaughterZBirth->Fill(mctrack->GetStartZ(),PdgToGeant(mother->GetPdgCode()),PdgToGeant(mctrack->GetPdgCode()));
      if (mother->GetPdgCode() == 22)
	fgammaDaughter->Fill(PdgToGeant(mctrack->GetPdgCode()));
    }
    else
      {
	fMotherDaughter->Fill(49,PdgToGeant(mctrack->GetPdgCode()));
	fMotherDaughterZBirth->Fill(mctrack->GetStartZ(),49,PdgToGeant(mctrack->GetPdgCode()));
      }
   
    if (mctrack->GetPdgCode() == 22) { 
      fGammaHistory->Fill(0);
      if (VertexInTarget(mctrack)) {
	fGammaHistory->Fill(1);
	if (motherId < 0) {
	  fGammaHistory->Fill(2);
	}
      }
      if (motherId >= 0) {    
	mother = (CbmMCTrack*)fMCTracks->At(motherId);
	fgammaMother->Fill(PdgToGeant(mother->GetPdgCode()));
	
      }
      else
	fgammaMother->Fill(49);
    }
    
    if (mctrack->GetPdgCode() == -11 || mctrack->GetPdgCode() == 11) {
      //if (motherId >= 0) { 
      mother = (CbmMCTrack*)fMCTracks->At(motherId);
      fePlusMinusMother->Fill(PdgToGeant(mother->GetPdgCode()));
      if (mother->GetPdgCode() == 22) {
	if (VertexInTarget(mother))
	  fGammaHistory->Fill(3);

	if (mctrack->GetPdgCode() == -11) {
	  posiFromPhot++;
	  if (VertexInTarget(mother)) {
	    positronGammaMotherIds.push_back(motherId);
	    fGammaHistory->Fill(5);
	    if (VertexInMagnet(mctrack)){
	      positronGammaMotherIdsInMagnet.push_back(motherId);
	      fGammaHistory->Fill(8);
	    }
	  }
	}
	if (mctrack->GetPdgCode() == 11) {
	  elecFromPhot++;
	  if (VertexInTarget(mother)) {
	    electronGammaMotherIds.push_back(motherId);
	    fGammaHistory->Fill(4);
	    if (VertexInMagnet(mctrack)){
	      electronGammaMotherIdsInMagnet.push_back(motherId);
	      fGammaHistory->Fill(7);
	    }
	  }
	}
      }
      // }
      //else
      //fePlusMinusMother->Fill(49);
    }
    
  }
  Bool_t primary = false;
  for (Int_t i = 0; i < (Int_t)positronMotherIds.size(); i++) {
    for (Int_t j = 0; j < (Int_t)electronMotherIds.size(); j++) {
      if (positronMotherIds[i] == electronMotherIds[j]) {
	mother = (CbmMCTrack*)fMCTracks->At(positronMotherIds[i]);
	if (fMCParticleMap[positronMotherIds[i]]->motherId >= 0) {
	  grandmother = (CbmMCTrack*)fMCTracks->At(fMCParticleMap[positronMotherIds[i]]->motherId);
	  primary = false;
	}
	else {
	  primary = true;
	  grandmother = NULL;
	}
	//if (fMCParticleMap[positronMotherIds[i]]->daughterIds.size() == 2) { // only e^{+} e^{-} pairs
	for (Int_t k = 0; k < fMCParticleMap[positronMotherIds[i]]->daughterIds.size(); k++) { // loop over electron and positron
	  daughter = (CbmMCTrack*)fMCTracks->At(fMCParticleMap[positronMotherIds[i]]->daughterIds[k]);
	  fPairAllVertex[0]->Fill(daughter->GetStartX());
	  fPairAllVertex[1]->Fill(daughter->GetStartY());
	  fPairAllVertex[2]->Fill(daughter->GetStartZ());	  
	  if (mother->GetPdgCode() == 22 && primary) {//photon	   
	    fPairGammaVertex[0]->Fill(daughter->GetStartX());
	    fPairGammaVertex[1]->Fill(daughter->GetStartY());
	    fPairGammaVertex[2]->Fill(daughter->GetStartZ());
	  }
	  if (!primary)
	    if (mother->GetPdgCode() == 111 || grandmother->GetPdgCode() == 111 ) {//#pi^{0}
	      fPairPi0Vertex[0]->Fill(daughter->GetStartX());
	      fPairPi0Vertex[1]->Fill(daughter->GetStartY());
	      fPairPi0Vertex[2]->Fill(daughter->GetStartZ());
	    }
	}
	//}
      }
    }
  }

  for (it=fMCParticleMap.begin(); it != fMCParticleMap.end(); it++) {
    mctrack = (CbmMCTrack*)fMCTracks->At((*it).first);
    fNoDaughters->Fill(PdgToGeant(mctrack->GetPdgCode()), (*it).second->daughterIds.size());
  }
  for (Int_t i = 0; i < (Int_t)positronGammaMotherIds.size(); i++) {
    for (Int_t j = 0; j < (Int_t)electronGammaMotherIds.size(); j++) {
      //cout << positronGammaMotherIds[i] << " " <<  electronGammaMotherIds[j] << endl;
      if (positronGammaMotherIds[i] == electronGammaMotherIds[j]) {
	fGammaHistory->Fill(6);
	
	for (Int_t k = 0; k < fMCParticleMap[positronGammaMotherIds[i]]->daughterIds.size(); k++) {
	  mctrackp = (CbmMCTrack*)fMCTracks->At(fMCParticleMap[positronGammaMotherIds[i]]->daughterIds[k]);
	  if (mctrackp->GetPdgCode() == -11)
	    fZBirthEPfromGamma->Fill(mctrackp->GetStartZ());
	}
	for (Int_t k = 0; k < fMCParticleMap[electronGammaMotherIds[i]]->daughterIds.size(); k++) {
	  mctracke = (CbmMCTrack*)fMCTracks->At(fMCParticleMap[electronGammaMotherIds[i]]->daughterIds[k]);
	  if (mctracke->GetPdgCode() == 11)
	    fZBirthEPfromGamma->Fill(mctracke->GetStartZ());
	}
	
      }
    }
  }
  for (Int_t i = 0; i < (Int_t)positronGammaMotherIdsInMagnet.size(); i++) {
    for (Int_t j = 0; j < (Int_t)electronGammaMotherIdsInMagnet.size(); j++) {
      //cout << positronGammaMotherIds[i] << " " <<  electronGammaMotherIds[j] << endl;
      if (positronGammaMotherIdsInMagnet[i] == electronGammaMotherIdsInMagnet[j]) {
	fGammaHistory->Fill(9);
      }
    }
  }


  printf("electron from photon = %i\npositron from photon = %i\n", elecFromPhot, posiFromPhot);
  for (Int_t iTrdTrack = 0; iTrdTrack < nTrdTracks; iTrdTrack++) {
    track = (CbmTrdTrack*) fTrdTracks->At(iTrdTrack);
    /*
      printf("PidWkn: %5.2f  PidANN: %5.2f  PidLikeEL: %5.2f  PidLikePI: %5.2f  PidLikeKA: %5.2f  PidLikePR: %5.2f  PidLikeMU: %5.2f\n",
      track->GetPidWkn(),
      track->GetPidANN(),
      track->GetPidLikeEL(),
      track->GetPidLikePI(),
      track->GetPidLikeKA(),
      track->GetPidLikePR(),
      track->GetPidLikeMU()
      );
    */
  }


  for (Int_t iGlobalTrack = 0; iGlobalTrack < nGlobalTracks; iGlobalTrack++) {
    gtrack = (CbmGlobalTrack*) fGlobalTracks->At(iGlobalTrack);
    fGTPid->Fill(gtrack->GetPidHypo());
    //printf("PidHypo:%.2i\n",gtrack->GetPidHypo());
  }
  SaveHistosToFile();
}
  Bool_t CbmTrdPhotonAnalysis::PairFromGamma(Int_t firstId, Int_t secondId)
  {
    return true;
  }
  Bool_t CbmTrdPhotonAnalysis::PairFromPi0(Int_t firstId, Int_t secondId)
  {
    return true;
  }
  Bool_t CbmTrdPhotonAnalysis::VertexInMagnet(CbmMCTrack* track)
  {
    if (fabs(track->GetStartX()) < 132/2 && fabs(track->GetStartY()) < 132/2 && track->GetStartZ() < 100 )
      return true;
    return false;
  }
  Bool_t CbmTrdPhotonAnalysis::VertexInTarget(CbmMCTrack* track)
  {
    if (fabs(track->GetStartX()) < 0.1 && fabs(track->GetStartY()) < 0.1 && track->GetStartZ() < 0.1)
      return true;
    return false;
  }

  Int_t CbmTrdPhotonAnalysis::PdgToGeant(Int_t PdgCode) 
  {
    if (PdgCode == 22)
      return 1;
    if (PdgCode == -11)
      return 2;
    if (PdgCode == 11)
      return 3;
    if (PdgCode == 12 || PdgCode == 14 || PdgCode == 16)
      return 4;
    if (PdgCode == -13)
      return 5;
    if (PdgCode == 13)
      return 6;
    if (PdgCode == 111)
      return 7;
    if (PdgCode == 211)
      return 8;
    if (PdgCode == -211)
      return 9;
    if (PdgCode == 130)
      return 10;
    if (PdgCode == 321)
      return 11;
    if (PdgCode == -321)
      return 12;
    if (PdgCode == 2112)
      return 13;
    if (PdgCode == 2212)
      return 14;
    if (PdgCode == -2212)
      return 15;
    if (PdgCode == 310)
      return 16;
    if (PdgCode == 221)
      return 17;
    if (PdgCode == 3122)
      return 18;
    if (PdgCode == 3222)
      return 19;
    if (PdgCode == 3212)
      return 20;
    if (PdgCode == 3112)
      return 21;
    if (PdgCode == 3322)
      return 22;
    if (PdgCode == 3312)
      return 23;
    if (PdgCode == 3332)
      return 24;
    if (PdgCode == -2112)
      return 25;
    if (PdgCode == -3122)
      return 26;
    if (PdgCode == -3112)
      return 27;
    if (PdgCode == -3212)
      return 28;
    if (PdgCode == -3322)
      return 30;
    if (PdgCode == -3312)
      return 31;
    if (PdgCode == -3332)
      return 32;
    if (PdgCode == -15)
      return 33;
    if (PdgCode == 15)
      return 34;
    if (PdgCode == 411)
      return 35;
    if (PdgCode == -411)
      return 36;
    if (PdgCode == 421)
      return 37;
    if (PdgCode == -412)
      return 38;
    if (PdgCode == 431)
      return 39;
    if (PdgCode == -431)
      return 40;
    if (PdgCode == 4122)
      return 41;
    if (PdgCode == 24)
      return 42;
    if (PdgCode == -24)
      return 43;  
    if (PdgCode == 23)
      return 44;
    if (PdgCode == 50000050)
      return 45;
    if (PdgCode == 1000010020)
      return 46;
    if (PdgCode == 1000010030)
      return 47;
    if (PdgCode == 1000020040)
      return 48;
    cout << PdgCode << endl;
    return 49;
  }

  void CbmTrdPhotonAnalysis::InitHistos()
  {
    TString particleID[49] = {
      "#gamma",
      "e^{+}",
      "e^{-}",
      "#nu_{(e,#mu,#tau)}",
      "#mu^{+}",
      "#mu^{-}",
      "#pi^{0}",
      "#pi^{+}",
      "#pi^{-}",
      "K^{0}_{L}",
      "K^{+}",
      "K^{-}",
      "n",
      "p",
      "#bar{p}",
      "K^{0}_{S}",
      "#eta",
      "#Lambda",
      "#Sigma^{+}",
      "#Sigma^{0}",
      "#Sigma^{-}",
      "#Xi^{0}",
      "#Xi^{-}",
      "#Omega^{-}",
      "#bar{n}",
      "#bar{#Lambda}",
      "#bar{#Sigma}^{-}",
      "#bar{#Sigma}^{0}",
      "#bar{#Sigma}^{+}",
      "#bar{#Xi}^{0}",
      "#bar{#Xi}^{+}",
      "#bar{#Omega}^{+}",
      "#tau^{+}",
      "#tau^{-}",
      "D^{+}",
      "D^{-}",
      "D^{0}",
      "#bar{D}^{+}",
      "D_{S}^{+}",
      "#bar{D_{S}}^{-}",
      "#Lambda_{C}^{+}",
      "W^{+}",
      "W^{-}",
      "Z^{0}",
      "d",
      "t",
      "He",
      "#gamma_{RICH}",
      "#gamma_{primary}"
    };
    fMCPid = new TH1I("MCPid","MC Pid",49,0.5,49.5);
    fGTPid = new TH1I("GTPid","GT Pid",49,0.5,49.5);
    fePlusMinusMother = new TH1I("ePlusMinusMother","Mother ID for e^{+}/e^{-}",49,0.5,49.5);
    fgammaMother = new TH1I("gammaMother","Mother ID for #gamma",49,0.5,49.5);
    fgammaDaughter = new TH1I("gammaDaughter","Daughter ID for #gamma",49,0.5,49.5);
    fZBirthAll = new TH2I("ZBirth_All","z birth",10000,0,1000,49,0.5,49.5);
    fZBirthEPfromGamma = new TH1I("ZBirthEPfromGamma","z birth from e^{+} & e^{-} pairs with mother #gamma",10000,0,1000);
    fNoDaughters = new TH2I("NoDaughters","Number of daughters",49,0.5,49.5,101,-0.5,100.5);
    fMotherDaughter = new TH2I("MotherDaughter","mother / daughter",49,0.5,49.5,49,0.5,49.5);
    fMotherDaughterZBirth = new TH3I("MotherDaughterZBirth","mother / daughter / z-birth",10000,0,1000,49,0.5,49.5,49,0.5,49.5);
    for (Int_t bin = 0; bin < 49; bin++) {
      fMCPid->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
      fGTPid->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
      fePlusMinusMother->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
      fgammaMother->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
      fgammaDaughter->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
      fZBirthAll->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
      fMotherDaughter->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
      fMotherDaughter->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
      fMotherDaughterZBirth->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
      fMotherDaughterZBirth->GetZaxis()->SetBinLabel(bin+1,particleID[bin]);
      fNoDaughters->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    }
    TString name[3] = {"x","y","z"};
    Int_t low[3] = {-50,-50,0};
    Int_t high[3] = {50,50,100};
    for (Int_t i = 0; i < 3; i++) {
      fPairAllVertex[i] = new TH1I("PairAllVertex_" + name[i], "all e^{+} e^{-} pair vertex" + name[i], 200, low[i], high[i]);
      fPairAllVertex[i]->SetXTitle(name[i]+"-position [cm]");
      fPairGammaVertex[i] = new TH1I("PairGammaVertex_" + name[i], "e^{+} e^{-} pair from #gamma vertex" + name[i], 200, low[i], high[i]);
      fPairGammaVertex[i]->SetXTitle(name[i]+"-position [cm]");
      fPairPi0Vertex[i] = new TH1I("PairPi0Vertex_" + name[i], "e^{+} e^{-} pair from #pi^{0} vertex" + name[i], 200, low[i], high[i]);
      fPairPi0Vertex[i]->SetXTitle(name[i]+"-position [cm]");
    }
    /*
      fMCPid = new TH1I("MCPid","MC Pid",4501,-1000.5,3500.5);
      fGTPid = new TH1I("GTPid","GT Pid",4501,-1000.5,3500.5);
      fePlusMinusMother = new TH1I("fePlusMinusMother","Mother ID for e^{+}/e^{-}",4501,-1000.5,3500.5);
    */
    fZBirth[0] = new TH1I("ZBirth_gamma","z birth #gamma",10000,0,1000);
    fZBirth[1] = new TH1I("ZBirth_positron","z birth e^{+}",10000,0,1000);
    fZBirth[2] = new TH1I("ZBirth_electron","z birth e^{-}",10000,0,1000);
    fZBirth[3] = new TH1I("ZBirth_pi0","z birth #pi^{0}",10000,0,1000);
    fGammaHistory = new TH1I("GammaHistory","history for #gamma",10,-0.5,9.5);
    fGammaHistory->GetXaxis()->SetBinLabel( 1,"all #gamma");
    fGammaHistory->GetXaxis()->SetBinLabel( 2,"#gamma from z#leq0.1cm");
    fGammaHistory->GetXaxis()->SetBinLabel( 3,"#gamma with mother id#equiv-1");
    fGammaHistory->GetXaxis()->SetBinLabel( 4,"producing e^{-} or e^{+}");
    fGammaHistory->GetXaxis()->SetBinLabel( 5,"producing e^{-}");
    fGammaHistory->GetXaxis()->SetBinLabel( 6,"producing e^{+}");
    fGammaHistory->GetXaxis()->SetBinLabel( 7,"producing e^{-} & e^{+}");
    fGammaHistory->GetXaxis()->SetBinLabel( 8,"producing e^{-} within magnet");
    fGammaHistory->GetXaxis()->SetBinLabel( 9,"producing e^{+} within magnet");
    fGammaHistory->GetXaxis()->SetBinLabel(10,"producing e^{-} & e^{+} within magnet");
    NiceHisto1(fMCPid,1,20,1);
    NiceHisto1(fGTPid,1,20,1);
    NiceHisto1(fePlusMinusMother,1,20,1);
    NiceHisto1(fgammaMother,1,20,1);
    NiceHisto1(fgammaDaughter,1,20,1);
    NiceHisto1(fZBirth[0],1,20,1);
    NiceHisto1(fZBirth[1],1,20,1);
    NiceHisto1(fZBirth[2],1,20,1);
    NiceHisto1(fZBirth[3],1,20,1);
    NiceHisto1(fGammaHistory,1,20,1);
    NiceHisto2(fZBirthAll,1,1,1);
    NiceHisto2(fMotherDaughter,1,1,1);
  }

  void CbmTrdPhotonAnalysis::SaveHistosToFile()
  {

    TCanvas *c = new TCanvas("CbmTrdPhotonAnalysis","CbmTrdPhotonAnalysis",800,600);
    c->Divide(2,1);
    c->cd(1);
    fMCPid->Draw();
    c->cd(2);
    fGTPid->Draw();
    c->SaveAs("data/CbmTrdPhotonAnalysis.pdf");
    TFile *outFile = new TFile("data/CbmTrdPhotonAnalysis.root","RECREATE","output of CbmTrdPhotonAnalysis");
    outFile->cd();
    fMCPid->Write("", TObject::kOverwrite);
    fGTPid->Write("", TObject::kOverwrite);
    fePlusMinusMother->Write("", TObject::kOverwrite);
    fgammaMother->Write("", TObject::kOverwrite);
    fgammaDaughter->Write("", TObject::kOverwrite);
    fGammaHistory->Write("", TObject::kOverwrite);
    for (Int_t i = 0; i < 4; i++) {
      fZBirth[i]->SetXTitle("z-position [cm]");
      fZBirth[i]->Write("", TObject::kOverwrite);
    }
    fZBirthAll->SetXTitle("z-position [cm]");
    fZBirthAll->Write("", TObject::kOverwrite);
    fMotherDaughter->SetXTitle("Mother");
    fMotherDaughter->SetYTitle("Daughter");
    fMotherDaughter->Write("", TObject::kOverwrite);
    fMotherDaughterZBirth->SetXTitle("z-position [cm]");
    fMotherDaughter->SetYTitle("Mother");
    fMotherDaughter->SetZTitle("Daughter");
    fMotherDaughterZBirth->Write("", TObject::kOverwrite);
    fZBirthEPfromGamma->Write("", TObject::kOverwrite);
    fNoDaughters->SetYTitle("Number of daughters");
    fNoDaughters->Write("", TObject::kOverwrite);
    for (Int_t i = 0; i < 3; i++) {
      fPairAllVertex[i]->Write("", TObject::kOverwrite);
      fPairGammaVertex[i]->Write("", TObject::kOverwrite);
      fPairPi0Vertex[i]->Write("", TObject::kOverwrite);
    }
    //outFile->Close();
    c->Close();
  }

  void CbmTrdPhotonAnalysis::FinishEvent()
  {

  }

  void CbmTrdPhotonAnalysis::FinishTask()
  {

  }

  void CbmTrdPhotonAnalysis::Register()
  {

  }


  void CbmTrdPhotonAnalysis::NiceHisto1(TH1 *h, Int_t color, Int_t mStyle, Int_t mSize) 
  {
    h->SetMarkerStyle(mStyle);
    h->SetMarkerSize(mSize);  
    h->SetMarkerColor(color);
    h->SetLineColor(color);  
    h->GetXaxis()->SetLabelSize(0.03);
    h->GetYaxis()->SetLabelSize(0.03);
    //h->GetZaxis()->SetLabelSize(0.03);
    h->GetXaxis()->SetTitleSize(0.035);
    h->GetXaxis()->SetTitleOffset(1.25);
    h->GetYaxis()->SetTitleSize(0.035);
    h->GetYaxis()->SetTitleOffset(1.25);
    //h->GetZaxis()->SetTitleSize(0.035);
    //h->GetZaxis()->SetTitleOffset(-2);
  }
  void CbmTrdPhotonAnalysis::NiceHisto2(TH2 *h, Int_t color, Int_t mStyle, Int_t mSize) 
  {
    h->SetMarkerStyle(mStyle);
    h->SetMarkerSize(mSize);  
    h->SetMarkerColor(color);
    h->SetLineColor(color);  
    h->GetXaxis()->SetLabelSize(0.03);
    h->GetYaxis()->SetLabelSize(0.03);
    h->GetZaxis()->SetLabelSize(0.03);
    h->GetXaxis()->SetTitleSize(0.035);
    h->GetXaxis()->SetTitleOffset(1.25);
    h->GetYaxis()->SetTitleSize(0.035);
    h->GetYaxis()->SetTitleOffset(1.25);
    h->GetZaxis()->SetTitleSize(0.035);
    h->GetZaxis()->SetTitleOffset(-2);
  }
  void CbmTrdPhotonAnalysis::NiceProfile(TProfile *h, Int_t color, Int_t mStyle, Int_t mSize) 
  {
    h->SetMarkerStyle(mStyle);
    h->SetMarkerSize(mSize);  
    h->SetMarkerColor(color);
    h->SetLineColor(color);  
    h->GetXaxis()->SetLabelSize(0.03);
    h->GetYaxis()->SetLabelSize(0.03);
    //h->GetZaxis()->SetLabelSize(0.03);
    h->GetXaxis()->SetTitleSize(0.035);
    h->GetXaxis()->SetTitleOffset(1.25);
    h->GetYaxis()->SetTitleSize(0.035);
    h->GetYaxis()->SetTitleOffset(1.25);
    //h->GetZaxis()->SetTitleSize(0.035);
    //h->GetZaxis()->SetTitleOffset(-2);
  }

  void CbmTrdPhotonAnalysis::NiceLegend(TLegend *l)
  {
    l->SetLineColor(0);
    l->SetLineStyle(0);
    l->SetFillStyle(0);
    l->SetTextSize(0.03);
  }
