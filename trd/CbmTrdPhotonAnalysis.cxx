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

#include "TStopwatch.h"
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
#include "TParticle.h"
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
#include <utility>
#include <map>
#include <math.h>
#include <iostream>
#include <iomanip>
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
    // Birth Vertex
    fMotherDaughterZBirth(NULL),
    fBirthGamma(NULL),
    fBirthPi0(NULL),
    fBirthPair(NULL),
    fZBirthAll(NULL),
    fZBirthEPfromGamma(NULL),
    fZBirth(),
    fPairAllVertex(),
    fPairGammaVertex(),
    fPairPi0Vertex(),
    fEPPairVertexDistance(NULL),
    // Death Vertex
    fpi02GammaVertex(),
    fgamma2PairVertex(),
    // Mother Daughter
    fMotherDaughter(NULL),
    fNoDaughters(NULL),
    fePlusMinusMother(NULL),
    fePlusAndMinusMother(NULL),
    fgammaMother(NULL),
    fgammaAndGammaMother(NULL),
    fDalizMother(NULL),
    fgammaDaughter(NULL),
    fInvMPairMother(NULL),
    fPtPairMother(NULL),
    fPPairMother(NULL),
    fOpenAnglePairMother(NULL),
    // Global PID
    fMCPid(NULL),
    fGTPid(NULL),
    fPairHistory(NULL),
    // Spectra
    //TH1F* fInvMSpectra[20];
    //TH1F* fEPPairOpenAngle[20];
    fInvMassSpectrumGammaTruePairs(NULL),
    fInvMassSpectrumGammaAllPairs(NULL),
    fInvMassSpectrumGammaEPPairs(NULL),
    //TH1I* fInvMassSpectrumGammaEPPairsSameMother;
    fInvMassSpectrumGammaEPPairsInTarget(NULL),
    fInvMassSpectrumGammaEPPairsInMagnet(NULL),
    fInvMassSpectrumGammaEPPairsOpenAngle(NULL),
    fInvMassSpectrumGammaEPPairsGamma(NULL),
    fInvMassSpectrumGammaEPPairsPi0(NULL),
    fInvMassSpectrumTrueEPPairs(NULL),
    fInvMassSpectrumAllEPPairs(NULL),
    fInvMassSpectrumEPPairsInTarget(NULL),
    fInvMassSpectrumEPPairsInMagnet(NULL),
    // Cuts
    fEPPairOpeningAngle(NULL),
    fEPPairOpeningAngleGamma(NULL),
    fEPPairOpeningAnglePi0(NULL),
    fEPPairOpeningAngleSameMother(NULL),
    fEPPairOpeningAngleInTarget(NULL),
    fEPPairOpeningAngleInMagnet(NULL),
    fPairOpeningAngleAll(NULL),
    fPairOpeningAngleGamma(NULL),
    fPairOpeningAnglePi0(NULL),
    fPairOpeningAngleGammaWoPi0(NULL)

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
    // Birth Vertex
    fMotherDaughterZBirth(NULL),
    fBirthGamma(NULL),
    fBirthPi0(NULL),
    fBirthPair(NULL),
    fZBirthAll(NULL),
    fZBirthEPfromGamma(NULL),
    fZBirth(),
    fPairAllVertex(),
    fPairGammaVertex(),
    fPairPi0Vertex(),
    fEPPairVertexDistance(NULL),
    // Death Vertex
    fpi02GammaVertex(),
    fgamma2PairVertex(),
    // Mother Daughter
    fMotherDaughter(NULL),
    fNoDaughters(NULL),
    fePlusMinusMother(NULL),
    fePlusAndMinusMother(NULL),
    fgammaMother(NULL),
    fgammaAndGammaMother(NULL),
    fDalizMother(NULL),
    fgammaDaughter(NULL),
    fInvMPairMother(NULL),
    fPtPairMother(NULL),
    fPPairMother(NULL),
    fOpenAnglePairMother(NULL),
    // Global PID
    fMCPid(NULL),
    fGTPid(NULL),
    fPairHistory(NULL),
    // Spectra
    //TH1F* fInvMSpectra[20];
    //TH1F* fEPPairOpenAngle[20];
    fInvMassSpectrumGammaTruePairs(NULL),
    fInvMassSpectrumGammaAllPairs(NULL),
    fInvMassSpectrumGammaEPPairs(NULL),
    //TH1I* fInvMassSpectrumGammaEPPairsSameMother;
    fInvMassSpectrumGammaEPPairsInTarget(NULL),
    fInvMassSpectrumGammaEPPairsInMagnet(NULL),
    fInvMassSpectrumGammaEPPairsOpenAngle(NULL),
    fInvMassSpectrumGammaEPPairsGamma(NULL),
    fInvMassSpectrumGammaEPPairsPi0(NULL),
    fInvMassSpectrumTrueEPPairs(NULL),
    fInvMassSpectrumAllEPPairs(NULL),
    fInvMassSpectrumEPPairsInTarget(NULL),
    fInvMassSpectrumEPPairsInMagnet(NULL),
    // Cuts
    fEPPairOpeningAngle(NULL),
    fEPPairOpeningAngleGamma(NULL),
    fEPPairOpeningAnglePi0(NULL),
    fEPPairOpeningAngleSameMother(NULL),
    fEPPairOpeningAngleInTarget(NULL),
    fEPPairOpeningAngleInMagnet(NULL),
    fPairOpeningAngleAll(NULL),
    fPairOpeningAngleGamma(NULL),
    fPairOpeningAnglePi0(NULL),
    fPairOpeningAngleGammaWoPi0(NULL)
{
}

CbmTrdPhotonAnalysis::~CbmTrdPhotonAnalysis()
{
  //TODO::
  // Clear TClonesArrays
  if(fMCTracks){
    fMCTracks->Clear("C");
    fMCTracks->Delete();
    delete fMCTracks;
  }
  if(fTrdPoints){
    fTrdPoints->Clear("C");
    fTrdPoints->Delete();
    delete fTrdPoints;
  }
  if(fTrdDigis){
    fTrdDigis->Clear("C");
    fTrdDigis->Delete();
    delete fTrdDigis;
  }
  if(fTrdClusters){
    fTrdClusters->Clear("C");
    fTrdClusters->Delete();
    delete fTrdClusters;
  }
  if(fTrdHits){
    fTrdHits->Clear("C");
    fTrdHits->Delete();
    delete fTrdHits;
  }
  if(fTrdTracks){
    fTrdTracks->Clear("C");
    fTrdTracks->Delete();
    delete fTrdTracks;
  }
  if(fGlobalTracks){
    fGlobalTracks->Clear("C");
    fGlobalTracks->Delete();
    delete fGlobalTracks;
  }
  if(fPrimaryVertex){
    fPrimaryVertex->Clear("C");
    fPrimaryVertex->Delete();
    delete fPrimaryVertex;
  }
  // Delete global pointer
  if(fDigiPar)
    delete fDigiPar;
  if(fModuleInfo)
    delete fModuleInfo;

  delete fMotherDaughterZBirth;
  delete fBirthGamma;
  delete fBirthPi0;
  delete fBirthPair;
  delete fZBirthAll;
  delete fZBirthEPfromGamma;
  delete fEPPairVertexDistance;
  for (Int_t i = 0; i < 4; i++)
    delete fZBirth[i];
  for (Int_t i = 0; i < 3; i++){
    delete fPairAllVertex[i];
    delete fPairGammaVertex[i];
    delete fPairPi0Vertex[i];
  
    // Death Vertex
    delete fpi02GammaVertex[i];
    delete fgamma2PairVertex[i];
  }
  // Mother Daughter
  delete fMotherDaughter;
  delete fNoDaughters;
  delete fePlusMinusMother;
  delete fePlusAndMinusMother;
  delete fgammaMother;
  delete fgammaAndGammaMother;
  delete fDalizMother;
  delete fgammaDaughter;
  delete fInvMPairMother;
  delete fPtPairMother;
  delete fPPairMother;
  delete fOpenAnglePairMother;
  // Global PID
  delete fMCPid;
  delete fGTPid;
  delete fPairHistory;
  // Spectra
  //TH1F* fInvMSpectra[20];
  //TH1F* fEPPairOpenAngle[20];
  delete fInvMassSpectrumGammaTruePairs;
  delete fInvMassSpectrumGammaAllPairs;
  delete fInvMassSpectrumGammaEPPairs;
  //TH1I* fInvMassSpectrumGammaEPPairsSameMother;
  delete fInvMassSpectrumGammaEPPairsInTarget;
  delete fInvMassSpectrumGammaEPPairsInMagnet;
  delete fInvMassSpectrumGammaEPPairsOpenAngle;
  delete fInvMassSpectrumGammaEPPairsGamma;
  delete fInvMassSpectrumGammaEPPairsPi0;
  delete fInvMassSpectrumTrueEPPairs;
  delete fInvMassSpectrumAllEPPairs;
  delete fInvMassSpectrumEPPairsInTarget;
  delete fInvMassSpectrumEPPairsInMagnet;
  // Cuts
  delete fEPPairOpeningAngle;
  delete fEPPairOpeningAngleGamma;
  delete fEPPairOpeningAnglePi0;
  delete fEPPairOpeningAngleSameMother;
  delete fEPPairOpeningAngleInTarget;
  delete fEPPairOpeningAngleInMagnet;

  delete fPairOpeningAngleAll;
  delete fPairOpeningAngleGamma;
  delete fPairOpeningAnglePi0;
  delete fPairOpeningAngleGammaWoPi0;


}

InitStatus CbmTrdPhotonAnalysis::ReInit()
{
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();

  fDigiPar = (CbmTrdDigiPar*)
    (rtdb->getContainer("CbmTrdDigiPar"));

  InitHistos();

  return kSUCCESS;
}

InitStatus CbmTrdPhotonAnalysis::Init()
{
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
    //return kERROR;
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
    //return kERROR;
  }

  fTrdTracks=(TClonesArray *)ioman->GetObject("TrdTrack");
 
  if ( ! fTrdTracks ) {
    cout << "-W CbmTrdPhotonAnalysis::Init: No TrdTracks array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
  }

  fGlobalTracks=(TClonesArray *)ioman->GetObject("GlobalTrack");
 
  if ( ! fGlobalTracks ) {
    cout << "-W CbmTrdPhotonAnalysis::Init: No GlobalTracks array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
  }

  fPrimaryVertex=(TClonesArray *)ioman->GetObject("PrimaryVertex");
 
  if ( ! fPrimaryVertex ) {
    cout << "-W CbmTrdPhotonAnalysis::Init: No PrimaryVertex array!" << endl;
    cout << "                            Task will be inactive" << endl;
    //return kERROR;
  }
  
  InitHistos();
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
  TStopwatch timer;
  timer.Start();
  cout << endl << "================CbmTrdPhotonAnalysis=====================" << endl;
  //InitHistos();
  
  CbmMCTrack* mctrack = NULL;
  CbmMCTrack* mother = NULL;
  CbmMCTrack* daughter = NULL;
  CbmMCTrack* etrack = NULL;
  CbmMCTrack* ptrack = NULL;
  CbmMCTrack* gtrack1 = NULL;
  CbmMCTrack* gtrack2 = NULL;


  CbmTrdTrack* track = NULL;
  CbmGlobalTrack* gtrack = NULL;

  //CbmMCTrack* grandmother = NULL;
  /*
    CbmTrdPoint *mcpt = NULL;
    CbmTrdDigi *digi = NULL;
    CbmTrdCluster* cluster = NULL;
    CbmTrdHit* hit = NULL;
 
    CbmVertex* pvertex = NULL;
  */

  Int_t nMcTracks(0),
    nTrdPoints(0),
    nTrdDigis(0),
    nTrdClusters(0),
    nTrdHits(0),
    nTrdTracks(0),
    nGlobalTracks(0),
    nPrimaryVertex(0);

  Int_t motherId = 0;

  std::map<Int_t, MCParticle*> fMCParticleMap;
  std::map<Int_t, MCParticle*>::iterator it;

  std::vector<CbmMCTrack*> fGammaFromPairs;

  std::vector<Int_t> fElectronIds;
  std::vector<Int_t> fPositronIds;
  std::vector<Int_t> fGammaIds;
  std::vector<Int_t> fPi0Ids;

  std::vector<std::vector<Int_t> > dalizTriplets;
  std::vector<std::pair<Int_t,Int_t> > electronPositronPairs;
  std::vector<std::pair<Int_t,Int_t> > gammaGammaPairs;
 
  std::vector<CbmMCTrack *> gammaFromEPPairs;
  std::vector<CbmMCTrack *> gammaFromEPPairsInMagnet;
  std::vector<CbmMCTrack *> gammaFromEPPairsInTarget;
  std::vector<CbmMCTrack *> gammaFromEPPairsOpeningAngle;
  std::vector<CbmMCTrack *> gammaFromTrueEPPairs;

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
  //Overview histograms and filling vectors for all electrons, positrons, gamma, and pi0
  for (Int_t iMcTrack = 0; iMcTrack < nMcTracks; iMcTrack++) {
    mctrack = (CbmMCTrack*) fMCTracks->At(iMcTrack);
    motherId = mctrack->GetMotherId();
    if (motherId != -1)
      mother = (CbmMCTrack*)fMCTracks->At(motherId);
   
    fMCPid->Fill(PdgToGeant(mctrack->GetPdgCode()));
    fPt->Fill(mctrack->GetPt(), PdgToGeant(mctrack->GetPdgCode()));
    fP ->Fill(mctrack->GetP(),  PdgToGeant(mctrack->GetPdgCode()));
    fZBirthAll->Fill(mctrack->GetStartZ(),PdgToGeant(mctrack->GetPdgCode()));

    if (mctrack->GetPdgCode() ==  22) { // Gamma
      fZBirth[0]->Fill(mctrack->GetStartZ());
      fBirthGamma->Fill(mctrack->GetStartX(), mctrack->GetStartY(), mctrack->GetStartZ());
      if (motherId != -1)
	fgammaMother->Fill(PdgToGeant(mother->GetPdgCode()));
      else
	fgammaMother->Fill(49);
      fGammaIds.push_back(iMcTrack);
    }
    if (mctrack->GetPdgCode() == -11) { // Positron
      fZBirth[1]->Fill(mctrack->GetStartZ());
      fPositronIds.push_back(iMcTrack);
    }
    if (mctrack->GetPdgCode() ==  11) { // Electron
      fZBirth[2]->Fill(mctrack->GetStartZ());
      fElectronIds.push_back(iMcTrack);
    }
    if (mctrack->GetPdgCode() == 111) { //Pi^0
      fZBirth[3]->Fill(mctrack->GetStartZ());
      fBirthPi0->Fill(mctrack->GetStartX(), mctrack->GetStartY(), mctrack->GetStartZ());
      fPi0Ids.push_back(iMcTrack);
    }

    
    if (motherId >= 0) {    
      mother = (CbmMCTrack*)fMCTracks->At(motherId);
      fMotherDaughter->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(mctrack->GetPdgCode()));
      fMotherDaughterZBirth->Fill(mctrack->GetStartZ(), PdgToGeant(mother->GetPdgCode()), PdgToGeant(mctrack->GetPdgCode()));
      if (mother->GetPdgCode() == 22)
	fgammaDaughter->Fill(PdgToGeant(mctrack->GetPdgCode()));
      if (mctrack->GetPdgCode() ==  11 || mctrack->GetPdgCode() == -11) 
	fePlusMinusMother->Fill(PdgToGeant(mother->GetPdgCode()));
			
    }
    else {
      fMotherDaughter->Fill(49,PdgToGeant(mctrack->GetPdgCode()));
      fMotherDaughterZBirth->Fill(mctrack->GetStartZ(),49,PdgToGeant(mctrack->GetPdgCode()));
    }

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
  //=========================================================================
  // Find Particle of Interest Candidates

  for (Int_t ipi0 = 0; ipi0 < fPi0Ids.size(); ipi0++){
    if(fMCParticleMap[fPi0Ids[ipi0]]->daughterIds.size() > 1) {
      for (Int_t idaughter = 0; idaughter < fMCParticleMap[fPi0Ids[ipi0]]->daughterIds.size(); idaughter++){
	daughter = (CbmMCTrack*) fMCTracks->At(fMCParticleMap[fPi0Ids[ipi0]]->daughterIds[idaughter]);
	if (VertexInMagnet(daughter)) {
	  fpi02GammaVertex[0]->Fill(daughter->GetStartX());
	  fpi02GammaVertex[1]->Fill(daughter->GetStartY());
	  fpi02GammaVertex[2]->Fill(daughter->GetStartZ());
	}
      }
    }
  }
  for (Int_t igamma = 0; igamma < fGammaIds.size(); igamma++) {
    if (fMCParticleMap[fGammaIds[igamma]]->daughterIds.size() > 1) {
      for (Int_t idaughter = 0; idaughter < fMCParticleMap[fGammaIds[igamma]]->daughterIds.size(); idaughter++){
	daughter = (CbmMCTrack*) fMCTracks->At(fMCParticleMap[fGammaIds[igamma]]->daughterIds[idaughter]);
	if (VertexInMagnet(daughter)) {
	  fgamma2PairVertex[0]->Fill(daughter->GetStartX());
	  fgamma2PairVertex[1]->Fill(daughter->GetStartY());
	  fgamma2PairVertex[2]->Fill(daughter->GetStartZ());
	}
      }
    }
  }

  // Find Electron Positron Pairs
  for (Int_t iElectron = 0; iElectron < fElectronIds.size(); iElectron++) {
    for (Int_t iPositron = 0; iPositron < fPositronIds.size(); iPositron++) {
      etrack = (CbmMCTrack*)fMCTracks->At(fElectronIds[iElectron]);
      ptrack = (CbmMCTrack*)fMCTracks->At(fPositronIds[iPositron]);
 

      Double_t vertexDistance = CalcVertexDistance(etrack, ptrack);
      fEPPairVertexDistance->Fill(vertexDistance);
      if (CloseByVertex(etrack, ptrack, 0.1)) {
	std::pair<Int_t,Int_t> epPair (fElectronIds[iElectron], fPositronIds[iPositron]);
	electronPositronPairs.push_back(epPair);
	CbmMCTrack *gamma = new CbmMCTrack(22, -1, 
					   etrack->GetPx() + ptrack->GetPx(),
					   etrack->GetPy() + ptrack->GetPy(),
					   etrack->GetPz() + ptrack->GetPz(),
					   0,
					   0,
					   0,
					   0,
					   0);
	gammaFromEPPairs.push_back(gamma);
	//cout << iElectron << " " << iPositron << endl;
	Double_t openingAngle = CalcOpeningAngle(etrack, ptrack);
	Double_t invariantMass = CalcInvariantMass(etrack, ptrack);
	Double_t pt = CalcPt(etrack, ptrack);
	fEPPairOpeningAngle->Fill(openingAngle);
	fInvMassSpectrumAllEPPairs->Fill(invariantMass);
	fInvMPairMother->Fill(invariantMass, PdgToGeant(fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID));
	fPtPairMother->Fill(pt, PdgToGeant(fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID));
	fPPairMother->Fill(TMath::Sqrt(pow(etrack->GetPx() + ptrack->GetPx(),2) + pow(etrack->GetPy() + ptrack->GetPy(),2) + pow(etrack->GetPz() + ptrack->GetPz(),2)), PdgToGeant(fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID));
	fOpenAnglePairMother->Fill(openingAngle, PdgToGeant(fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID));

	fInvMPairMother->Fill(invariantMass, PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));
	fPtPairMother->Fill(pt, PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));
	fOpenAnglePairMother->Fill(openingAngle, PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));

	if (VertexInMagnet(etrack) && VertexInMagnet(ptrack)) {
	  gammaFromEPPairsInMagnet.push_back(gamma);
	  fEPPairOpeningAngleInMagnet->Fill(openingAngle);
	  fInvMassSpectrumEPPairsInMagnet->Fill(invariantMass);
	  if(VertexInTarget(etrack) && VertexInTarget(ptrack)) {
	    gammaFromEPPairsInTarget.push_back(gamma);
	    fEPPairOpeningAngleInTarget->Fill(openingAngle);
	    fInvMassSpectrumEPPairsInTarget->Fill(invariantMass);	

	    if (fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID == 22  && fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID == 22) {
	      fEPPairOpeningAngleGamma->Fill(openingAngle);
	      fInvMassSpectrumGammaEPPairsGamma->Fill(invariantMass);
	    }
	    if (fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID == 111 && fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID == 111) {
	      fEPPairOpeningAnglePi0->Fill(openingAngle);
	      fInvMassSpectrumGammaEPPairsPi0->Fill(invariantMass);
	    }
	    
	    if (openingAngle < 1.0){
	      gammaFromEPPairsOpeningAngle.push_back(gamma);
	      
	      if (fMCParticleMap[fElectronIds[iElectron]]->motherId == fMCParticleMap[fPositronIds[iPositron]]->motherId) {
		gammaFromTrueEPPairs.push_back(gamma);
		fEPPairOpeningAngleSameMother->Fill(openingAngle);
		fInvMassSpectrumTrueEPPairs->Fill(invariantMass);
		
		//epPair = make_pair (fElectronIds[iElectron], fPositronIds[iPositron]);
		
		fePlusAndMinusMother->Fill(PdgToGeant(fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID));
		
	      }
	      else
		cout << fMCParticleMap[fElectronIds[iElectron]]->motherId << " " << fMCParticleMap[fPositronIds[iPositron]]->motherId << endl;
	    }
	  }
	}
      }
    }
  }
  printf (
	  "\n--- Found\n\
---      %7i Gammas\n\
---      %7i Electrons\n\
---      %7i Positrons\n\
---      %7i Electron & Positron Pairs (after vertex distance cut)\n\
---      %7i Electron & Positron Pairs (in magnet volume)\n\
---      %7i Electron & Positron Pairs (in target volume)\n\
---      %7i Electron & Positron Pairs (opening angle cut)\n\
---      %7i Electron & Positron Pairs (same mother id)\n\
---      %7i Pi0\n\n", 
	  (Int_t)fGammaIds.size(), 
	  (Int_t)fElectronIds.size(), 
	  (Int_t)fPositronIds.size(), 
	  (Int_t)electronPositronPairs.size(),
	  (Int_t)gammaFromEPPairsInMagnet.size(),
	  (Int_t)gammaFromEPPairsInTarget.size(),
	  (Int_t)gammaFromEPPairsOpeningAngle.size(),
	  (Int_t)gammaFromTrueEPPairs.size(),
	  (Int_t)fPi0Ids.size());
  cout << "fInvMassSpectrumGammaEPPairsOpenAngle: " << gammaFromEPPairsOpeningAngle.size() << endl;
  for (Int_t iGamma = 0; iGamma < gammaFromEPPairsOpeningAngle.size(); iGamma++) {
    for (Int_t jGamma = iGamma + 1; jGamma < gammaFromEPPairsOpeningAngle.size(); jGamma++) {
      //cout << iGamma << " " << jGamma << endl;
      fInvMassSpectrumGammaEPPairsOpenAngle->Fill(CalcInvariantMass(gammaFromEPPairsOpeningAngle[iGamma], gammaFromEPPairsOpeningAngle[jGamma]));
    }
  }
  cout << "fInvMassSpectrumGammaEPPairsInTarget: " << gammaFromEPPairsInTarget.size() << endl;
  for (Int_t iGamma = 0; iGamma < gammaFromEPPairsInTarget.size(); iGamma++) {
    for (Int_t jGamma = iGamma + 1; jGamma < gammaFromEPPairsInTarget.size(); jGamma++) {
      //cout << iGamma << " " << jGamma << endl;
      fInvMassSpectrumGammaEPPairsInTarget->Fill(CalcInvariantMass(gammaFromEPPairsInTarget[iGamma], gammaFromEPPairsInTarget[jGamma]));
    }
  }
  cout << "fgammaFromEPPairsInMagnet: " << gammaFromEPPairsInMagnet.size() << endl;
  for (Int_t iGamma = 0; iGamma < gammaFromEPPairsInMagnet.size(); iGamma++) {
    for (Int_t jGamma = iGamma + 1; jGamma < gammaFromEPPairsInMagnet.size(); jGamma++) {
      //cout << iGamma << " " << jGamma << endl;
      fInvMassSpectrumGammaEPPairsInMagnet->Fill(CalcInvariantMass(gammaFromEPPairsInMagnet[iGamma], gammaFromEPPairsInMagnet[jGamma]));
    }
  }
  cout << "fInvMassSpectrumGammaEPPairs: " << gammaFromEPPairs.size() <<  endl;  
  for (Int_t iGamma = 0; iGamma < gammaFromEPPairs.size(); iGamma++) {
    for (Int_t jGamma = iGamma + 1; jGamma < gammaFromEPPairs.size(); jGamma++) {
      //cout << iGamma << " " << jGamma << endl;
      fInvMassSpectrumGammaEPPairs->Fill(CalcInvariantMass(gammaFromEPPairs[iGamma], gammaFromEPPairs[jGamma]));
    }
  }
  
  
  // Find Gamma Gamma Pairs
  for (Int_t iGamma = 0; iGamma < fGammaIds.size(); iGamma++) {
    for (Int_t jGamma = iGamma+1; jGamma < fGammaIds.size(); jGamma++) {
      gtrack1 = (CbmMCTrack*)fMCTracks->At(fGammaIds[iGamma]);
      gtrack2 = (CbmMCTrack*)fMCTracks->At(fGammaIds[jGamma]); 
     
      fInvMassSpectrumGammaAllPairs->Fill(CalcInvariantMass(gtrack1, gtrack2));
      if ((fMCParticleMap[fGammaIds[iGamma]]->motherId == fMCParticleMap[fGammaIds[jGamma]]->motherId) 
	  && 
	  (fMCParticleMap[fGammaIds[iGamma]]->motherId != -1)
	  ) { 
	fInvMassSpectrumGammaTruePairs->Fill(CalcInvariantMass(gtrack1, gtrack2));
	std::pair<Int_t,Int_t> ggPair (fGammaIds[iGamma], fGammaIds[jGamma]);
	//ggPair = make_pair (fGammaIds[iGamma], fGammaIds[jGamma]);
	gammaGammaPairs.push_back(ggPair);
	fgammaAndGammaMother->Fill(PdgToGeant(fMCParticleMap[fMCParticleMap[fGammaIds[iGamma]]->motherId]->PID));
      }
    }
  }
  // Find Daliz Triplets
  for (Int_t iGamma = 0; iGamma < fGammaIds.size(); iGamma++) {
    for (Int_t iPair = 0; iPair < electronPositronPairs.size(); iPair++) {
      if (fMCParticleMap[fGammaIds[iGamma]]->motherId == fMCParticleMap[electronPositronPairs[iPair].first]->motherId) {
	std::vector<Int_t> triplet;
	triplet.push_back(electronPositronPairs[iPair].first);
	triplet.push_back(electronPositronPairs[iPair].second);
	triplet.push_back(fGammaIds[iGamma]);
	dalizTriplets.push_back(triplet);
	fDalizMother->Fill(PdgToGeant(fMCParticleMap[fMCParticleMap[fGammaIds[iGamma]]->motherId]->PID));
      }
    }
  }
  //=========================================================================
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


  // Vertizes of real electron positron pairs
  for (Int_t iPair = 0; iPair < electronPositronPairs.size(); iPair++ ) {
    etrack = (CbmMCTrack*)fMCTracks->At(electronPositronPairs[iPair].first);
    ptrack = (CbmMCTrack*)fMCTracks->At(electronPositronPairs[iPair].second);

    fPairAllVertex[0]->Fill(etrack->GetStartX());
    fPairAllVertex[1]->Fill(etrack->GetStartY());
    fPairAllVertex[2]->Fill(etrack->GetStartZ());
    fBirthPair->Fill(etrack->GetStartX(), etrack->GetStartY(), etrack->GetStartZ());
    fPairHistory->Fill(0);
    if (VertexInMagnet(etrack))
      fPairHistory->Fill(3);
    if (VertexInTarget(etrack))
      fPairHistory->Fill(6);
    if (fMCParticleMap[fMCParticleMap[electronPositronPairs[iPair].first]->motherId]->PID == 22) { 	   
      fPairGammaVertex[0]->Fill(etrack->GetStartX());
      fPairGammaVertex[1]->Fill(etrack->GetStartY());
      fPairGammaVertex[2]->Fill(etrack->GetStartZ());
      fZBirthEPfromGamma->Fill(etrack->GetStartZ());
      fPairHistory->Fill(1);
      if (VertexInMagnet(etrack))
	fPairHistory->Fill(4);
      if (VertexInTarget(etrack))
	fPairHistory->Fill(7);
    }
    if (fMCParticleMap[fMCParticleMap[electronPositronPairs[iPair].first]->motherId]->PID == 111) { 
      fPairPi0Vertex[0]->Fill(etrack->GetStartX());
      fPairPi0Vertex[1]->Fill(etrack->GetStartY());
      fPairPi0Vertex[2]->Fill(etrack->GetStartZ());
      fPairHistory->Fill(2);
      if (VertexInMagnet(etrack))
	fPairHistory->Fill(5);
      if (VertexInTarget(etrack))
	fPairHistory->Fill(8);
    }
 
  }
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  for (it=fMCParticleMap.begin(); it != fMCParticleMap.end(); it++) {
    mctrack = (CbmMCTrack*)fMCTracks->At((*it).first);
    fNoDaughters->Fill(PdgToGeant(mctrack->GetPdgCode()), (*it).second->daughterIds.size());
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
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
  StatisticHistos();
  SaveHistosToFile();
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();

  printf("\n\n******************** Reading Test  **********************\n");
  printf("   RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  printf("*********************************************************\n\n");
  /*
    if(gtrack1 != NULL)
    delete gtrack1;
    if(gtrack2 != NULL)
    delete gtrack2;
    if(etrack != NULL)
    delete etrack;
    if(ptrack != NULL)
    delete ptrack;
    if(mctrack != NULL)
    delete mctrack;
    if(track != NULL)
    delete track;
    if(mother != NULL)
    delete mother;
    if(grandmother != NULL)
    delete grandmother;
    if(daughter != NULL)
    delete daughter;
    if(mcpt != NULL)
    delete mcpt;
    if(digi != NULL)
    delete digi;
    if(cluster != NULL)
    delete cluster;
    if(hit != NULL)
    delete hit;
    if(track != NULL)
    delete track;
    if(gtrack != NULL)
    delete gtrack;
    if(pvertex != NULL)
    delete pvertex;
  */
  // TODO:: clear all global data container
 

  for (Int_t i = 0; i < fGammaFromPairs.size(); i++)
    delete fGammaFromPairs[i];
  for (Int_t i = 0; i < fMCParticleMap.size(); i++)
    delete fMCParticleMap[i];
  for (Int_t i = 0; i < gammaFromEPPairs.size(); i++)
    delete gammaFromEPPairs[i];
  /*
    for (Int_t i = 0; i < gammaFromEPPairsInMagnet.size(); i++)
    delete gammaFromEPPairsInMagnet[i];
    for (Int_t i = 0; i < gammaFromEPPairsInTarget.size(); i++)
    delete gammaFromEPPairsInTarget[i];
    for (Int_t i = 0; i < gammaFromEPPairsOpeningAngle.size(); i++)
    delete gammaFromEPPairsOpeningAngle[i];
    for (Int_t i = 0; i < gammaFromTrueEPPairs.size(); i++)
    delete gammaFromTrueEPPairs[i];
  */

  //delete track;
  //delete gtrack;
  //delete etrack;
  //delete ptrack;
  //delete gtrack1;
  //delete gtrack2;
  //delete mctrack;
  //delete mother;
}

Bool_t CbmTrdPhotonAnalysis::CloseByVertex(CbmMCTrack* trackA, CbmMCTrack* trackB, Double_t minimumDistance)
{
  Double_t vertexDistance = CalcVertexDistance(trackA, trackB);
  if (vertexDistance < minimumDistance)
    return true;
  else
    return false;
}

Double_t CbmTrdPhotonAnalysis::CalcVertexDistance(CbmMCTrack* trackA, CbmMCTrack* trackB)
{
 Double_t vertexDistance = TMath::Sqrt(pow(trackA->GetStartX() - trackB->GetStartX(),2) + pow(trackA->GetStartY() - trackB->GetStartY(),2) + pow(trackA->GetStartZ() - trackB->GetStartZ(),2));
 return vertexDistance;
}

Double_t CbmTrdPhotonAnalysis::CalcPt(CbmMCTrack* trackA, CbmMCTrack* trackB)
{
  Double_t Px = trackA->GetPx() + trackB->GetPx();
  Double_t Py = trackA->GetPy() + trackB->GetPy();
  Double_t Pt = TMath::Sqrt(pow(Px,2) + pow(Py,2));

  return Pt;
}

Double_t CbmTrdPhotonAnalysis::CalcInvariantMass(CbmMCTrack* trackA, CbmMCTrack* trackB)
{
  //Double_t energyA = TMath::Sqrt(pow(trackA->GetPx(),2) + pow(trackA->GetPy(),2) + pow(trackA->GetPz(),2));
  //Double_t energyB = TMath::Sqrt(pow(trackB->GetPx(),2) + pow(trackB->GetPy(),2) + pow(trackB->GetPz(),2));
  //if (energyA != trackA->GetEnergy() || energyB != trackB->GetEnergy())
  //printf ("A: %.8f != %.8f Delta = %E  B: %.8f != %.8f Delta = %E\n", energyA, trackA->GetEnergy(), energyA-trackA->GetEnergy(), energyB, trackB->GetEnergy(), energyB - trackB->GetEnergy());
  Double_t energyAB = trackA->GetEnergy() + trackB->GetEnergy();//energyA + energyB;
  //Double_t ptAB = trackA->GetPt() + trackB->GetPt();
  Double_t invariantMass = TMath::Sqrt(
			      pow(energyAB,2) 
			      - pow((trackA->GetPx() + trackB->GetPx()),2) 
			      - pow((trackA->GetPy() + trackB->GetPy()),2) 
			      - pow((trackA->GetPz() + trackB->GetPz()),2)
			      );
  return invariantMass;
}
Double_t CbmTrdPhotonAnalysis::CalcOpeningAngle(CbmMCTrack* trackA, CbmMCTrack* trackB) {
  //Double_t energyA = trackA->GetEnergy();//TMath::Sqrt(pow(trackA->GetPx(),2) + pow(trackA->GetPy(),2) + pow(trackA->GetPz(),2));
  //Double_t energyB = trackB->GetEnergy();//TMath::Sqrt(pow(trackB->GetPx(),2) + pow(trackB->GetPy(),2) + pow(trackB->GetPz(),2));
  /*
    Double_t openingAngle = TMath::ACos(0.5 * pow(CalcInvariantMass(trackA, trackB),2) - pow(0.000510998928,2) - energyA * energyB
    + (trackA->GetPx() * trackB->GetPx() + trackA->GetPy() * trackB->GetPy() + trackA->GetPz() * trackB->GetPz())
    ) / TMath::Pi() * 180.;
  */
  Double_t openingAngle = TMath::ACos(
				      (
				       0.5 * 
				       (pow(CalcInvariantMass(trackA, trackB),2) - pow(trackA->GetMass(),2) - pow(trackB->GetMass(),2))
				       - (trackA->GetEnergy() * trackB->GetEnergy())
				       ) 
				      // / (-1. * (trackA->GetPx() * trackB->GetPx() + trackA->GetPy() * trackB->GetPy() + trackA->GetPz() * trackB->GetPz()))
				      
					/ ( -1. * 
					TMath::Sqrt(pow(trackA->GetPx(),2) + pow(trackA->GetPy(),2) + pow(trackA->GetPz(),2)) 
					* TMath::Sqrt(pow(trackB->GetPx(),2) + pow(trackB->GetPy(),2) + pow(trackB->GetPz(),2))
					)
				      
				      ) * 180.0 / TMath::Pi();
  return openingAngle;
}
void CbmTrdPhotonAnalysis::StatisticHistos()
{
  Int_t allPairs = 0;
  Int_t gammaPairs = 0;
  Int_t pi0Pairs = 0;
  Int_t gammaWoPi0 = 0;
  for (Int_t bin = 1; bin < fEPPairOpeningAngleInTarget->GetNbinsX(); bin++) {
    //for (Int_t bin = 1; bin < maxbin; bin++) {
    allPairs += fEPPairOpeningAngleInTarget->GetBinContent(bin);
    gammaPairs += fEPPairOpeningAngleGamma->GetBinContent(bin);
    pi0Pairs += fEPPairOpeningAnglePi0->GetBinContent(bin);
    gammaWoPi0 = gammaPairs - pi0Pairs;
    fPairOpeningAngleAll->SetBinContent(bin,allPairs / (Float_t)fEPPairOpeningAngleInTarget->GetEntries());
    fPairOpeningAngleGamma->SetBinContent(bin,gammaPairs / (Float_t)fEPPairOpeningAngleInTarget/*fEPPairOpeningAngleGamma*/->GetEntries());
    fPairOpeningAnglePi0->SetBinContent(bin,pi0Pairs / (Float_t)fEPPairOpeningAngleInTarget/*fEPPairOpeningAnglePi0*/->GetEntries());
    fPairOpeningAngleGammaWoPi0->SetBinContent(bin,gammaWoPi0 / (Float_t)(fEPPairOpeningAngleInTarget/*fEPPairOpeningAngleGamma->GetEntries() - fEPPairOpeningAnglePi0*/->GetEntries()));
    //}
  }
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
  //if (PdgCode == -1)
  //return 49;
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
  fPt = new TH2I("Pt","p_{T} vs PID",200,0,2,49,0.5,49.5);
  fP = new TH2I("P","p vs PID",200,0,2,49,0.5,49.5);
  fDalizMother = new TH1I("DalizMother","Mother ID for e^{+} & e^{-} & #gamma",49,0.5,49.5);
  fePlusMinusMother = new TH1I("ePlusMinusMother","Mother ID for e^{+}/e^{-}",49,0.5,49.5);
  fePlusAndMinusMother = new TH1I("ePlusAndMinusMother","Mother ID for e^{+} & e^{-}",49,0.5,49.5);
  fgammaMother = new TH1I("gammaMother","Mother ID for #gamma",49,0.5,49.5);
  fgammaAndGammaMother = new TH1I("gammaAndGammaMother","Mother ID for #gamma & #gamma",49,0.5,49.5);
  fgammaDaughter = new TH1I("gammaDaughter","Daughter ID for #gamma",49,0.5,49.5);
  fZBirthAll = new TH2I("ZBirth_All","z birth",10000,0,1000,49,0.5,49.5);
  fZBirthEPfromGamma = new TH1I("ZBirthEPfromGamma","z birth from e^{+} & e^{-} pairs with mother #gamma",10000,0,1000);
  fNoDaughters = new TH2I("NoDaughters","Number of daughters",49,0.5,49.5,101,-0.5,100.5);
  fMotherDaughter = new TH2I("MotherDaughter","mother / daughter",49,0.5,49.5,49,0.5,49.5);
  fMotherDaughterZBirth = new TH3I("MotherDaughterZBirth","mother / daughter / z-birth",10000,0,1000,49,0.5,49.5,49,0.5,49.5);
  fInvMPairMother = new TH2I("InvMPairMother","Invariant Mass of e^{+} & e^{-} pairs vs. mother id",2000,0,2,49,0.5,49.5);
  fPtPairMother = new TH2I("PtPairMother","p_{T} of e^{+} & e^{-} pairs vs. mother id",200,0,2,49,0.5,49.5);
  fPPairMother  = new TH2I("PPairMother","p of e^{+} & e^{-} pairs vs. mother id",200,0,2,49,0.5,49.5);
  fOpenAnglePairMother = new TH2I("OpenAnglePairMother","Opening Angle of e^{+} & e^{-} pairs vs. mother id",1800,0,180,49,0.5,49.5);

  for (Int_t bin = 0; bin < 49; bin++) {
    fInvMPairMother->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fPtPairMother->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fPPairMother->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fOpenAnglePairMother->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fMCPid->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fGTPid->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fPt->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fP->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fDalizMother->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fePlusMinusMother->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fePlusAndMinusMother->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fgammaAndGammaMother->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
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
    fPairAllVertex[i] = new TH1I("PairAllVertex_" + name[i], "all e^{+} e^{-} pair vertex " + name[i], 1000, low[i], high[i]);
    fPairGammaVertex[i] = new TH1I("PairGammaVertex_" + name[i], "e^{+} e^{-} pair from #gamma vertex " + name[i], 1000, low[i], high[i]);
    fPairPi0Vertex[i] = new TH1I("PairPi0Vertex_" + name[i], "e^{+} e^{-} pair from #pi^{0} vertex " + name[i], 1000, low[i], high[i]);

    fpi02GammaVertex[i] = new TH1I("pi02GammaVertex_" + name[i], "decay vertex of #pi^{0} to 2#gamma or e^{+}e^{-}#gamma" + name[i], 1000, low[i], high[i]);
    fgamma2PairVertex[i] = new TH1I("gamma2PairVertex_" + name[i], "decay vertex of #gamma to e^{+} e^{-} pair " + name[i], 1000, low[i], high[i]);

    NiceHisto1(fPairAllVertex[i],1,0,0,TString(name[i]+"-position [cm]"),"");
    NiceHisto1(fPairGammaVertex[i],2,0,0,TString(name[i]+"-position [cm]"),"");
    NiceHisto1(fPairPi0Vertex[i],3,0,0,TString(name[i]+"-position [cm]"),"");

    NiceHisto1(fpi02GammaVertex[i],1,0,0,TString(name[i]+"-position [cm]"),"");
    NiceHisto1( fgamma2PairVertex[i],2,0,0,TString(name[i]+"-position [cm]"),"");
  }
  /*
    fMCPid = new TH1I("MCPid","MC Pid",4501,-1000.5,3500.5);
    fGTPid = new TH1I("GTPid","GT Pid",4501,-1000.5,3500.5);
    fePlusMinusMother = new TH1I("fePlusMinusMother","Mother ID for e^{+}/e^{-}",4501,-1000.5,3500.5);
  */
  fEPPairVertexDistance = new TH1I("EPPairVertexDistance","Vertex distance for e^{+}e^{-} pairs",250000,0,2500);
  fZBirth[0] = new TH1I("ZBirth_gamma","z birth #gamma",10000,0,1000);
  fZBirth[1] = new TH1I("ZBirth_positron","z birth e^{+}",10000,0,1000);
  fZBirth[2] = new TH1I("ZBirth_electron","z birth e^{-}",10000,0,1000);
  fZBirth[3] = new TH1I("ZBirth_pi0","z birth #pi^{0}",10000,0,1000);
  fBirthPi0 = new TH3I("BirthPi0","#pi^{0} vertex"           ,100,-500,500,100,-500,500,100,0,1000);
  fBirthPair = new TH3I("BirthPair","e^{-}e^{+} pairs vertex",100,-500,500,100,-500,500,100,0,1000);
  fBirthGamma = new TH3I("BirthGamma","#gamma vertex"        ,100,-500,500,100,-500,500,100,0,1000);
  fPairHistory = new TH1I("PairHistory","history for #gamma",10,-0.5,9.5);
  fPairHistory->GetXaxis()->SetBinLabel( 1,"all e^{-} e^{+} pairs");
  fPairHistory->GetXaxis()->SetBinLabel( 2,"pairs from #gamma");
  fPairHistory->GetXaxis()->SetBinLabel( 3,"pairs from #pi^{0}#rightarrow e^{+}+e^{-}+#gamma");
  fPairHistory->GetXaxis()->SetBinLabel( 4,"all pairs produced within magnet");
  fPairHistory->GetXaxis()->SetBinLabel( 5,"pairs from #gamma within magnet");
  fPairHistory->GetXaxis()->SetBinLabel( 6,"pairs from #pi^{0} within magnet");
  fPairHistory->GetXaxis()->SetBinLabel( 7,"all pairs produced within target");
  fPairHistory->GetXaxis()->SetBinLabel( 8,"pairs from #gamma within target");
  fPairHistory->GetXaxis()->SetBinLabel( 9,"pairs from #pi^{0} within target");
  fPairHistory->GetXaxis()->SetBinLabel(10,"");

  fInvMassSpectrumGammaTruePairs = new TH1F("InvMassSpectrumGammaTruePairs","Invariant mass spectrum from true MC-#gamma-#gamma pairs",2000,0,2);
  fInvMassSpectrumGammaAllPairs  = new TH1F("InvMassSpectrumGammaAllPairs","Invariant mass spectrum from all MC-#gamma-#gamma pairs",2000,0,2);
  fInvMassSpectrumGammaEPPairs   = new TH1F("InvMassSpectrumGammaEPPairs","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs",2000,0,2);
  fInvMassSpectrumGammaEPPairsInTarget    = new TH1F("InvMassSpectrumGammaEPPairsInTarget","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs in target",2000,0,2);
  fInvMassSpectrumGammaEPPairsInMagnet    = new TH1F("InvMassSpectrumGammaEPPairsInMagnet","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs in magnet",2000,0,2);
  fInvMassSpectrumGammaEPPairsOpenAngle   = new TH1F("InvMassSpectrumGammaEPPairsOpenAngle","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs oping angle cut",2000,0,2);
  fInvMassSpectrumGammaEPPairsGamma       = new TH1F("InvMassSpectrumGammaEPPairsGamma","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs mother=#gamma",2000,0,2);
  fInvMassSpectrumGammaEPPairsPi0         = new TH1F("InvMassSpectrumGammaEPPairsPi0","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs mother=#pi^{0}",2000,0,2);

  fInvMassSpectrumTrueEPPairs  = new TH1F("InvMassSpectrumTrueEPPairs","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",2000,0,2);
  fInvMassSpectrumAllEPPairs   = new TH1F("InvMassSpectrumAllEPPairs","Invariant mass spectrum from all MC-e^{+}e^{-} pairs",2000,0,2);
  fInvMassSpectrumEPPairsInTarget   = new TH1F("InvMassSpectrumAllEPPairsInTarget","Invariant mass spectrum from all MC-e^{+}e^{-} pairs in target",2000,0,2);
  fInvMassSpectrumEPPairsInMagnet   = new TH1F("InvMassSpectrumAllEPPairsInMagnet","Invariant mass spectrum from all MC-e^{+}e^{-} pairs in magnet",2000,0,2);

  fEPPairOpeningAngle   = new TH1F("EPPairOpeningAngle","opening angle #theta of MC-e^{+}e^{-} pairs",1800,0,180);
  fEPPairOpeningAngleGamma   = new TH1F("EPPairOpeningAngleGamma ","opening angle #theta of MC-e^{+}e^{-} pairs mother #gamma",1800,0,180);
  fEPPairOpeningAnglePi0   = new TH1F("EPPairOpeningAnglePi0","opening angle #theta of MC-e^{+}e^{-} pairs mother #pi^{0}",1800,0,180);
  fEPPairOpeningAngleSameMother   = new TH1F("EPPairOpeningAngleSameMother","opening angle #theta of MC-e^{+}e^{-} pairs same mother",1800,0,180);
  fEPPairOpeningAngleInTarget   = new TH1F("EPPairOpeningAngleInTarget","opening angle #theta of MC-e^{+}e^{-} pairs in target",1800,0,180);
  fEPPairOpeningAngleInMagnet   = new TH1F("EPPairOpeningAngleInMagnet","opening angle #theta of MC-e^{+}e^{-} pairs in magnet",1800,0,180);

  fPairOpeningAngleAll  = new TH1F("PairOpeningAngleAll","opening angle #theta of MC-e^{+}e^{-} pairs",1800,0,180);
  fPairOpeningAngleGamma  = new TH1F("PairOpeningAngleGamma","opening angle #theta of MC-e^{+}e^{-} pairs from #gamma",1800,0,180);
  fPairOpeningAnglePi0  = new TH1F("PairOpeningAnglePi0","opening angle #theta of MC-e^{+}e^{-} pairs from #pi^{0}",1800,0,180);
  fPairOpeningAngleGammaWoPi0  = new TH1F("PairOpeningAngleGammaWoPi0","opening angle #theta of MC-e^{+}e^{-} pairs from #gamma - pairs from #pi^{0}",1800,0,180);
  /*
    TString nameInvM[20] = {"",
    "",
    ""
    };
    TString titleInvM[20] = {"",
    "",
    ""
    };
    for (Int_t i = 0; i < 20; i++) {
    fInvMSpectra[i] = new TH1F(nameInvM[i], titleInvM[i], 2000, 0, 2);

    }

  */

  NiceHisto1(fMCPid,1,20,1,"","");
  NiceHisto1(fGTPid,1,20,1,"","");
  NiceHisto2(fPt,1,1,1,"p_{T} [Gev/c]","","");
  NiceHisto2(fP,1,1,1,"p [Gev/c]","","");
  NiceHisto1(fePlusMinusMother,1,20,1,"","");
  NiceHisto1(fgammaMother,1,20,1,"","");
  NiceHisto1(fgammaDaughter,1,20,1,"","");
  NiceHisto1(fZBirth[0],1,20,1,"z-position [cm]","");
  NiceHisto1(fZBirth[1],2,20,1,"z-position [cm]","");
  NiceHisto1(fZBirth[2],3,20,1,"z-position [cm]","");
  NiceHisto1(fZBirth[3],4,20,1,"z-position [cm]","");
  NiceHisto1(fPairHistory,1,20,1,"","");
  NiceHisto2(fNoDaughters,1,1,1,"","Number of daughters","");
  NiceHisto2(fZBirthAll,1,1,1,"z-position [cm]","","#");
  NiceHisto2(fMotherDaughter,1,1,1,"Mother","Daughter","#");
  NiceHisto3(fBirthPi0,1,7,1,"x-position [cm]","y-position [cm]","z-position [cm]");
  NiceHisto3(fBirthGamma,2,7,1,"x-position [cm]","y-position [cm]","z-position [cm]");
  NiceHisto3(fBirthPair,3,7,1,"x-position [cm]","y-position [cm]","z-position [cm]");
  NiceHisto3(fMotherDaughterZBirth,1,1,1,"z-position [cm]","","");

  NiceHisto1(fEPPairVertexDistance,1,1,1,"vertex distance [cm]","");


  NiceHisto2(fInvMPairMother,1,1,1,"Invariant mass [GeV/c^{2}]","Mother","");
  NiceHisto2(fPtPairMother,1,1,1,"p_{T} [Gev/c]","Mother","");
  NiceHisto2(fPPairMother,1,1,1,"p [Gev/c]","Mother","");
  NiceHisto2(fOpenAnglePairMother,1,1,1,"opening angle #theta [#circ]","Mother","");

  NiceHisto1(fInvMassSpectrumGammaAllPairs,1,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fInvMassSpectrumGammaTruePairs,2,1,1,"Invariant mass [GeV/c^{2}]","");

  NiceHisto1(fInvMassSpectrumAllEPPairs,1,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fInvMassSpectrumEPPairsInMagnet,2,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fInvMassSpectrumEPPairsInTarget,3,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fInvMassSpectrumTrueEPPairs,4,1,1,"Invariant mass [GeV/c^{2}]","");

  NiceHisto1(fInvMassSpectrumGammaEPPairs,1,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fInvMassSpectrumGammaEPPairsInMagnet,2,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fInvMassSpectrumGammaEPPairsInTarget,3,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fInvMassSpectrumGammaEPPairsOpenAngle,4,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fInvMassSpectrumGammaEPPairsGamma,6,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fInvMassSpectrumGammaEPPairsPi0,7,1,1,"Invariant mass [GeV/c^{2}]","");

  NiceHisto1(fEPPairOpeningAngle,1,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fEPPairOpeningAngleInMagnet,2,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fEPPairOpeningAngleInTarget,3,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fEPPairOpeningAngleSameMother,4,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fEPPairOpeningAngleGamma,6,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fEPPairOpeningAnglePi0,7,1,1,"opening angle #theta [#circ]","");

  NiceHisto1(fPairOpeningAngleAll,1,1,1,"opening angle #theta cut [#circ]","pair fraction");
  NiceHisto1(fPairOpeningAngleGamma,2,1,1,"opening angle #theta cut [#circ]","pair fraction");
  NiceHisto1(fPairOpeningAnglePi0,3,1,1,"opening angle #theta cut [#circ]","pair fraction");
  NiceHisto1(fPairOpeningAngleGammaWoPi0,4,1,1,"opening angle #theta cut [#circ]","pair fraction");
}
void  CbmTrdPhotonAnalysis::NormalizeHistos(TH1* h)
{
  h->Scale(1./ h->GetEntries());
}


void CbmTrdPhotonAnalysis::SaveHistosToFile()
{
  //TODO:: undate histos instead of overwriting for each event. find a way to get event number from run task
  /*
    TCanvas *c = new TCanvas("CbmTrdPhotonAnalysis","CbmTrdPhotonAnalysis",800,600);
    c->Divide(2,1);
    c->Cd(1);
    fMCPid->Draw();
    c->Cd(2);
    fGTPid->Draw();
    c->SaveAs("data/CbmTrdPhotonAnalysis.pdf");
  */
  //TFile *outFile = new TFile("data/CbmTrdPhotonAnalysis.root","RECREATE","output of CbmTrdPhotonAnalysis");
  //outFile->Cd();
  gDirectory->pwd();
  gDirectory->Cd("data/test.esd.pa.root:");
  gDirectory->pwd();
  if (!gDirectory->Cd("PhotonAnalysis")) 
    gDirectory->mkdir("PhotonAnalysis");
  gDirectory->Cd("PhotonAnalysis");
  gDirectory->pwd();

  if (!gDirectory->Cd("MC_Statistic")) 
    gDirectory->mkdir("MC_Statistic");
  gDirectory->Cd("MC_Statistic");
  gDirectory->pwd();
  fMCPid->Write("", TObject::kOverwrite);
  fGTPid->Write("", TObject::kOverwrite);
  fPt->Write("", TObject::kOverwrite);
  fP->Write("", TObject::kOverwrite);
  fePlusMinusMother->Write("", TObject::kOverwrite);
  fePlusAndMinusMother->Write("", TObject::kOverwrite);
  fgammaMother->Write("", TObject::kOverwrite);
  fgammaAndGammaMother->Write("", TObject::kOverwrite);
  fgammaDaughter->Write("", TObject::kOverwrite);
  fInvMPairMother->Write("", TObject::kOverwrite);
  fPtPairMother->Write("", TObject::kOverwrite);
  fPPairMother->Write("", TObject::kOverwrite);
  fOpenAnglePairMother->Write("", TObject::kOverwrite);
  fPairHistory->Write("", TObject::kOverwrite);
  for (Int_t i = 0; i < 4; i++) {
    fZBirth[i]->Write("", TObject::kOverwrite);
  }

  fZBirthAll->Write("", TObject::kOverwrite);
  fDalizMother->Write("", TObject::kOverwrite);
  fMotherDaughter->Write("", TObject::kOverwrite);
  
  fMotherDaughterZBirth->Write("", TObject::kOverwrite);
  fZBirthEPfromGamma->Write("", TObject::kOverwrite);
  fNoDaughters->Write("", TObject::kOverwrite);

  fBirthGamma->Write("", TObject::kOverwrite);
  fBirthPi0->Write("", TObject::kOverwrite);
  fBirthPair->Write("", TObject::kOverwrite);

  for (Int_t i = 0; i < 3; i++) {
    fPairAllVertex[i]->Write("", TObject::kOverwrite);
    fPairGammaVertex[i]->Write("", TObject::kOverwrite);
    fPairPi0Vertex[i]->Write("", TObject::kOverwrite);

    fpi02GammaVertex[i]->Write("", TObject::kOverwrite);
    fgamma2PairVertex[i]->Write("", TObject::kOverwrite);
  }
  fEPPairVertexDistance->Write("", TObject::kOverwrite);

  gDirectory->Cd("..");

  if (!gDirectory->Cd("InvMassSpectrum_GammaPairs")) 
    gDirectory->mkdir("InvMassSpectrum_GammaPairs");
  gDirectory->Cd("InvMassSpectrum_GammaPairs");
  gDirectory->pwd();
  //NormalizeHistos(fInvMassSpectrumGammaEPPairs);
  fInvMassSpectrumGammaEPPairs->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumGammaEPPairsInMagnet);
  fInvMassSpectrumGammaEPPairsInMagnet->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumGammaEPPairsInTarget);
  fInvMassSpectrumGammaEPPairsInTarget->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumGammaEPPairsOpenAngle);
  fInvMassSpectrumGammaEPPairsOpenAngle->Write("", TObject::kOverwrite);
  fInvMassSpectrumGammaEPPairsGamma->Write("", TObject::kOverwrite);
  fInvMassSpectrumGammaEPPairsPi0->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumGammaAllPairs);
  fInvMassSpectrumGammaAllPairs->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumGammaTruePairs);
  fInvMassSpectrumGammaTruePairs->Write("", TObject::kOverwrite);
  gDirectory->Cd("..");

  if (!gDirectory->Cd("InvMassSpectrum_EPPairs")) 
    gDirectory->mkdir("InvMassSpectrum_EPPairs");
  gDirectory->Cd("InvMassSpectrum_EPPairs");
  gDirectory->pwd();
  //NormalizeHistos(fInvMassSpectrumAllEPPairs);
  fInvMassSpectrumAllEPPairs->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumEPPairsInTarget);
  fInvMassSpectrumEPPairsInTarget->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumEPPairsInMagnet);
  fInvMassSpectrumEPPairsInMagnet->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumTrueEPPairs);
  fInvMassSpectrumTrueEPPairs->Write("", TObject::kOverwrite);
  gDirectory->Cd("..");

  if (!gDirectory->Cd("OpeningAngle_EPPairs")) 
    gDirectory->mkdir("OpeningAngle_EPPairs");
  gDirectory->Cd("OpeningAngle_EPPairs");
  gDirectory->pwd();
  //NormalizeHistos(fEPPairOpeningAngle);
  fEPPairOpeningAngle->Write("", TObject::kOverwrite);
 
  //NormalizeHistos(fEPPairOpeningAngleInMagnet);
  fEPPairOpeningAngleInMagnet->Write("", TObject::kOverwrite);
  //NormalizeHistos(fEPPairOpeningAngleInTarget);
  fEPPairOpeningAngleInTarget->Write("", TObject::kOverwrite);
  //NormalizeHistos(fEPPairOpeningAngleSameMother);
  fEPPairOpeningAngleSameMother->Write("", TObject::kOverwrite);
  //NormalizeHistos(fEPPairOpeningAngleGamma);
  fEPPairOpeningAngleGamma->Write("", TObject::kOverwrite);
  //NormalizeHistos(fEPPairOpeningAnglePi0);
  fEPPairOpeningAnglePi0->Write("", TObject::kOverwrite);

  fPairOpeningAngleAll->Write("", TObject::kOverwrite);
  fPairOpeningAngleGamma->Write("", TObject::kOverwrite);
  fPairOpeningAnglePi0->Write("", TObject::kOverwrite);
  fPairOpeningAngleGammaWoPi0->Write("", TObject::kOverwrite);

  gDirectory->Cd("..");
  //outFile->Close();
  //c->Close();
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


void CbmTrdPhotonAnalysis::NiceHisto1(TH1 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle) 
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
  h->SetXTitle(xTitle);
  h->SetYTitle(yTitle);
}
void CbmTrdPhotonAnalysis::NiceHisto2(TH2 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle, TString zTitle) 
{
  h->SetMarkerStyle(mStyle);
  h->SetMarkerSize(mSize);  
  h->SetMarkerColor(color);
  h->SetLineColor(color);  
  h->GetXaxis()->SetLabelSize(0.03);
  h->GetYaxis()->SetLabelSize(0.03);
  h->GetZaxis()->SetLabelSize(0.03);
  h->GetXaxis()->SetTitleSize(0.035);
  h->GetXaxis()->SetTitleOffset(1.5);
  h->GetYaxis()->SetTitleSize(0.035);
  h->GetYaxis()->SetTitleOffset(1.5);
  h->GetZaxis()->SetTitleSize(0.035);
  h->GetZaxis()->SetTitleOffset(1.25);
  h->SetXTitle(xTitle);
  h->SetYTitle(yTitle);
  h->SetZTitle(zTitle);
}
void CbmTrdPhotonAnalysis::NiceHisto3(TH3 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle, TString zTitle) 
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
  h->GetZaxis()->SetTitleOffset(1.25);
  h->SetXTitle(xTitle);
  h->SetYTitle(yTitle);
  h->SetZTitle(zTitle);
}
void CbmTrdPhotonAnalysis::NiceProfile(TProfile *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle) 
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
  h->SetXTitle(xTitle);
  h->SetYTitle(yTitle);
}

void CbmTrdPhotonAnalysis::NiceLegend(TLegend *l)
{
  l->SetLineColor(0);
  l->SetLineStyle(0);
  l->SetFillStyle(0);
  l->SetTextSize(0.03);
}
