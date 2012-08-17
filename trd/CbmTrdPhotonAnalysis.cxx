// ------------------------------------------------------------------------
// -----                       CbmTrdPhotonAnalysis source file       -----
// -----                       Created  13/02/12  by C.Bergmann       -----
// ------------------------------------------------------------------------

#include "CbmTrdPhotonAnalysis.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"

#include "CbmTrdModule.h"
#include "CbmTrdDigiPar.h"

#include "CbmMCTrack.h"
/**/
#include "FairTask.h"
#include "FairRootManager.h"
#include "CbmVertex.h"
#include "CbmGlobalTrack.h"
/*
#include "CbmStsKFTrackFitter.h"
*/
#include "CbmRichElectronIdAnn.h"

#include "FairMCPoint.h"

#include "CbmMCTrack.h"

#include "CbmStsTrack.h"
#include "CbmRichRing.h"
#include "CbmTrackMatch.h"
#include "CbmTrdTrack.h"
#include "CbmTrdHit.h"
#include "CbmTofHit.h"

#include "FairTrackParam.h"
#include "CbmTrackMatch.h"
#include "CbmKF.h"

#include "CbmRichPoint.h"
#include "CbmRichHit.h"
#include "CbmTofPoint.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "TGraph.h"
#include "TVector3.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TProfile.h"
#include "TObject.h"

#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "TDatabasePDG.h"

#include "CbmStsHit.h"
#include "CbmMvdHit.h"

#include "TString.h"
#include "TSystem.h"
#include "TStopwatch.h"

/**/
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
using std::flush;

CbmTrdPhotonAnalysis::CbmTrdPhotonAnalysis()
  : FairTask("TrdPhotonAnalysis",1),
    fMotherDaughterZBirth(NULL),
    fBirthGamma(NULL),
    fBirthPi0(NULL),
    fBirthPair(NULL),
    fKFFitter(NULL)
{
  //identification cuts
  fTrdAnnCut = 0.85;
  fRichAnnCut = 0.0;
  fUseRichAnn = true;
  fMeanA = -1.;
  fMeanB = -1.;
  fRmsA = -1.;
  fRmsB = -1.;
  fRmsCoeff = -1.;
  fDistCut = -1.;
  // analysis cuts
  fPtCut = 0.2;
  fAngleCut = 1.;
  fChiPrimCut = 3.;
  fGammaCut = 0.025;
  fStCutAngle = 1.5;
  fStCutPP = 1.5;
  fTtCutAngle = 0.75;
  fTtCutPP = 4.0;
  fMvd1CutP = 1.;
  fMvd1CutD = 0.4;
  fMvd2CutP = 1.5;
  fMvd2CutD = 0.5;
  if (fUseRichAnn){
    string richANNFile(gSystem->Getenv("VMCWORKDIR"));
    richANNFile += "/parameters/rich/el_id_ann_weights_rich_compact.txt";
    fElIdAnn = new CbmRichElectronIdAnn(richANNFile);
    fElIdAnn->Init();
  }
  fKFFitter = new CbmStsKFTrackFitter();
  fHistoMap.clear();
  fCandidates.clear();
  fElectronCandidates.clear();
  fPositronCandidates.clear();
  InitHistos();

}

CbmTrdPhotonAnalysis::CbmTrdPhotonAnalysis(const char *name, const char *title, Int_t iVerbose)
  : FairTask(name, iVerbose),
    fBirthGamma(NULL),
    fBirthPi0(NULL),
    fBirthPair(NULL),
    fKFFitter(NULL)
{
  //identification cuts
  fTrdAnnCut = 0.85;
  fRichAnnCut = 0.0;
  fUseRichAnn = true;
  fMeanA = -1.;
  fMeanB = -1.;
  fRmsA = -1.;
  fRmsB = -1.;
  fRmsCoeff = -1.;
  fDistCut = -1.;
  // analysis cuts
  fPtCut = 0.2;
  fAngleCut = 1.;
  fChiPrimCut = 3.;
  fGammaCut = 0.025;
  fStCutAngle = 1.5;
  fStCutPP = 1.5;
  fTtCutAngle = 0.75;
  fTtCutPP = 4.0;
  fMvd1CutP = 1.;
  fMvd1CutD = 0.4;
  fMvd2CutP = 1.5;
  fMvd2CutD = 0.5;
  if (fUseRichAnn){
    string richANNFile(gSystem->Getenv("VMCWORKDIR"));
    richANNFile += "/parameters/rich/el_id_ann_weights_rich_compact.txt";
    fElIdAnn = new CbmRichElectronIdAnn(richANNFile);
    fElIdAnn->Init();
  }
  fKFFitter = new CbmStsKFTrackFitter();
  fHistoMap.clear();
  fCandidates.clear();
  fElectronCandidates.clear();
  fPositronCandidates.clear();
  InitHistos();
 
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
    if (fTrdTrackMatches){
      fTrdTrackMatches->Clear("C");
      fTrdTrackMatches->Delete();
      delete fTrdTrackMatches;
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
   if(fPrimVertex){
      fPrimVertex->Clear("C");
      fPrimVertex->Delete();
      delete fPrimVertex;
    }
    if (fRichRings){
      fRichRings->Clear("C");
      fRichRings->Delete();
      delete fRichRings;
    }
    if (fRichProj){
      fRichProj->Clear("C");
      fRichProj->Delete();
      delete fRichProj;
    }
    if (fRichPoints){
      fRichPoints->Clear("C");
      fRichPoints->Delete();
      delete fRichPoints;
    }
    if (fRichRingMatches){
      fRichRingMatches->Clear("C");
      fRichRingMatches->Delete();
      delete fRichRingMatches;
    }
    if (fRichHits){
      fRichHits->Clear("C");
      fRichHits->Delete();
      delete fRichHits;
    }
    if (fStsTracks){
      fStsTracks->Clear("C");
      fStsTracks->Delete();
      delete fStsTracks;
    }
    if (fStsTrackMatches){
      fStsTrackMatches->Clear("C");
      fStsTrackMatches->Delete();
      delete fStsTrackMatches;
    }
    if (fStsHits){
      fStsHits->Clear("C");
      fStsHits->Delete();
      delete fStsHits;
    }
    if (fMvdHits){
      fMvdHits->Clear("C");
      fMvdHits->Delete();
      delete fMvdHits;
    }
    if (fTofHits){
      fTofHits->Clear("C");
      fTofHits->Delete();
      delete fTofHits;
    }
    if (fTofPoints){
      fTofPoints->Clear("C");
      fTofPoints->Delete();
      delete fTofPoints;
    }
  
    if (fKFFitter){
      //fKFFitter->Clear("C");
      //fKFFitter->Delete();
      delete fKFFitter;
    }
  
    // Delete global pointer
    if(fDigiPar)
      delete fDigiPar;
    if(fModuleInfo)
      delete fModuleInfo;

    delete fBirthGamma;
    delete fBirthPi0;
    delete fBirthPair;
    delete fMotherDaughterZBirth;
 
    for (fHistoMapIt = fHistoMap.begin(); fHistoMapIt != fHistoMap.end(); fHistoMapIt++)
      delete fHistoMapIt->second;

    delete fDigiPar;
    delete fModuleInfo;

  fCandidates.clear();
  fElectronCandidates.clear();
  fPositronCandidates.clear();
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

    fTrdTrackMatches = (TClonesArray*) ioman->GetObject("TrdTrackMatch");
    if (!fTrdTrackMatches) { cout << "-W CbmTrdPhotonAnalysis::Init: No TrdTrackMatch array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR; 
    }

    fRichHits = (TClonesArray*) ioman->GetObject("RichHit");
    if (!fRichHits) { cout << "-W CbmTrdPhotonAnalysis::Init: No RichHit array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR; 
    }

    fRichRings = (TClonesArray*) ioman->GetObject("RichRing");
    if (!fRichRings) { cout << "-W CbmTrdPhotonAnalysis::Init: No RichRing array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR; 
    }

    fRichPoints = (TClonesArray*) ioman->GetObject("RichPoint");
    if (!fRichPoints) {  cout << "-W CbmTrdPhotonAnalysis::Init: No RichPoint array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR; 
    }

    fRichRingMatches = (TClonesArray*) ioman->GetObject("RichRingMatch");
    if (!fRichRingMatches) {  cout << "-W CbmTrdPhotonAnalysis::Init: No RichRingMatch array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR; 
    }

    fRichProj = (TClonesArray*) ioman->GetObject("RichProjection");
    if (!fRichProj) {  cout << "-W CbmTrdPhotonAnalysis::Init: No RichProjection array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR; 
    }

    fStsTrackMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
    if (! fStsTrackMatches) {  cout << "-W CbmTrdPhotonAnalysis::Init: No StsTrackMatch array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR;
    }

    fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
    if (! fStsTracks) { cout << "-W CbmTrdPhotonAnalysis::Init: No StsTrack array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR; 
    }

    fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
    if (!fStsHits) { cout << "-W CbmTrdPhotonAnalysis::Init: No StsHit array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR;
    }

    fMvdHits = (TClonesArray*) ioman->GetObject("MvdHit");
    if (!fMvdHits) { cout << "-W CbmTrdPhotonAnalysis::Init: No MvdHit array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR; 
    }

    fTofPoints = (TClonesArray*) ioman->GetObject("TofPoint");
    if (!fTofPoints) { cout << "-W CbmTrdPhotonAnalysis::Init: No TofPoint array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR;
    }

    fTofHits = (TClonesArray*) ioman->GetObject("TofHit");
    if (!fTofHits) { cout << "-W CbmTrdPhotonAnalysis::Init: No TofHit array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR;
    }

    fGlobalTracks=(TClonesArray *)ioman->GetObject("GlobalTrack");
 
    if ( ! fGlobalTracks ) {
      cout << "-W CbmTrdPhotonAnalysis::Init: No GlobalTracks array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR;
    }
    /*
    fPrimaryVertex=(TClonesArray *)ioman->GetObject("PrimaryVertex");
    
    if ( ! fPrimaryVertex ) {
      cout << "-W CbmTrdPhotonAnalysis::Init: No PrimaryVertex array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR;
    }
    */
   fPrimVertex = (CbmVertex*) ioman->GetObject("PrimaryVertex");
   if (NULL == fPrimVertex) {   cout << "-W CbmTrdPhotonAnalysis::Init: No PrimaryVertex array!" << endl;
      cout << "                            Task will be inactive" << endl;
      //return kERROR;
 }


    fKFFitter->Init();
    /*
      TString richANNFile(gSystem->Getenv("VMCWORKDIR"));
      richANNFile += "/parameters/rich/el_id_ann_weights_rich_compact.txt";
      fElIdAnn = new CbmRichElectronIdAnn(richANNFile.Data());
      fElIdAnn->Init();
    */
   
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
  fHistoMap["MC_EventCounter"]->Fill(0);
  CbmMCTrack* mctrack = NULL;
  CbmMCTrack* grandmother = NULL;
  CbmMCTrack* mother = NULL;
  CbmMCTrack* daughter = NULL;
  CbmMCTrack* etrack = NULL;
  CbmMCTrack* ptrack = NULL;
  CbmMCTrack* gtrack1 = NULL;
  CbmMCTrack* gtrack2 = NULL;

  const Double_t minDistance = 1e-4;

  CbmTrdTrack* track = NULL;
  CbmGlobalTrack* gtrack = NULL;

  CbmTrdHit* trdHit = NULL;
  /*
    CbmTrdPoint *mcpt = NULL;
    CbmTrdDigi *digi = NULL;
    CbmTrdCluster* cluster = NULL;
    CbmTrdHit* hit = NULL;
 
    CbmVertex* pvertex = NULL;
  */

  Int_t nMcTracks(0), nMvdHits(0), nStsTracks(0), nStsHits(0), nStsTrackMatches(0), nRichPoints(0), nRichRings(0), nRichProj(0), nRichHits(0), nRichRingMatches(0), nTrdPoints(0), nTrdDigis(0), nTrdClusters(0), nTrdHits(0), nTrdTracks(0), nTrdTrackMatches(0), nTofPoints(0), nTofHits(0), nGlobalTracks(0), nPrimaryVertex(0);


  Int_t motherId = 0;
  Int_t grandmotherId = 0;

  std::map<Int_t, MCParticle*> fMCParticleMap;
  std::map<Int_t, MCParticle*>::iterator it;

  std::vector<CbmMCTrack*> fGammaFromPairs;
  std::vector<Int_t> fElectronPositronIds;
  std::vector<Int_t> fElectronIds;
  std::vector<Int_t> fPositronIds;
  std::vector<Int_t> fGammaIds;
  std::vector<Int_t> fPi0Ids;
  Int_t nPi0_global(0), nPi0_inMagnet(0), nPi0_inTarget(0);

  std::vector<std::vector<Int_t> > dalizTriplets;
  std::vector<std::pair<Int_t,Int_t> > electronPositronPairs;
  std::vector<std::pair<Int_t,Int_t> > gammaGammaPairs;
 
  std::vector<CbmMCTrack *> gammaFromEPPairs;
  std::vector<CbmMCTrack *> gammaFromEPPairsInMagnetBackground;
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
  if (fTrdTrackMatches)
    nTrdTrackMatches = fTrdTrackMatches->GetEntries();
  if (fRichRings)
    nRichRings = fRichRings->GetEntries();
  if (fRichProj)
    nRichProj = fRichProj->GetEntries();
  if (fRichPoints)
    nRichPoints = fRichPoints->GetEntries();
  if (fRichRingMatches)
    nRichRingMatches = fRichRingMatches->GetEntries();
  if (fRichHits)
    nRichHits = fRichHits->GetEntries();
  if (fStsTracks)
    nStsTracks = fStsTracks->GetEntries();
  if (fStsTrackMatches)
    nStsTrackMatches = fStsTrackMatches->GetEntries();
  if (fStsHits)
    nStsHits = fStsHits->GetEntries();
  if (fMvdHits)
    nMvdHits = fMvdHits->GetEntries();
  if (fTofHits)
    nTofHits = fTofHits->GetEntries();
  if (fTofPoints)
    nTofPoints = fTofPoints->GetEntries();  
  /*
    if (fPrimaryVertex)
    nPrimaryVertex = fPrimaryVertex->GetEntries();
  */

  printf("\n MC Tracks:        %i\n MVD Hits:         %i\n STS Tracks:       %i\n STS Hits:         %i\n STS Track Match:  %i\n Rich Points:      %i\n Rich Rings:       %i\n Rich Proj:        %i\n Rich Hits:        %i\n Rich Ring Match:  %i\n MC TRD Points:    %i\n TRD Digis:        %i\n TRD Cluster:      %i\n TRD Hits:         %i\n TRD Tracks:       %i\n TRD Track Match:  %i\n TOF Points        %i\n TOF HIts          %i\n Global Tracks:    %i\n Primary Verticis: %i\n", nMcTracks, nMvdHits, nStsTracks, nStsHits, nStsTrackMatches, nRichPoints, nRichRings, nRichProj, nRichHits, nRichRingMatches, nTrdPoints, nTrdDigis, nTrdClusters, nTrdHits, nTrdTracks, nTrdTrackMatches, nTofPoints, nTofHits, nGlobalTracks, nPrimaryVertex);
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //Overview histograms and filling vectors for all electrons, positrons, gamma, and pi0
  printf("Searching electrons, positrons and photons\n");
  
  for (Int_t iMcTrack = 0; iMcTrack < nMcTracks; iMcTrack++) {
    Statusbar(iMcTrack,nMcTracks);
    mctrack = (CbmMCTrack*) fMCTracks->At(iMcTrack);
    motherId = mctrack->GetMotherId();
    if (motherId != -1)
      mother = (CbmMCTrack*)fMCTracks->At(motherId);
   
    fHistoMap["MCPid_global"]->Fill(PdgToGeant(mctrack->GetPdgCode()));
    fHistoMap["Pt_global"]->Fill(mctrack->GetPt(), PdgToGeant(mctrack->GetPdgCode()));
    fHistoMap["P_global"]->Fill(mctrack->GetP(),  PdgToGeant(mctrack->GetPdgCode()));
    fHistoMap["ZBirthAll"]->Fill(mctrack->GetStartZ(),PdgToGeant(mctrack->GetPdgCode()));
    if (VertexInMagnet(mctrack)){
      fHistoMap["MCPid_inMagnet"]->Fill(PdgToGeant(mctrack->GetPdgCode()));
      fHistoMap["Pt_inMagnet"]->Fill(mctrack->GetPt(), PdgToGeant(mctrack->GetPdgCode()));
      fHistoMap["P_inMagnet"]->Fill(mctrack->GetP(),  PdgToGeant(mctrack->GetPdgCode()));
    }
    if(VertexInTarget(mctrack)){
      fHistoMap["MCPid_inTarget"]->Fill(PdgToGeant(mctrack->GetPdgCode()));
      fHistoMap["Pt_inTarget"]->Fill(mctrack->GetPt(), PdgToGeant(mctrack->GetPdgCode()));
      fHistoMap["P_inTarget"]->Fill(mctrack->GetP(),  PdgToGeant(mctrack->GetPdgCode()));    
    }
    if (mctrack->GetPdgCode() ==  22) { // Gamma
      //fZBirth[0]"]->Fill(mctrack->GetStartZ());
      fBirthGamma->Fill(mctrack->GetStartX(), mctrack->GetStartY(), mctrack->GetStartZ());
      if (motherId != -1)
	fHistoMap["gammaMother"]->Fill(PdgToGeant(mother->GetPdgCode()));
      else
	fHistoMap["gammaMother"]->Fill(49);
      if (VertexInMagnet(mctrack)) 
	fGammaIds.push_back(iMcTrack);
    }
    if (mctrack->GetPdgCode() == -11) { // Positron
      //fZBirth[1]"]->Fill(mctrack->GetStartZ());
      if (VertexInMagnet(mctrack)) 
	fPositronIds.push_back(iMcTrack);
      else
	fElectronPositronIds.push_back(iMcTrack);
    }
    if (mctrack->GetPdgCode() ==  11) { // Electron
      //fZBirth[2]"]->Fill(mctrack->GetStartZ());
      if (VertexInMagnet(mctrack)) 
	fElectronIds.push_back(iMcTrack);
      else
	fElectronPositronIds.push_back(iMcTrack);
    }
    if (mctrack->GetPdgCode() == 111) { //Pi^0
      //fZBirth[3]"]->Fill(mctrack->GetStartZ());
      fBirthPi0->Fill(mctrack->GetStartX(), mctrack->GetStartY(), mctrack->GetStartZ());
      if (VertexInMagnet(mctrack)) 
	fPi0Ids.push_back(iMcTrack);

    }

    
    if (motherId >= 0) {    
      mother = (CbmMCTrack*)fMCTracks->At(motherId);
      grandmotherId = mother->GetMotherId();
      if (grandmotherId != -1) {
	grandmother = (CbmMCTrack*)fMCTracks->At(grandmotherId);
	if (grandmother->GetPdgCode() == 111 || mother->GetPdgCode() == 111) { // pi0
	  nPi0_global++;
	  if (VertexInMagnet(mctrack))
	    nPi0_inMagnet++;
	  if(VertexInTarget(mctrack)){
	    nPi0_inTarget++;
	    if (mctrack->GetPdgCode() == -11 ) {// count each pi0 by only one single daughter which converted within target volume
	      fHistoMap["EPPairFromPi0DetectionEfficiency"]->Fill(1); //not Dalitz pi0
	      fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Fill(1);
	    }
	  }
	}
	if (mctrack->GetPdgCode() ==  22){
	  fHistoMap["motherGrani_gamma_global"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(grandmother->GetPdgCode()));
	  if (VertexInMagnet(mctrack))
	    fHistoMap["motherGrani_gamma_inMagnet"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(grandmother->GetPdgCode()));
	  if(VertexInTarget(mctrack))
	    fHistoMap["motherGrani_gamma_inTarget"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(grandmother->GetPdgCode()));
	}
	if (mctrack->GetPdgCode() == -11){
	  fHistoMap["motherGrani_posi_global"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(grandmother->GetPdgCode()));
	  if (VertexInMagnet(mctrack))
	    fHistoMap["motherGrani_posi_inMagnet"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(grandmother->GetPdgCode()));
	  if(VertexInTarget(mctrack))
	    fHistoMap["motherGrani_posi_inTarget"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(grandmother->GetPdgCode()));
	}
	if (mctrack->GetPdgCode() ==  11){
	  fHistoMap["motherGrani_elec_global"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(grandmother->GetPdgCode()));
	  if (VertexInMagnet(mctrack))
	    fHistoMap["motherGrani_elec_inMagnet"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(grandmother->GetPdgCode()));
	  if(VertexInTarget(mctrack))
	    fHistoMap["motherGrani_elec_inTarget"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(grandmother->GetPdgCode()));
	}
      }
      else {
	if (mother->GetPdgCode() == 111) {
	  if(VertexInTarget(mctrack))
	    if ( mctrack->GetPdgCode() == -11 ) {// count each pi0 by only one single daughter which converted within target volume
	      fHistoMap["EPPairFromPi0DetectionEfficiency"]->Fill(6); //Dalitz pi0
	      fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Fill(6);
	    }
	}
	if (mctrack->GetPdgCode() ==  22){
	  fHistoMap["motherGrani_gamma_global"]->Fill(PdgToGeant(mother->GetPdgCode()), 49);
	  if (VertexInMagnet(mctrack))
	    fHistoMap["motherGrani_gamma_inMagnet"]->Fill(PdgToGeant(mother->GetPdgCode()), 49);
	  if(VertexInTarget(mctrack))
	    fHistoMap["motherGrani_gamma_inTarget"]->Fill(PdgToGeant(mother->GetPdgCode()), 49);
	}
	if (mctrack->GetPdgCode() == -11){
	  fHistoMap["motherGrani_posi_global"]->Fill(PdgToGeant(mother->GetPdgCode()), 49);
	  if (VertexInMagnet(mctrack))
	    fHistoMap["motherGrani_posi_inMagnet"]->Fill(PdgToGeant(mother->GetPdgCode()), 49);
	  if(VertexInTarget(mctrack))
	    fHistoMap["motherGrani_posi_inTarget"]->Fill(PdgToGeant(mother->GetPdgCode()), 49);
	}
	if (mctrack->GetPdgCode() ==  11){
	  fHistoMap["motherGrani_elec_global"]->Fill(PdgToGeant(mother->GetPdgCode()), 49);
	  if (VertexInMagnet(mctrack))
	    fHistoMap["motherGrani_elec_inMagnet"]->Fill(PdgToGeant(mother->GetPdgCode()), 49);
	  if(VertexInTarget(mctrack))
	    fHistoMap["motherGrani_elec_inTarget"]->Fill(PdgToGeant(mother->GetPdgCode()), 49);
	}

      }
  

      fHistoMap["MotherDaughter_global"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(mctrack->GetPdgCode()));
   
      if (VertexInMagnet(mctrack))
	fHistoMap["MotherDaughter_inMagnet"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(mctrack->GetPdgCode()));
      if(VertexInTarget(mctrack))
	fHistoMap["MotherDaughter_inTarget"]->Fill(PdgToGeant(mother->GetPdgCode()), PdgToGeant(mctrack->GetPdgCode()));
     
      if (mother->GetPdgCode() == 22)
	fHistoMap["gammaDaughter"]->Fill(PdgToGeant(mctrack->GetPdgCode()));
      if (mctrack->GetPdgCode() ==  11 || mctrack->GetPdgCode() == -11) 
	fHistoMap["ePlusMinusMother"]->Fill(PdgToGeant(mother->GetPdgCode()));
			
    }
    else {
      fHistoMap["MotherDaughter_global"]->Fill(49,PdgToGeant(mctrack->GetPdgCode()));
      /*
	if (VertexInMagnet(mother))
	fMotherDaughter_inMagnet"]->Fill(49, PdgToGeant(mctrack->GetPdgCode()));
	if(VertexInTarget(mother))
	fMotherDaughter_inTarget"]->Fill(49, PdgToGeant(mctrack->GetPdgCode()));
      */
      if (mctrack->GetPdgCode() ==  22){
	fHistoMap["motherGrani_gamma_global"]->Fill(49, 49);
	if (VertexInMagnet(mctrack))
	  fHistoMap["motherGrani_gamma_inMagnet"]->Fill(49, 49);
	if(VertexInTarget(mctrack))
	  fHistoMap["motherGrani_gamma_inTarget"]->Fill(49, 49);
      }
      if (mctrack->GetPdgCode() == -11){
	fHistoMap["motherGrani_posi_global"]->Fill(49, 49);
	if (VertexInMagnet(mctrack))
	  fHistoMap["motherGrani_posi_inMagnet"]->Fill(49, 49);
	if(VertexInTarget(mctrack))
	  fHistoMap["motherGrani_posi_inTarget"]->Fill(49, 49);
      }
      if (mctrack->GetPdgCode() ==  11){
	fHistoMap["motherGrani_elec_global"]->Fill(49, 49);
	if (VertexInMagnet(mctrack))
	  fHistoMap["motherGrani_elec_inMagnet"]->Fill(49, 49);
	if(VertexInTarget(mctrack))
	  fHistoMap["motherGrani_elec_inTarget"]->Fill(49, 49);
      }
 
    }

    if (fMCParticleMap.find(iMcTrack) == fMCParticleMap.end()) {
      MCParticle* p = new MCParticle;
      fMCParticleMap[iMcTrack] = p;
    }
    fMCParticleMap[iMcTrack]->motherId = mctrack->GetMotherId();
    fMCParticleMap[iMcTrack]->PID = mctrack->GetPdgCode();
    if (mctrack->GetMotherId() >= 0) {
      if (fMCParticleMap.find(mctrack->GetMotherId()) == fMCParticleMap.end()) {
	//mother = (CbmMCTrack*)fMCTracks->At(mctrack->GetMotherId());
	MCParticle* p = new MCParticle;
	fMCParticleMap[mctrack->GetMotherId()] = p;
	//fMCParticleMap[mctrack->GetMotherId()]->motherPid = mother->GetPdgCode();
      }
      fMCParticleMap[mctrack->GetMotherId()]->daughterIds.push_back(iMcTrack);
      //fMCParticleMap[mctrack->GetMotherId()]->daughterPids.push_back(mctrack->GetPdgCode());
    }
  }
  printf ("\n--- Found\n\
---      %7i Photons   (in magnet)\n\
---      %7i Electrons (in magnet)\n\
---      %7i Positrons (in magnet)\n\
---      %7i Electrons + Positrons (outside magnet)\n\n", 
	  (Int_t)fGammaIds.size(), 
	  (Int_t)fElectronIds.size(), 
	  (Int_t)fPositronIds.size(),
	  (Int_t)fElectronPositronIds.size()
	  );
 
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  //=========================================================================
  // Find Particle of Interest Candidates

  for (Int_t ipi0 = 0; ipi0 < fPi0Ids.size(); ipi0++){
    if(fMCParticleMap[fPi0Ids[ipi0]]->daughterIds.size() > 1) {
      for (Int_t idaughter = 0; idaughter < fMCParticleMap[fPi0Ids[ipi0]]->daughterIds.size(); idaughter++){
	daughter = (CbmMCTrack*) fMCTracks->At(fMCParticleMap[fPi0Ids[ipi0]]->daughterIds[idaughter]);
	if (VertexInMagnet(daughter)) {
	  fHistoMap["pi02GammaVertex_x"]->Fill(daughter->GetStartX());
	  fHistoMap["pi02GammaVertex_y"]->Fill(daughter->GetStartY());
	  fHistoMap["pi02GammaVertex_z"]->Fill(daughter->GetStartZ());
	}
      }
    }
  }
  for (Int_t igamma = 0; igamma < fGammaIds.size(); igamma++) {
    if (fMCParticleMap[fGammaIds[igamma]]->daughterIds.size() > 1) {
      for (Int_t idaughter = 0; idaughter < fMCParticleMap[fGammaIds[igamma]]->daughterIds.size(); idaughter++){
	daughter = (CbmMCTrack*) fMCTracks->At(fMCParticleMap[fGammaIds[igamma]]->daughterIds[idaughter]);
	if (VertexInMagnet(daughter)) {
	  fHistoMap["gamma2PairVertex_x"]->Fill(daughter->GetStartX());
	  fHistoMap["gamma2PairVertex_y"]->Fill(daughter->GetStartY());
	  fHistoMap["gamma2PairVertex_z"]->Fill(daughter->GetStartZ());
	}
      }
    }
  }
  /*
  // Find Electron Positron Pairs
  printf("Searching electron positron pairs\n");
  Int_t nElPos = fElectronPositronIds.size();
  for (Int_t iElPos = 0; iElPos < nElPos; iElPos++) {
  Statusbar(iElPos,nElPos);
  for (Int_t jElPos = iElPos+1; jElPos < nElPos; jElPos++) {
  etrack = (CbmMCTrack*)fMCTracks->At(fElectronPositronIds[iElPos]);
  ptrack = (CbmMCTrack*)fMCTracks->At(fElectronPositronIds[jElPos]);
  if (CloseByVertex(etrack, ptrack, minDistance)) {
  if (CalcOpeningAngle(etrack, ptrack) < 1.0){
  CbmMCTrack *gamma = new CbmMCTrack(22, -1, 
  etrack->GetPx() + ptrack->GetPx(),
  etrack->GetPy() + ptrack->GetPy(),
  etrack->GetPz() + ptrack->GetPz(),
  0,
  0,
  0,
  0,//invariantMass,
  0);

  gammaFromEPPairs.push_back(gamma);
  }
  }
  }
  }

  */
  Int_t nElectrons = fElectronIds.size();
  Int_t nPositrons = fPositronIds.size();
  for (Int_t iElectron = 0; iElectron < nElectrons; iElectron++) {
    Statusbar(iElectron,nElectrons);
    for (Int_t iPositron = 0; iPositron < nPositrons; iPositron++) {
      etrack = (CbmMCTrack*)fMCTracks->At(fElectronIds[iElectron]);
      ptrack = (CbmMCTrack*)fMCTracks->At(fPositronIds[iPositron]);
 

      Double_t vertexDistance = CalcVertexDistance(etrack, ptrack);
      Double_t openingAngle = CalcOpeningAngle(etrack, ptrack);
      Double_t invariantMass = CalcInvariantMass(etrack, ptrack);
      Double_t pt = CalcPt(etrack, ptrack);

      fHistoMap["EPPairVertexDistance_global"]->Fill(vertexDistance);
      if (VertexInMagnet(etrack) && VertexInMagnet(ptrack))
	fHistoMap["EPPairVertexDistance_inMagnet"]->Fill(vertexDistance);
      if (VertexInTarget(etrack) && VertexInTarget(ptrack))
	fHistoMap["EPPairVertexDistance_inTarget"]->Fill(vertexDistance);



      fHistoMap["EPPairOpeningAngle"]->Fill(openingAngle);
      fHistoMap["InvMassSpectrumAllEPPairs"]->Fill(invariantMass);
      fHistoMap["InvMPairMother"]->Fill(invariantMass, PdgToGeant(fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID));
      fHistoMap["PtPairMother"]->Fill(pt, PdgToGeant(fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID));
      

      fHistoMap["PPairMother"]->Fill(TMath::Sqrt(pow(etrack->GetPx() + ptrack->GetPx(),2) + pow(etrack->GetPy() + ptrack->GetPy(),2) + pow(etrack->GetPz() + ptrack->GetPz(),2)), PdgToGeant(fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID));
      fHistoMap["OpenAnglePairMother"]->Fill(openingAngle, PdgToGeant(fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID));

      fHistoMap["InvMPairMother"]->Fill(invariantMass, PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));
      fHistoMap["PtPairMother"]->Fill(pt, PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));
      fHistoMap["PPairMother"]->Fill(TMath::Sqrt(pow(etrack->GetPx() + ptrack->GetPx(),2) + pow(etrack->GetPy() + ptrack->GetPy(),2) + pow(etrack->GetPz() + ptrack->GetPz(),2)), PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));
      fHistoMap["OpenAnglePairMother"]->Fill(openingAngle, PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));

      if ((fMCParticleMap[fPositronIds[iPositron]]->motherId == fMCParticleMap[fElectronIds[iElectron]]->motherId) && fMCParticleMap[fElectronIds[iElectron]]->motherId >= 0) {
	fHistoMap["InvMPairSameMother"]->Fill(invariantMass, PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));
	fHistoMap["PtPairSameMother"]->Fill(pt, PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));
	fHistoMap["PPairSameMother"]->Fill(TMath::Sqrt(pow(etrack->GetPx() + ptrack->GetPx(),2) + pow(etrack->GetPy() + ptrack->GetPy(),2) + pow(etrack->GetPz() + ptrack->GetPz(),2)), PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));
	fHistoMap["OpenAnglePairSameMother"]->Fill(openingAngle, PdgToGeant(fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID));
      }
      else {
	//fHistoMap["InvMassSpectrumGammaEPPairsInMagnetBackground"]->Fill(invariantMass);
      }

      if (CloseByVertex(etrack, ptrack, minDistance /*cm*/)) {
	std::pair<Int_t,Int_t> epPair (fElectronIds[iElectron], fPositronIds[iPositron]);
	electronPositronPairs.push_back(epPair);
	fHistoMap["Pt_OpenAngle_global"]->Fill(pt, openingAngle);
	/*
	  CbmMCTrack *gamma = new CbmMCTrack(22, -1, 
	  etrack->GetPx() + ptrack->GetPx(),
	  etrack->GetPy() + ptrack->GetPy(),
	  etrack->GetPz() + ptrack->GetPz(),
	  0,
	  0,
	  0,
	  0,//invariantMass,
	  0);
	  gammaFromEPPairs.push_back(gamma);
	*/


	if (VertexInMagnet(etrack) && VertexInMagnet(ptrack)) {
	  CbmMCTrack *gamma = new CbmMCTrack(22, -1, 
					     etrack->GetPx() + ptrack->GetPx(),
					     etrack->GetPy() + ptrack->GetPy(),
					     etrack->GetPz() + ptrack->GetPz(),
					     0,
					     0,
					     0,
					     0,//invariantMass,
					     0);

	  gammaFromEPPairsInMagnet.push_back(gamma);
	  fHistoMap["EPPairOpeningAngleInMagnet"]->Fill(openingAngle);
	  fHistoMap["Pt_OpenAngle_inMagnet"]->Fill(pt, openingAngle);
	  //if (fMCParticleMap[fElectronIds[iElectron]]->motherId != fMCParticleMap[fPositronIds[iPositron]]->motherId) 
	  //gammaFromEPPairsInMagnetBackground.push_back(gamma);
	  
	  fHistoMap["InvMassSpectrumEPPairsInMagnet"]->Fill(invariantMass);
	  if(VertexInTarget(etrack) && VertexInTarget(ptrack)) {
	    gammaFromEPPairsInTarget.push_back(gamma);
	    fHistoMap["EPPairOpeningAngleInTarget"]->Fill(openingAngle);
	    fHistoMap["Pt_OpenAngle_inTarget"]->Fill(pt, openingAngle);
	    fHistoMap["InvMassSpectrumEPPairsInTarget"]->Fill(invariantMass);	

	    if (fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID == 22  && fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID == 22) {
	      fHistoMap["EPPairOpeningAngleGamma"]->Fill(openingAngle);
	      fHistoMap["InvMassSpectrumGammaEPPairsGamma"]->Fill(invariantMass);
	      fHistoMap["Pt_OpenAngle_inTarget_EPPairsGamma"]->Fill(pt, openingAngle);
	    }
	    if (fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID == 111 && fMCParticleMap[fMCParticleMap[fPositronIds[iPositron]]->motherId]->PID == 111) {
	      fHistoMap["EPPairOpeningAnglePi0"]->Fill(openingAngle);
	      fHistoMap["InvMassSpectrumGammaEPPairsPi0"]->Fill(invariantMass);
	      fHistoMap["Pt_OpenAngle_inTarget_EPPairsPi0"]->Fill(pt, openingAngle);
	    }
	    
	    if (openingAngle < 1.0){
	      gammaFromEPPairsOpeningAngle.push_back(gamma);
	      
	      if ((fMCParticleMap[fElectronIds[iElectron]]->motherId == fMCParticleMap[fPositronIds[iPositron]]->motherId) && fMCParticleMap[fElectronIds[iElectron]]->motherId >= 0) {
		gammaFromTrueEPPairs.push_back(gamma);
		fHistoMap["EPPairOpeningAngleSameMother"]->Fill(openingAngle);
		fHistoMap["InvMassSpectrumTrueEPPairs"]->Fill(invariantMass);
		
		//epPair = make_pair (fElectronIds[iElectron], fPositronIds[iPositron]);
		
		fHistoMap["ePlusAndMinusMother"]->Fill(PdgToGeant(fMCParticleMap[fMCParticleMap[fElectronIds[iElectron]]->motherId]->PID));
		
	      }
	      else
		cout << fMCParticleMap[fElectronIds[iElectron]]->motherId << " " << fMCParticleMap[fPositronIds[iPositron]]->motherId << endl;
	    }
	  }
	}
      }
      else {
	/*
	  if (fMCParticleMap[fElectronIds[iElectron]]->motherId != fMCParticleMap[fPositronIds[iPositron]]->motherId)
	  if (VertexInMagnet(etrack) && VertexInMagnet(ptrack)) {
	  CbmMCTrack *gamma = new CbmMCTrack(22, -1, 
	  etrack->GetPx() + ptrack->GetPx(),
	  etrack->GetPy() + ptrack->GetPy(),
	  etrack->GetPz() + ptrack->GetPz(),
	  0,
	  0,
	  0,
	  0,//invariantMass,
	  0);
	  gammaFromEPPairsInMagnetBackground.push_back(gamma);
	
	  }
	*/
      }
    }
  }

  printf (
	  "\n--- Found\n\
---      %7i Photons   (global)\n\
---      %7i Electrons (global)\n\
---      %7i Positrons (global)\n\
---      %7i Electron & Positron Pairs (global after vertex distance cut)\n\
---      %7i Electron & Positron Pairs (outside magnet volume)\n\
---      %7i Electron & Positron Pairs (in magnet volume)\n\
---      %7i Electron & Positron Pairs (in magnet volume) background\n\
---      %7i Electron & Positron Pairs (in target volume)\n\
---      %7i Electron & Positron Pairs (opening angle cut)\n\
---      %7i Electron & Positron Pairs (same mother id)\n\
---      %7i Pi0 (MC global)\n\
---      %7i particle with grandmother or mother Pi0 (global)\n\
---      %7i particle with grandmother or mother Pi0 (in magnet volume)\n\
---      %7i particle with grandmother or mother Pi0 (in target volume)\n\n", 
	  (Int_t)fGammaIds.size(), 
	  (Int_t)fElectronIds.size(), 
	  (Int_t)fPositronIds.size(), 
	  (Int_t)electronPositronPairs.size(),
	  (Int_t)gammaFromEPPairs.size(),
	  (Int_t)gammaFromEPPairsInMagnet.size(),
	  (Int_t)gammaFromEPPairsInMagnetBackground.size(),
	  (Int_t)gammaFromEPPairsInTarget.size(),
	  (Int_t)gammaFromEPPairsOpeningAngle.size(),
	  (Int_t)gammaFromTrueEPPairs.size(),
	  (Int_t)fPi0Ids.size(),
	  nPi0_global,
	  nPi0_inMagnet,
	  nPi0_inTarget
	  );
  printf("fInvMassSpectrumGammaEPPairsOpenAngle\n");
  Int_t nGamma = gammaFromEPPairsOpeningAngle.size();
  Double_t invMass(0.0), openingAngle(-1.0), pT(0.0), p(0.0), vertexDistance(0.0);
  for (Int_t iGamma = 0; iGamma < nGamma-1; iGamma++) {
    Statusbar(iGamma,nGamma);
    for (Int_t jGamma = iGamma + 1; jGamma < nGamma; jGamma++) {
      //cout << iGamma << " " << jGamma << endl;
      if (CloseByVertex(gammaFromEPPairsOpeningAngle[iGamma], gammaFromEPPairsOpeningAngle[jGamma], minDistance /*cm*/)) // up to now now vertex calculated
	invMass = CalcInvariantMass(gammaFromEPPairsOpeningAngle[iGamma], gammaFromEPPairsOpeningAngle[jGamma]);
      fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"]->Fill(invMass);
    }
  }
  /*
    printf("fInvMassSpectrumGammaEPPairsInMagnetBackground\n");
    nGamma = gammaFromEPPairsInMagnetBackground.size() * 1e-2;
    for (Int_t iGamma = 0; iGamma < nGamma; iGamma++) {
    Statusbar(iGamma,nGamma);
    for (Int_t jGamma = iGamma + 1; jGamma < nGamma; jGamma++) {
    //cout << iGamma << " " << jGamma << endl;
    if (CloseByVertex(gammaFromEPPairsInMagnetBackground[iGamma], gammaFromEPPairsInMagnetBackground[jGamma], minDistance))
    fInvMassSpectrumGammaEPPairsInMagnetBackground"]->Fill(CalcInvariantMass(gammaFromEPPairsInMagnetBackground[iGamma], gammaFromEPPairsInMagnetBackground[jGamma]));
    }
    }
  */
  printf("fInvMassSpectrumGammaEPPairsInTarget\n");
  nGamma = gammaFromEPPairsInTarget.size();
  for (Int_t iGamma = 0; iGamma < nGamma-1; iGamma++) {
    Statusbar(iGamma,nGamma);
    for (Int_t jGamma = iGamma + 1; jGamma < nGamma; jGamma++) {
      //cout << iGamma << " " << jGamma << endl;
      if (CloseByVertex(gammaFromEPPairsInTarget[iGamma], gammaFromEPPairsInTarget[jGamma], minDistance /*cm*/)){ // up to now now vertex calculated
	invMass = CalcInvariantMass(gammaFromEPPairsInTarget[iGamma], gammaFromEPPairsInTarget[jGamma]);
	pT = CalcPt(gammaFromEPPairsInTarget[iGamma], gammaFromEPPairsInTarget[jGamma]);
	p = CalcP(gammaFromEPPairsInTarget[iGamma], gammaFromEPPairsInTarget[jGamma]);
	vertexDistance = CalcVertexDistance(gammaFromEPPairsInTarget[iGamma], gammaFromEPPairsInTarget[jGamma]);
	openingAngle = CalcOpeningAngle(gammaFromEPPairsInTarget[iGamma], gammaFromEPPairsInTarget[jGamma]);
	fHistoMap["InvMassSpectrumGammaEPPairsInTarget"]->Fill(invMass);
	fHistoMap["InvMassSpectrumGammaEPPairsInTargetPt"]->Fill(invMass, pT);
	fHistoMap["InvMassSpectrumGammaEPPairsInTargetP"]->Fill(invMass, p);
	fHistoMap["InvMassSpectrumGammaEPPairsInTargetVD"]->Fill(invMass, vertexDistance);
	fHistoMap["InvMassSpectrumGammaEPPairsInTargetOA"]->Fill(invMass, openingAngle);
      }
    }
  }
  printf("fInvMassSpectrumGammaEPPairsInMagnet\n");
  nGamma = gammaFromEPPairsInMagnet.size();
  for (Int_t iGamma = 0; iGamma < nGamma-1; iGamma++) {
    Statusbar(iGamma,nGamma);
    for (Int_t jGamma = iGamma + 1; jGamma < nGamma; jGamma++) {
      //cout << iGamma << " " << jGamma << endl;
      if (CloseByVertex(gammaFromEPPairsInMagnet[iGamma], gammaFromEPPairsInMagnet[jGamma], minDistance /*cm*/)){ // up to now now vertex calculated
	invMass = CalcInvariantMass(gammaFromEPPairsInMagnet[iGamma], gammaFromEPPairsInMagnet[jGamma]);
	pT = CalcPt(gammaFromEPPairsInMagnet[iGamma], gammaFromEPPairsInMagnet[jGamma]);
	p = CalcP(gammaFromEPPairsInMagnet[iGamma], gammaFromEPPairsInMagnet[jGamma]);
	vertexDistance = CalcVertexDistance(gammaFromEPPairsInMagnet[iGamma], gammaFromEPPairsInMagnet[jGamma]);
	openingAngle = CalcOpeningAngle(gammaFromEPPairsInMagnet[iGamma], gammaFromEPPairsInMagnet[jGamma]);
	fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"]->Fill(invMass);
	fHistoMap["InvMassSpectrumGammaEPPairsInMagnetPt"]->Fill(invMass, pT);
	fHistoMap["InvMassSpectrumGammaEPPairsInMagnetP"]->Fill(invMass, p);
	fHistoMap["InvMassSpectrumGammaEPPairsInMagnetVD"]->Fill(invMass, vertexDistance);
	fHistoMap["InvMassSpectrumGammaEPPairsInMagnetOA"]->Fill(invMass, openingAngle);
	fHistoMap["Pt_OpenAngle_inMagnet"]->Fill(pT, openingAngle);
      }
      else {
	invMass = CalcInvariantMass(gammaFromEPPairsInMagnet[iGamma], gammaFromEPPairsInMagnet[jGamma]);
	fHistoMap["InvMassSpectrumGammaEPPairsInMagnetBackground"]->Fill(invMass);
      }
    }
  }
  
  printf("fInvMassSpectrumGammaEPPairs\n");
  nGamma = gammaFromEPPairs.size();
  for (Int_t iGamma = 0; iGamma < nGamma-1; iGamma++) {
    Statusbar(iGamma,nGamma);
    for (Int_t jGamma = iGamma + 1; jGamma < nGamma; jGamma++) {
      //cout << iGamma << " " << jGamma << endl;
      if (CloseByVertex(gammaFromEPPairs[iGamma], gammaFromEPPairs[jGamma], minDistance /*cm*/)){ // up to now now vertex calculated
	invMass = CalcInvariantMass(gammaFromEPPairs[iGamma], gammaFromEPPairs[jGamma]);
	pT = CalcPt(gammaFromEPPairs[iGamma], gammaFromEPPairs[jGamma]);
	p = CalcP(gammaFromEPPairs[iGamma], gammaFromEPPairs[jGamma]);
	vertexDistance = CalcVertexDistance(gammaFromEPPairs[iGamma], gammaFromEPPairs[jGamma]);
	openingAngle = CalcOpeningAngle(gammaFromEPPairs[iGamma], gammaFromEPPairs[jGamma]);
	fHistoMap["InvMassSpectrumGammaEPPairs"]->Fill(invMass);
	fHistoMap["InvMassSpectrumGammaEPPairsPt"]->Fill(invMass, pT);
	fHistoMap["InvMassSpectrumGammaEPPairsP"]->Fill(invMass, p);
	fHistoMap["InvMassSpectrumGammaEPPairsVD"]->Fill(invMass, vertexDistance);
	fHistoMap["InvMassSpectrumGammaEPPairsOA"]->Fill(invMass, openingAngle);
	fHistoMap["Pt_OpenAngle_global"]->Fill(pT, openingAngle);
	//fInvMassSpectrumGammaEPPairs"]->Fill(gammaFromEPPairs[iGamma]->GetStartT() + gammaFromEPPairs[jGamma]->GetStartT());
      }
    }
  }
  
  printf("fInvMassSpectrumGammaAllPairs\n");
  // Find Gamma Gamma Pairs
  nGamma = fGammaIds.size();
  for (Int_t iGamma = 0; iGamma < nGamma-1; iGamma++) {
    Statusbar(iGamma,nGamma);
    gtrack1 = (CbmMCTrack*)fMCTracks->At(fGammaIds[iGamma]);
    for (Int_t jGamma = iGamma+1; jGamma < nGamma; jGamma++) {      
      gtrack2 = (CbmMCTrack*)fMCTracks->At(fGammaIds[jGamma]); 
      if (CloseByVertex(gtrack1, gtrack2, minDistance))  
	{
	  fHistoMap["InvMassSpectrumGammaAllPairs"]->Fill(CalcInvariantMass(gtrack1, gtrack2));
	  if ((fMCParticleMap[fGammaIds[iGamma]]->motherId == fMCParticleMap[fGammaIds[jGamma]]->motherId) 
	      && 
	      (fMCParticleMap[fGammaIds[iGamma]]->motherId != -1) // no combinatorial background from primary gammas (ID == -1)
	      ) { 
	    fHistoMap["InvMassSpectrumGammaTruePairs"]->Fill(CalcInvariantMass(gtrack1, gtrack2));
	    std::pair<Int_t,Int_t> ggPair (fGammaIds[iGamma], fGammaIds[jGamma]);
	    //ggPair = make_pair (fGammaIds[iGamma], fGammaIds[jGamma]);
	    gammaGammaPairs.push_back(ggPair);
	    if (fMCParticleMap[fGammaIds[iGamma]]->motherId >= 0)
	      fHistoMap["gammaAndGammaMother"]->Fill(PdgToGeant(fMCParticleMap[fMCParticleMap[fGammaIds[iGamma]]->motherId]->PID));
	  }
	}
    }
  }
  Int_t nPairs = electronPositronPairs.size();
  /*
  // Find Daliz Triplets
  printf("Find Daliz Triplets\n");
  nGamma = fGammaIds.size();
  for (Int_t iGamma = 0; iGamma < nGamma; iGamma++) {
  Statusbar(iGamma,nGamma);
  for (Int_t iPair = 0; iPair < nPairs; iPair++) {
  if (fMCParticleMap[fGammaIds[iGamma]]->motherId == fMCParticleMap[electronPositronPairs[iPair].first]->motherId) {
  std::vector<Int_t> triplet;
  triplet.push_back(electronPositronPairs[iPair].first);
  triplet.push_back(electronPositronPairs[iPair].second);
  triplet.push_back(fGammaIds[iGamma]);
  dalizTriplets.push_back(triplet);
  fDalizMother"]->Fill(PdgToGeant(fMCParticleMap[fMCParticleMap[fGammaIds[iGamma]]->motherId]->PID));
  }
  }
  }
  */
  //=========================================================================
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  //fHistoMap["InvMassSpectrumGammaEPPairsInMagnetBackground"]->Scale(fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"]->GetEntries()/(Float_t)fHistoMap["InvMassSpectrumGammaEPPairsInMagnetBackground"]->GetEntries());

  // Vertizes of real electron positron pairs
  printf("Vertizes of real electron positron pairs\n");
  for (Int_t iPair = 0; iPair < nPairs; iPair++ ) {
    Statusbar(iPair,nPairs);
    etrack = (CbmMCTrack*)fMCTracks->At(electronPositronPairs[iPair].first);
    ptrack = (CbmMCTrack*)fMCTracks->At(electronPositronPairs[iPair].second);

    fHistoMap["PairAllVertex_x"]->Fill(etrack->GetStartX());
    fHistoMap["PairAllVertex_y"]->Fill(etrack->GetStartY());
    fHistoMap["PairAllVertex_z"]->Fill(etrack->GetStartZ());
    fBirthPair->Fill(etrack->GetStartX(), etrack->GetStartY(), etrack->GetStartZ());
    fHistoMap["PairHistory"]->Fill(0);
    if (VertexInMagnet(etrack))
      fHistoMap["PairHistory"]->Fill(3);
    if (VertexInTarget(etrack))
      fHistoMap["PairHistory"]->Fill(6);
    if (fMCParticleMap[fMCParticleMap[electronPositronPairs[iPair].first]->motherId]->PID == 22) { 	   
      fHistoMap["PairGammaVertex_x"]->Fill(etrack->GetStartX());
      fHistoMap["PairGammaVertex_y"]->Fill(etrack->GetStartY());
      fHistoMap["PairGammaVertex_z"]->Fill(etrack->GetStartZ());
      fHistoMap["ZBirthEPfromGamma"]->Fill(etrack->GetStartZ());
      fHistoMap["PairHistory"]->Fill(1);
      if (VertexInMagnet(etrack))
	fHistoMap["PairHistory"]->Fill(4);
      if (VertexInTarget(etrack))
	fHistoMap["PairHistory"]->Fill(7);
    }
    if (fMCParticleMap[fMCParticleMap[electronPositronPairs[iPair].first]->motherId]->PID == 111) { 
      fHistoMap["PairPi0Vertex_x"]->Fill(etrack->GetStartX());
      fHistoMap["PairPi0Vertex_y"]->Fill(etrack->GetStartY());
      fHistoMap["PairPi0Vertex_z"]->Fill(etrack->GetStartZ());
      fHistoMap["PairHistory"]->Fill(2);
      if (VertexInMagnet(etrack))
	fHistoMap["PairHistory"]->Fill(5);
      if (VertexInTarget(etrack))
	fHistoMap["PairHistory"]->Fill(8);
    }
 
  }
  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

  for (it=fMCParticleMap.begin(); it != fMCParticleMap.end(); it++) {
    mctrack = (CbmMCTrack*)fMCTracks->At((*it).first);
    fHistoMap["NoDaughters_global"]->Fill(PdgToGeant(mctrack->GetPdgCode()), (*it).second->daughterIds.size());
    if (VertexInMagnet(mctrack))
      fHistoMap["NoDaughters_inMagnet"]->Fill(PdgToGeant(mctrack->GetPdgCode()), (*it).second->daughterIds.size());
    if(VertexInTarget(mctrack))
      fHistoMap["NoDaughters_inTarget"]->Fill(PdgToGeant(mctrack->GetPdgCode()), (*it).second->daughterIds.size());
  }

  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
  for (Int_t iTrdTrack = 0; iTrdTrack < nTrdTracks; iTrdTrack++) {
    track = (CbmTrdTrack*) fTrdTracks->At(iTrdTrack);
    
    if (track) {
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

      fHistoMap["PidWkn"]->Fill(track->GetPidWkn());
      fHistoMap["PidANN"]->Fill(track->GetPidANN());
      fHistoMap["PidLikeEL"]->Fill(track->GetPidLikeEL());
      fHistoMap["PidLikePI"]->Fill(track->GetPidLikePI());
      fHistoMap["PidLikeKA"]->Fill(track->GetPidLikeKA());
      fHistoMap["PidLikePR"]->Fill(track->GetPidLikePR());
      fHistoMap["PidLikeMU"]->Fill(track->GetPidLikeMU());
    }
    else
      printf("no TRD track found \n");
  }
  std::map<Int_t, std::vector<Int_t> > notDalitzEPMapAll;//< pi0Id, e+e-IDs[]>
  std::map<Int_t, std::vector<Int_t> >::iterator notDalitzEPMapAllIt;
  std::map<Int_t, std::vector<Int_t> > dalitzEPMapAll; //< pi0Id, e+e-IDs[]>
  std::map<Int_t, std::vector<Int_t> >::iterator dalitzEPMapAllIt;
  CbmMCTrack* mctMother = NULL; 
  CbmMCTrack* mctGrani = NULL; 
  for (Int_t iGlobalTrack = 0; iGlobalTrack < nGlobalTracks; iGlobalTrack++) {
    ElectronCandidate cand;
    cand.isMcSignalElectron = false;
    cand.isMcPi0Electron = false;
    cand.isMcGammaElectron = false;
    cand.isMcEtaElectron = false;
    cand.McMotherId = -1;
    cand.isElectron = false;
    cand.isGamma = false;
    cand.isStCutElectron = false;
    cand.isTtCutElectron = false;
    cand.isApmCutElectron = false;
    cand.isMvd1CutElectron = true;
    cand.isMvd2CutElectron = true;
    gtrack = (CbmGlobalTrack*) fGlobalTracks->At(iGlobalTrack);
    fHistoMap["GTPid"]->Fill(gtrack->GetPidHypo());
    // STS
    cand.stsInd = gtrack->GetStsTrackIndex();
    if (cand.stsInd < 0) continue;
    CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(cand.stsInd);
    if (stsTrack == NULL) continue;
    CbmTrackMatch* stsMatch  = (CbmTrackMatch*) fStsTrackMatches->At(cand.stsInd);
    if (stsMatch == NULL) continue;
    cand.stsMcTrackId = stsMatch->GetMCTrackId();
    if (cand.stsMcTrackId < 0) continue;
    CbmMCTrack* mcTrack1 = (CbmMCTrack*) fMCTracks->At(cand.stsMcTrackId);
    if (mcTrack1 == NULL) continue;

     fKFFitter->DoFit(stsTrack,11);
    
    cand.chi2sts = stsTrack->GetChi2() / stsTrack->GetNDF();
    cand.chi2Prim = fKFFitter->GetChiToVertex(stsTrack, fPrimVertex);
    // Fit tracks to the primary vertex
      
    FairTrackParam vtxTrack;
    FairTrackParam fParamFirst = *(gtrack->GetParamFirst());
    //printf("First:  Qp %f  (%f, %f, %f)\n",fParamFirst.GetQp(),fParamFirst.GetX(),fParamFirst.GetY(),fParamFirst.GetZ());
    FairTrackParam fParamLast = *(gtrack->GetParamLast());
    //printf("Last:  Qp %f  (%f, %f, %f)\n",fParamLast.GetQp(),fParamLast.GetX(),fParamLast.GetY(),fParamLast.GetZ());

    fHistoMap["GT_Qp_first"] ->Fill(fParamFirst.GetQp());
    fHistoMap["GT_Qp_last"]->Fill(fParamLast.GetQp());
    fHistoMap["GT_Vertex_x_first"]->Fill(fParamFirst.GetX());
    fHistoMap["GT_Vertex_y_first"]->Fill(fParamFirst.GetY());
    fHistoMap["GT_Vertex_z_first"]->Fill(fParamFirst.GetZ());
    fHistoMap["GT_Vertex_x_last"]->Fill(fParamLast.GetX());
    fHistoMap["GT_Vertex_y_last"]->Fill(fParamLast.GetY());
    fHistoMap["GT_Vertex_z_last"]->Fill(fParamLast.GetZ());
    fKFFitter->FitToVertex(stsTrack, fPrimVertex, &vtxTrack);
    
    vtxTrack.Position(cand.position);
    vtxTrack.Momentum(cand.momentum);

    cand.mass = TDatabasePDG::Instance()->GetParticle(11)->Mass();
    cand.charge = (vtxTrack.GetQp() > 0) ?1 :-1;
    cand.energy = sqrt(cand.momentum.Mag2() + cand.mass * cand.mass);
    cand.rapidity = 0.5*TMath::Log((cand.energy + cand.momentum.Z()) / (cand.energy - cand.momentum.Z()));

    Double_t MCmoment = mcTrack1->GetP();
    Int_t Pdg = TMath::Abs(mcTrack1->GetPdgCode());
    Int_t Pdg1 = mcTrack1->GetPdgCode();
    Int_t MotherId = mcTrack1->GetMotherId();
    Int_t motherPdg = 0;
    cand.McMotherId = MotherId;
    // RICH
    cand.richInd = gtrack->GetRichRingIndex();
    if (cand.richInd < 0) continue;
    CbmRichRing* richRing = (CbmRichRing*) fRichRings->At(cand.richInd);

    CbmTrackMatch* richMatch  = (CbmTrackMatch*) fRichRingMatches->At(cand.richInd);
    if (richMatch == NULL) continue;
    cand.richMcTrackId = richMatch->GetMCTrackId();
    if (cand.richMcTrackId < 0) continue;
    CbmMCTrack* mcTrack2 = (CbmMCTrack*) fMCTracks->At(cand.richMcTrackId);
    if (mcTrack2 == NULL) continue;
    fHistoMap["RICH_GT_radiusA_KF_P"]->Fill(sqrt(pow(cand.momentum[0],2) + pow(cand.momentum[1],2) + pow(cand.momentum[2],2)),richRing->GetAaxis());
    fHistoMap["RICH_GT_radiusB_KF_P"]->Fill(sqrt(pow(cand.momentum[0],2) + pow(cand.momentum[1],2) + pow(cand.momentum[2],2)),richRing->GetBaxis());
    // check RICH ring - STS track matches
    //	   if ( cand.stsMCTrackId == cand.richMCTrackId){
    Int_t pdg = TMath::Abs( mcTrack1->GetPdgCode() );
    motherId = mcTrack1->GetMotherId();
    cand.McMotherId = motherId;
    if (pdg == 11 && motherId == -1) cand.isMcSignalElectron = true;

    if (motherId >=0){
      CbmMCTrack* mct1 = (CbmMCTrack*) fMCTracks->At(cand.McMotherId);
      motherPdg = mct1->GetPdgCode();
      if (mct1 != NULL && motherPdg == 111 && pdg == 11) {
	cand.isMcPi0Electron = true;
      }
      if (mct1 != NULL && motherPdg == 22 && pdg == 11){
	cand.isMcGammaElectron = true;
      }
      if(mct1 != NULL && motherPdg == 221 && pdg == 11){
	cand.isMcEtaElectron = true;
      }
    }
    //	    }
    //        IsRichElectron(richRing, cand.momentum.Mag(), &cand);

    // TRD

    cand.trdInd = gtrack->GetTrdTrackIndex();
    if (cand.trdInd < 0) continue;
    CbmTrdTrack* trdTrack = (CbmTrdTrack*) fTrdTracks->At(cand.trdInd);
    if (trdTrack == NULL) continue;
    CbmTrackMatch* trdMatch = (CbmTrackMatch*) fTrdTrackMatches->At(cand.trdInd);
    if (trdMatch == NULL) continue;
    cand.trdMcTrackId = trdMatch->GetMCTrackId();
    if (cand.trdMcTrackId < 0) continue;
    CbmMCTrack* mcTrack3 = (CbmMCTrack*) fMCTracks->At(cand.trdMcTrackId);
    if (mcTrack3 == NULL) continue;
    //fHistoMap["TRD_GT_dEdx_KF_P"]->Fill(sqrt(pow(cand.momentum[0],2) + pow(cand.momentum[1],2) + pow(cand.momentum[2],2)),trdTrack->GetELoss()); //trdTrack->GetELoss() always == 0
    Int_t nTrdTrackHits = trdTrack->GetNofHits();
    for (Int_t iTrdTrackHit = 0; iTrdTrackHit < nTrdTrackHits; iTrdTrackHit++) {
      if (fTrdHits) {
	trdHit = (CbmTrdHit*)fTrdHits->At(iTrdTrackHit);
	fHistoMap["TRD_GT_dEdx_KF_P"]->Fill(sqrt(pow(cand.momentum[0],2) + pow(cand.momentum[1],2) + pow(cand.momentum[2],2)),Double_t(trdHit->GetELoss()*1e6));//GeV -> keV
	//printf("%f\n",trdHit->GetELoss()*1e6);
      }
    }
  
    //       IsTrdElectron(trdTrack, &cand);


   
    // ToF
    cand.tofInd = gtrack->GetTofHitIndex();
    if (cand.tofInd < 0) continue;
    CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(cand.tofInd);
    if (tofHit == NULL) continue;
    Int_t tofPointIndex = tofHit->GetRefId();
    if (tofPointIndex < 0) continue;
    FairMCPoint* tofPoint = (FairMCPoint*) fTofPoints->At(tofPointIndex);
    if (tofPoint == NULL) continue;
    cand.tofMcTrackId = tofPoint->GetTrackID();
    if (cand.tofMcTrackId < 0) continue;
    CbmMCTrack* mcTrack4 = (CbmMCTrack*) fMCTracks->At(cand.tofMcTrackId);
    if (mcTrack4 == NULL) continue;
    fHistoMap["TOF_GT_time_KF_P"]->Fill(sqrt(pow(cand.momentum[0],2) + pow(cand.momentum[1],2) + pow(cand.momentum[2],2)), ((gtrack->GetLength()/100.) / (tofHit->GetTime()*1e-9) / TMath::C())); //cm -> m; ns -> s
    fHistoMap["GT_MC_PID"]->Fill(PdgToGeant(Pdg1));
    if (IsElec(richRing, cand.momentum.Mag(), trdTrack, gtrack, &cand)) {
      fHistoMap["KF_PID_MC_PID"]->Fill(PdgToGeant(Pdg1), PdgToGeant(-11 * cand.charge));
      fCandidates.push_back(cand);
      if (cand.charge < 0)
	fElectronCandidates.push_back(cand);
      if (cand.charge > 0)
	fPositronCandidates.push_back(cand);
    }
    //else
    //fHistoMap["KF_PID_MC_PID"]->Fill(PdgToGeant(pdg),PdgToGeant(Pdg1));

    fHistoMap["KF_PrimVertex_x"]->Fill(cand.position[0]);
    fHistoMap["KF_PrimVertex_y"]->Fill(cand.position[1]);
    fHistoMap["KF_PrimVertex_z"]->Fill(cand.position[2]);
    fHistoMap["KF_P"]->Fill(sqrt(pow(cand.momentum[0],2) + pow(cand.momentum[1],2) + pow(cand.momentum[2],2)));
    fHistoMap["KF_Pt"]->Fill(sqrt(pow(cand.momentum[0],2) + pow(cand.momentum[1],2)));
    fHistoMap["KF_E"]->Fill(cand.energy);



    fHistoMap["DeltaP"]->Fill(MCmoment,(MCmoment-sqrt(pow(cand.momentum[0],2) + pow(cand.momentum[1],2) + pow(cand.momentum[2],2)))/MCmoment);
    fHistoMap["DeltaPt"]->Fill(mcTrack1->GetPt(),(mcTrack1->GetPt()-sqrt(pow(cand.momentum[0],2) + pow(cand.momentum[1],2)))/mcTrack1->GetPt());
    fHistoMap["DeltaPGT_PKF"]->Fill(MCmoment, fabs(fParamLast.GetQp()) - sqrt(pow(cand.momentum[0],2) + pow(cand.momentum[1],2) + pow(cand.momentum[2],2)));

    //printf("PidHypo:%.2i\n",gtrack->GetPidHypo());
   if (cand.McMotherId > -1) {
      mctMother = (CbmMCTrack*) fMCTracks->At(cand.McMotherId);
      //printf("MPID:%i) ",mctMotherE->GetPdgCode());
      if (mctMother->GetPdgCode() == 111){
	dalitzEPMapAll[cand.McMotherId].push_back(cand.stsMcTrackId);
	//printf("dalitz: %i(%i) ",cand.McMotherId,(Int_t)dalitzEPMap[cand.McMotherId].size());
      }
      else if (mctMother->GetPdgCode() == 22) {
	if (mctMother->GetMotherId() > -1){
	  mctGrani = (CbmMCTrack*) fMCTracks->At(mctMother->GetMotherId());
	  if (mctGrani->GetPdgCode() == 111){
	    notDalitzEPMapAll[mctMother->GetMotherId()].push_back(cand.stsMcTrackId);
	    //printf("not dalitz: %i(%i) ",mctMotherE->GetMotherId(),(Int_t)notDalitzEPMap[mctMotherE->GetMotherId()].size());
	  }
	}
      }
    }
  }


  for (dalitzEPMapAllIt = dalitzEPMapAll.begin(); dalitzEPMapAllIt != dalitzEPMapAll.end(); dalitzEPMapAllIt++) {
    Int_t dalitzDaughterSize = (Int_t)(*dalitzEPMapAllIt).second.size();
    if (dalitzDaughterSize == 1)
      fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Fill(7);
    if (dalitzDaughterSize == 2)
      fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Fill(8);
  }
  for (notDalitzEPMapAllIt = notDalitzEPMapAll.begin(); notDalitzEPMapAllIt != notDalitzEPMapAll.end(); notDalitzEPMapAllIt++) {
    Int_t notDalitzDaughterSize = (Int_t)(*notDalitzEPMapAllIt).second.size();
    if (notDalitzDaughterSize == 1)
      fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Fill(2);
    if (notDalitzDaughterSize == 2)
      fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Fill(3);
    if (notDalitzDaughterSize == 3)
      fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Fill(4);
    if (notDalitzDaughterSize == 4)
      fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Fill(5);
  }
 
  std::vector<CbmMCTrack *> gammaFromCandPairs;
  Int_t nCand = (Int_t)fCandidates.size();
  for (Int_t iCand = 0; iCand < nCand-1; iCand++) {
    for (Int_t jCand = iCand+1; jCand < nCand; jCand++) {
      invMass = CalcInvariantMass(fCandidates[iCand],fCandidates[jCand]);
      openingAngle = CalcOpeningAngle(fCandidates[iCand],fCandidates[jCand]);
      pT = CalcPt(fCandidates[iCand],fCandidates[jCand]);
      p = CalcP(fCandidates[iCand],fCandidates[jCand]);
      CbmMCTrack *gamma = new CbmMCTrack(22, -1, 
					 fCandidates[iCand].momentum[0] + fCandidates[jCand].momentum[0],
					 fCandidates[iCand].momentum[1] + fCandidates[jCand].momentum[1],
					 fCandidates[iCand].momentum[2] + fCandidates[jCand].momentum[2],
					 0,
					 0,
					 0,
					 0,//invariantMass,
					 0);
      gammaFromCandPairs.push_back(gamma);
      fHistoMap["CandPairOpeningAngle"]->Fill(openingAngle);
      fHistoMap["InvMassSpectrumCandPairs"]->Fill(invMass);
    }
  }
  nGamma = (Int_t)gammaFromCandPairs.size();
  for (Int_t iGamma = 0; iGamma < nGamma-1; iGamma++) {
    gtrack1 = gammaFromCandPairs[iGamma];
    for (Int_t jGamma = iGamma+1; jGamma < nGamma; jGamma++) {
      gtrack2 = gammaFromCandPairs[jGamma];
      invMass = CalcInvariantMass(gtrack1, gtrack2);
      pT = CalcPt(gtrack1, gtrack2);
      p = CalcP(gtrack1, gtrack2);
      fHistoMap["InvMassSpectrumGammaCandPairs"]->Fill(invMass);
    }
  }

  Int_t nECand = (Int_t)fElectronCandidates.size();
  Int_t nPCand = (Int_t)fPositronCandidates.size();

  std::map<Int_t, std::vector<Int_t> > notDalitzEPMap;//< pi0Id, e+e-IDs[]>
  std::map<Int_t, std::vector<Int_t> >::iterator notDalitzEPMapIt;
  std::map<Int_t, std::vector<Int_t> > dalitzEPMap; //< pi0Id, e+e-IDs[]>
  std::map<Int_t, std::vector<Int_t> >::iterator dalitzEPMapIt;

  CbmMCTrack* mctMotherE = NULL; 
  CbmMCTrack* mctGraniE = NULL;  
  for (Int_t iECand = 0; iECand < nECand; iECand++) {
    //printf("%i (MID:%i ",iECand,fElectronCandidates[iECand].McMotherId);
    if (fElectronCandidates[iECand].McMotherId > -1) {
      mctMotherE = (CbmMCTrack*) fMCTracks->At(fElectronCandidates[iECand].McMotherId);
      //printf("MPID:%i) ",mctMotherE->GetPdgCode());
      if (mctMotherE->GetPdgCode() == 111){
	dalitzEPMap[fElectronCandidates[iECand].McMotherId].push_back(fElectronCandidates[iECand].stsMcTrackId);
	//printf("dalitz: %i(%i) ",fElectronCandidates[iECand].McMotherId,(Int_t)dalitzEPMap[fElectronCandidates[iECand].McMotherId].size());
      }
      else if (mctMotherE->GetPdgCode() == 22) {
	if (mctMotherE->GetMotherId() > -1){
	  mctGraniE = (CbmMCTrack*) fMCTracks->At(mctMotherE->GetMotherId());
	  if (mctGraniE->GetPdgCode() == 111){
	    notDalitzEPMap[mctMotherE->GetMotherId()].push_back(fElectronCandidates[iECand].stsMcTrackId);
	    //printf("not dalitz: %i(%i) ",mctMotherE->GetMotherId(),(Int_t)notDalitzEPMap[mctMotherE->GetMotherId()].size());
	  }
	}
      }
    }
  }
  CbmMCTrack* mctMotherP = NULL;
  CbmMCTrack* mctGraniP = NULL;
  for (Int_t iPCand = 0; iPCand < nPCand; iPCand++) {
    if (fPositronCandidates[iPCand].McMotherId > -1){
      mctMotherP = (CbmMCTrack*) fMCTracks->At(fPositronCandidates[iPCand].McMotherId);
      if (mctMotherP->GetPdgCode() == 111){
	dalitzEPMap[fPositronCandidates[iPCand].McMotherId].push_back(fPositronCandidates[iPCand].stsMcTrackId);
      }
      else if (mctMotherP->GetPdgCode() == 22) {
	if (mctMotherP->GetMotherId() > -1){
	  mctGraniP = (CbmMCTrack*) fMCTracks->At(mctMotherP->GetMotherId());
	  if (mctGraniP->GetPdgCode() == 111){
	    notDalitzEPMap[mctMotherP->GetMotherId()].push_back(fPositronCandidates[iPCand].stsMcTrackId);
	  }
	}
      }
    }
  }
  for (dalitzEPMapIt = dalitzEPMap.begin(); dalitzEPMapIt != dalitzEPMap.end(); dalitzEPMapIt++) {
    Int_t dalitzDaughterSize = (Int_t)(*dalitzEPMapIt).second.size();
    if (dalitzDaughterSize == 1)
      fHistoMap["EPPairFromPi0DetectionEfficiency"]->Fill(7);
    if (dalitzDaughterSize == 2)
      fHistoMap["EPPairFromPi0DetectionEfficiency"]->Fill(8);
  }
  for (notDalitzEPMapIt = notDalitzEPMap.begin(); notDalitzEPMapIt != notDalitzEPMap.end(); notDalitzEPMapIt++) {
    Int_t notDalitzDaughterSize = (Int_t)(*notDalitzEPMapIt).second.size();
    if (notDalitzDaughterSize == 1)
      fHistoMap["EPPairFromPi0DetectionEfficiency"]->Fill(2);
    if (notDalitzDaughterSize == 2)
      fHistoMap["EPPairFromPi0DetectionEfficiency"]->Fill(3);
    if (notDalitzDaughterSize == 3)
      fHistoMap["EPPairFromPi0DetectionEfficiency"]->Fill(4);
    if (notDalitzDaughterSize == 4)
      fHistoMap["EPPairFromPi0DetectionEfficiency"]->Fill(5);
  }

  std::vector<CbmMCTrack *> gammaFromEPCandPairs;
  for (Int_t iECand = 0; iECand < nECand; iECand++) {
    for (Int_t iPCand = 0; iPCand < nPCand; iPCand++) {
 
      invMass = CalcInvariantMass(fElectronCandidates[iECand],fPositronCandidates[iPCand]);
      openingAngle = CalcOpeningAngle(fElectronCandidates[iECand],fPositronCandidates[iPCand]);
      pT = CalcPt(fElectronCandidates[iECand],fPositronCandidates[iPCand]);
      p = CalcP(fElectronCandidates[iECand],fPositronCandidates[iPCand]);
      CbmMCTrack *gamma = new CbmMCTrack(22, -1, 
					 fElectronCandidates[iECand].momentum[0] + fPositronCandidates[iPCand].momentum[0],
					 fElectronCandidates[iECand].momentum[1] + fPositronCandidates[iPCand].momentum[1],
					 fElectronCandidates[iECand].momentum[2] + fPositronCandidates[iPCand].momentum[2],
					 0,
					 0,
					 0,
					 0,//invariantMass,
					 0);
      gammaFromEPCandPairs.push_back(gamma);
      fHistoMap["EPCandPairOpeningAngle"]->Fill(openingAngle);
      fHistoMap["InvMassSpectrumEPCandPairs"]->Fill(invMass);    
 
      //if (openingAngle < 1.0){
      //gammaFromEPCandPairsOpeningAngle.push_back(gamma);
      //fHistoMap["InvMassSpectrumEPCandPairsOpeningAngle"]->Fill(invMass);
      //}
    }
  }

  
  nGamma = (Int_t)gammaFromEPCandPairs.size();
  for (Int_t iGamma = 0; iGamma < nGamma-1; iGamma++) {
    gtrack1 = gammaFromEPCandPairs[iGamma];
    for (Int_t jGamma = iGamma+1; jGamma < nGamma; jGamma++) {
      gtrack2 = gammaFromEPCandPairs[jGamma];
      invMass = CalcInvariantMass(gtrack1, gtrack2);
      pT = CalcPt(gtrack1, gtrack2);
      p = CalcP(gtrack1, gtrack2);
      fHistoMap["InvMassSpectrumGammaEPCandPairs"]->Fill(invMass);
    }
  }
  std::vector<CbmMCTrack *> gammaFromEPCandPairsOpeningAngle;
  for (Int_t iECand = 0; iECand < nECand; iECand++) {
    for (Int_t iPCand = 0; iPCand < nPCand; iPCand++) {
      invMass = CalcInvariantMass(fElectronCandidates[iECand],fPositronCandidates[iPCand]);
      openingAngle = CalcOpeningAngle(fElectronCandidates[iECand],fPositronCandidates[iPCand]);
      pT = CalcPt(fElectronCandidates[iECand],fPositronCandidates[iPCand]);
      p = CalcP(fElectronCandidates[iECand],fPositronCandidates[iPCand]);
      if (openingAngle < 1.0){
	CbmMCTrack *gamma = new CbmMCTrack(22, -1, 
					   fElectronCandidates[iECand].momentum[0] + fPositronCandidates[iPCand].momentum[0],
					   fElectronCandidates[iECand].momentum[1] + fPositronCandidates[iPCand].momentum[1],
					   fElectronCandidates[iECand].momentum[2] + fPositronCandidates[iPCand].momentum[2],
					   0,
					   0,
					   0,
					   0,//invariantMass,
					   0);
     	gammaFromEPCandPairsOpeningAngle.push_back(gamma);
	//fHistoMap["InvMassSpectrumEPCandPairsOpeningAngle"]->Fill(invMass);
      }
    }
  }
  nGamma = (Int_t)gammaFromEPCandPairsOpeningAngle.size();
  for (Int_t iGamma = 0; iGamma < nGamma-1; iGamma++) {
    gtrack1 = gammaFromEPCandPairsOpeningAngle[iGamma];
    for (Int_t jGamma = iGamma+1; jGamma < nGamma; jGamma++) {
      gtrack2 = gammaFromEPCandPairsOpeningAngle[jGamma];
      invMass = CalcInvariantMass(gtrack1, gtrack2);
      pT = CalcPt(gtrack1, gtrack2);
      p = CalcP(gtrack1, gtrack2);
      fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"]->Fill(invMass);
    }
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
  // DO:: clear all global data container
 

  for (Int_t i = 0; i < fGammaFromPairs.size(); i++)
    delete fGammaFromPairs[i];
  fGammaFromPairs.clear();

  for (Int_t i = 0; i < fMCParticleMap.size(); i++)
    delete fMCParticleMap[i];
  fMCParticleMap.clear();

  for (Int_t i = 0; i < gammaFromEPPairs.size(); i++)
    delete gammaFromEPPairs[i]; 
  gammaFromEPPairs.clear();

  for (Int_t i = 0; i < gammaFromEPPairsInMagnet.size(); i++)
    if (NULL != gammaFromEPPairsInMagnet[i])
      delete gammaFromEPPairsInMagnet[i];
  gammaFromEPPairsInMagnet.clear();

  for (Int_t i = 0; i < gammaFromCandPairs.size(); i++)
    if (NULL != gammaFromCandPairs[i])
      delete gammaFromCandPairs[i];
  gammaFromCandPairs.clear();

  for (Int_t i = 0; i < gammaFromEPCandPairs.size(); i++)
    if (NULL != gammaFromEPCandPairs[i])
      delete gammaFromEPCandPairs[i];
  gammaFromEPCandPairs.clear();

  
  for (Int_t i = 0; i < gammaFromEPCandPairsOpeningAngle.size(); i++)
    if (NULL != gammaFromEPCandPairsOpeningAngle[i])
      delete gammaFromEPCandPairsOpeningAngle[i];
  gammaFromEPCandPairsOpeningAngle.clear();

  for (dalitzEPMapIt = dalitzEPMap.begin(); dalitzEPMapIt != dalitzEPMap.end(); dalitzEPMapIt++) 
    dalitzEPMapIt->second.clear();
  dalitzEPMap.clear();

  for (notDalitzEPMapIt = notDalitzEPMap.begin(); notDalitzEPMapIt != notDalitzEPMap.end(); notDalitzEPMapIt++) 
    notDalitzEPMapIt->second.clear();
  notDalitzEPMap.clear();

  fCandidates.clear();
  fElectronCandidates.clear();
  fPositronCandidates.clear();
  /*
    for (Int_t i = 0; i < gammaFromEPPairsInMagnetBackground.size(); i++)
    if (NULL != gammaFromEPPairsInMagnetBackground[i])
    delete gammaFromEPPairsInMagnetBackground[i];
  
    for (Int_t i = 0; i < gammaFromEPPairsInTarget.size(); i++)
    if (NULL != gammaFromEPPairsInTarget[i])
    delete gammaFromEPPairsInTarget[i];
    for (Int_t i = 0; i < gammaFromEPPairsOpeningAngle.size(); i++)
    if (NULL != gammaFromEPPairsOpeningAngle[i])
    delete gammaFromEPPairsOpeningAngle[i];
    for (Int_t i = 0; i < gammaFromTrueEPPairs.size(); i++)
    if (NULL != gammaFromTrueEPPairs[i])
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

    Bool_t CbmTrdPhotonAnalysis::IsElec( CbmRichRing * ring, Double_t momentum, CbmTrdTrack* trdTrack, CbmGlobalTrack * gTrack, ElectronCandidate* cand)
    {
      Bool_t richEl = IsRichElec(ring, momentum, cand);
      Bool_t trdEl = IsTrdElec(trdTrack, cand);
      Double_t annRich = cand->richAnn;
      Double_t annTrd = cand->trdAnn;
      Bool_t tofEl = IsTofElec(gTrack, momentum, cand);

      if (richEl && trdEl && tofEl) {
	cand->isElectron = true;
	return true;
      } 
      else {
	cand->isElectron = false;
	return false;
      }

      /*
	if (annRich > 0.85){
	cand->isElec = true;
	return;
	} else if (annTrd > 1.95) {
	cand->isElec = true;
	return;
	} else 	if (annRich > 0.4 && annTrd > 0.92) {
	cand->isElec = true;
	return;
	} else if (momentum < 0.7 && tofEl && annRich > 0.5) {
	cand->isElec = true;
	return;
	} else if (momentum < 0.7 && tofEl && annTrd > 0.92){
	cand->isElec = true;
	return;
	} else if (richEl && trdEl && tofEl) {
	cand->isElec = true;
	return;
	} else {
	cand->isElec = false;
	return;
	}*/
    }

    Bool_t CbmTrdPhotonAnalysis::IsRichElec(CbmRichRing * ring, Double_t momentum, ElectronCandidate* cand)
    {
      if (fUseRichAnn == false){
	Double_t axisA = ring->GetAaxis();
	Double_t axisB = ring->GetBaxis();
	Double_t dist = ring->GetDistance();
	if ( fabs(axisA-fMeanA) < fRmsCoeff*fRmsA &&fabs(axisB-fMeanB) < fRmsCoeff*fRmsB && dist < fDistCut){
	  return true;
	} else {
	  return false;
	}
      } else {
	Double_t ann = fElIdAnn->DoSelect(ring, momentum);
	cand->richAnn = ann;
	if (ann > fRichAnnCut) return true;
	else  return false;
      }
    }


    Bool_t CbmTrdPhotonAnalysis::IsTrdElec(CbmTrdTrack* trdTrack, ElectronCandidate* cand)
    {
      Double_t ann = trdTrack->GetPidANN();
      cand->trdAnn = ann;
      if (ann > fTrdAnnCut) return true;
      else return false;

    }

    Bool_t CbmTrdPhotonAnalysis::IsTofElec(CbmGlobalTrack* gTrack, Double_t momentum, ElectronCandidate* cand)
    {
      Double_t trackLength = gTrack->GetLength() / 100.;

      // Calculate time of flight from TOF hit
      Int_t tofIndex = gTrack->GetTofHitIndex();
      CbmTofHit* tofHit = (CbmTofHit*) fTofHits->At(tofIndex);
      if (NULL == tofHit){
	cand->mass2 = 100.;
	return false;
      }
      Double_t time = 0.2998 * tofHit->GetTime(); // time in ns -> transfrom to ct in m

      // Calculate mass squared
      Double_t mass2 = TMath::Power(momentum, 2.) * (TMath::Power(time/ trackLength, 2) - 1);
      cand->mass2 =mass2;

      if (momentum >= 1.) {
	if (mass2 < (0.013*momentum - 0.003)){
	  return true;
	}
      } else {
	if (mass2 < 0.01){
	  return true;//fTofM2
	}
      }
      return false;
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

    Double_t CbmTrdPhotonAnalysis::CalcP(CbmMCTrack* trackA, CbmMCTrack* trackB)
    {
      Double_t Px = trackA->GetPx() + trackB->GetPx();
      Double_t Py = trackA->GetPy() + trackB->GetPy();
      Double_t Pz = trackA->GetPz() + trackB->GetPz();
      Double_t P = TMath::Sqrt(pow(Px,2) + pow(Py,2) + pow(Pz,2));

      return P;
    }

    Double_t CbmTrdPhotonAnalysis::CalcInvariantMass(CbmMCTrack* trackA, CbmMCTrack* trackB)
    {
      Double_t energyA = TMath::Sqrt(pow(trackA->GetPx(),2) + pow(trackA->GetPy(),2) + pow(trackA->GetPz(),2));
      Double_t energyB = TMath::Sqrt(pow(trackB->GetPx(),2) + pow(trackB->GetPy(),2) + pow(trackB->GetPz(),2));
      //if (energyA != trackA->GetEnergy() || energyB != trackB->GetEnergy())
      //printf ("A: %.8f != %.8f Delta = %E  B: %.8f != %.8f Delta = %E\n", energyA, trackA->GetEnergy(), energyA-trackA->GetEnergy(), energyB, trackB->GetEnergy(), energyB - trackB->GetEnergy());
      Double_t energyAB = trackA->GetEnergy() + trackB->GetEnergy();//energyA + energyB;
      //if (energyAB != energyA + energyB)
      // printf ("A: %.8f != %.8f Delta = %E  B: %.8f != %.8f Delta = %E\n", energyA, trackA->GetEnergy(), energyA-trackA->GetEnergy(), energyB, trackB->GetEnergy(), energyB - trackB->GetEnergy());
      energyAB = energyA + energyB;
      //Double_t ptAB = trackA->GetPt() + trackB->GetPt();
      Double_t invariantMass = TMath::Sqrt(
					   pow(energyAB,2) 
					   - (
					      pow((trackA->GetPx() + trackB->GetPx()),2) 
					      + pow((trackA->GetPy() + trackB->GetPy()),2) 
					      + pow((trackA->GetPz() + trackB->GetPz()),2)
					      )
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

    Double_t CbmTrdPhotonAnalysis::CalcPt(ElectronCandidate candA, ElectronCandidate candB)
    {
      Double_t Px = candA.momentum[0] + candB.momentum[0];
      Double_t Py = candA.momentum[1] + candB.momentum[1];
      Double_t Pt = TMath::Sqrt(pow(Px,2) + pow(Py,2));

      return Pt;
    }

    Double_t CbmTrdPhotonAnalysis::CalcP(ElectronCandidate candA, ElectronCandidate candB)
    {
      Double_t Px = candA.momentum[0] + candB.momentum[0];
      Double_t Py = candA.momentum[1] + candB.momentum[1];
      Double_t Pz = candA.momentum[2] + candB.momentum[2];
      Double_t P = TMath::Sqrt(pow(Px,2) + pow(Py,2) + pow(Pz,2));

      return P;
    }

    Double_t CbmTrdPhotonAnalysis::CalcInvariantMass(ElectronCandidate candA, ElectronCandidate candB)
    {
      Double_t energyA = TMath::Sqrt(pow(candA.momentum[0],2) + pow(candA.momentum[1],2) + pow(candA.momentum[2],2));
      Double_t energyB = TMath::Sqrt(pow(candB.momentum[0],2) + pow(candB.momentum[1],2) + pow(candB.momentum[2],2));
      //if (energyA != candA.energy || energyB != candB.energy)
      //printf ("A: %.8f != %.8f Delta = %E  B: %.8f != %.8f Delta = %E\n", energyA, candA.energy, energyA-candA.energy, energyB, candB.energy, energyB - candB.energy);
      Double_t energyAB = candA.energy + candB.energy;//energyA + energyB;
      //if (energyAB != energyA + energyB)
      // printf ("A: %.8f != %.8f Delta = %E  B: %.8f != %.8f Delta = %E\n", energyA, candA.energy, energyA-candA.energy, energyB, candB.energy, energyB - candB.energy);
      energyAB = energyA + energyB;
      //Double_t ptAB = candA.GetPt() + candB.GetPt();
      Double_t invariantMass = TMath::Sqrt(
					   pow(energyAB,2) 
					   - (
					      pow((candA.momentum[0] + candB.momentum[0]),2) 
					      + pow((candA.momentum[1] + candB.momentum[1]),2) 
					      + pow((candA.momentum[2] + candB.momentum[2]),2)
					      )
					   );
      return invariantMass;
    }


    Double_t CbmTrdPhotonAnalysis::CalcOpeningAngle(ElectronCandidate candA, ElectronCandidate candB) {
      //Double_t energyA = candA.energy;//TMath::Sqrt(pow(candA.momentum[0],2) + pow(candA.momentum[1],2) + pow(candA.momentum[2],2));
      //Double_t energyB = candB.energy;//TMath::Sqrt(pow(candB.momentum[0],2) + pow(candB.momentum[1],2) + pow(candB.momentum[2],2));
      /*
	Double_t openingAngle = TMath::ACos(0.5 * pow(CalcInvariantMass(candA, candB),2) - pow(0.000510998928,2) - energyA * energyB
	+ (candA.momentum[0] * candB.momentum[0] + candA.momentum[1] * candB.momentum[1] + candA.momentum[2] * candB.momentum[2])
	) / TMath::Pi() * 180.;
      */
      Double_t openingAngle = TMath::ACos(
					  (
					   0.5 * 
					   (pow(CalcInvariantMass(candA, candB),2) - pow(candA.mass,2) - pow(candB.mass,2))
					   - (candA.energy * candB.energy)
					   ) 
					  // / (-1. * (candA.momentum[0] * candB.momentum[0] + candA.momentum[1] * candB.momentum[1] + candA.momentum[2] * candB.momentum[2]))
				      
					  / ( -1. * 
					      TMath::Sqrt(pow(candA.momentum[0],2) + pow(candA.momentum[1],2) + pow(candA.momentum[2],2)) 
					      * TMath::Sqrt(pow(candB.momentum[0],2) + pow(candB.momentum[1],2) + pow(candB.momentum[2],2))
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
      for (Int_t bin = 1; bin < fHistoMap["EPPairOpeningAngleInTarget"]->GetNbinsX(); bin++) {
	//for (Int_t bin = 1; bin < maxbin; bin++) {
	allPairs += fHistoMap["EPPairOpeningAngleInTarget"]->GetBinContent(bin);
	gammaPairs += fHistoMap["EPPairOpeningAngleGamma"]->GetBinContent(bin);
	pi0Pairs += fHistoMap["EPPairOpeningAnglePi0"]->GetBinContent(bin);
	gammaWoPi0 = gammaPairs - pi0Pairs;
	fHistoMap["PairOpeningAngleAll"]->SetBinContent(bin,allPairs / (Float_t)fHistoMap["EPPairOpeningAngleInTarget"]->GetEntries());
	fHistoMap["PairOpeningAngleGamma"]->SetBinContent(bin,gammaPairs / (Float_t)fHistoMap["EPPairOpeningAngleInTarget"]/*fEPPairOpeningAngleGamma*/->GetEntries());
	fHistoMap["PairOpeningAnglePi0"]->SetBinContent(bin,pi0Pairs / (Float_t)fHistoMap["EPPairOpeningAngleInTarget"]/*fEPPairOpeningAnglePi0*/->GetEntries());
	fHistoMap["PairOpeningAngleGammaWoPi0"]->SetBinContent(bin,gammaWoPi0 / (Float_t)(fHistoMap["EPPairOpeningAngleInTarget"]/*fEPPairOpeningAngleGamma->GetEntries() - fEPPairOpeningAnglePi0*/->GetEntries()));
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
  fHistoMap["MC_EventCounter"] = new TH1I("MC_EventCounter","MC_EventCounter",1,-0.5,0.5);
  NiceHisto1(fHistoMap["MC_EventCounter"],1,0,0,"","number of events");
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
    "Primary"
  };
  fHistoMap["GT_MC_PID"] = new TH1I("GT_MC_PID","MC PID of associated STS tracks for global tracks",49,0.5,49.5);
  fHistoMap["MCPid_global"] = new TH1I("MCPid_global","MC Pid global",49,0.5,49.5);
  fHistoMap["MCPid_inMagnet"] = new TH1I("MCPid_inMagnet","MC Pid in magnet",49,0.5,49.5);
  fHistoMap["MCPid_inTarget"] = new TH1I("MCPid_inTarget","MC Pid in target",49,0.5,49.5);
  fHistoMap["GTPid"] = new TH1I("GTPid","GT Pid",49,0.5,49.5);
  fHistoMap["Pt_global"] = new TH2I("Pt_global","p_{T} vs PID global",200,0,2,49,0.5,49.5);
  fHistoMap["Pt_inMagnet"] = new TH2I("Pt_inMagnet","p_{T} vs PID in magnet",200,0,2,49,0.5,49.5);
  fHistoMap["Pt_inTarget"] = new TH2I("Pt_inTarget","p_{T} vs PID in target",200,0,2,49,0.5,49.5);
  fHistoMap["P_global"] = new TH2I("P_global","p vs PID global",200,0,2,49,0.5,49.5);
  fHistoMap["P_inMagnet"] = new TH2I("P_inMagnet","p vs PID in magnet",200,0,2,49,0.5,49.5);
  fHistoMap["P_inTarget"] = new TH2I("P_inTarget","p vs PID in target",200,0,2,49,0.5,49.5);
  fHistoMap["Pt_OpenAngle_global"] = new TH2I("Pt_OpenAngle_global","p_{T} vs OpenAngle global",200,0,2,180,0,180);
  fHistoMap["Pt_OpenAngle_inMagnet"] = new TH2I("Pt_OpenAngle_inMagnet","p_{T} vs OpenAngle in magnet",200,0,2,180,0,180);
  fHistoMap["Pt_OpenAngle_inTarget"] = new TH2I("Pt_OpenAngle_inTarget","p_{T} vs OpenAngle in target",200,0,2,180,0,180);
  fHistoMap["Pt_OpenAngle_inTarget_EPPairsPi0"] = new TH2I("Pt_OpenAngle_inTarget_EPPairsPi0","p_{T} vs OpenAngle in target e^{+}e^{-} from #gamma",200,0,2,45,0,4.5);
  fHistoMap["Pt_OpenAngle_inTarget_EPPairsGamma"] = new TH2I("Pt_OpenAngle_inTarget_EPPairsGamma","p_{T} vs OpenAngle in target e^{+}e^{-} from #pi^{0}",200,0,2,45,0,4.5);
  fHistoMap["DalizMother"] = new TH1I("DalizMother","Mother ID for e^{+} & e^{-} & #gamma",49,0.5,49.5);
  fHistoMap["ePlusMinusMother"] = new TH1I("ePlusMinusMother","Mother ID for e^{+}/e^{-}",49,0.5,49.5);
  fHistoMap["ePlusAndMinusMother"] = new TH1I("ePlusAndMinusMother","Mother ID for e^{+} & e^{-}",49,0.5,49.5);
  fHistoMap["gammaMother"] = new TH1I("gammaMother","Mother ID for #gamma",49,0.5,49.5);
  fHistoMap["gammaAndGammaMother"] = new TH1I("gammaAndGammaMother","Mother ID for #gamma & #gamma",49,0.5,49.5);
  fHistoMap["gammaDaughter"] = new TH1I("gammaDaughter","Daughter ID for #gamma",49,0.5,49.5);
  fHistoMap["ZBirthAll"] = new TH2I("ZBirth_All","z birth",10000,0,1000,49,0.5,49.5);
  fHistoMap["ZBirthEPfromGamma"] = new TH1I("ZBirthEPfromGamma","z birth from e^{+} & e^{-} pairs with mother #gamma",10000,0,1000);
  fHistoMap["NoDaughters_global"] = new TH2I("NoDaughters_global","Number of daughters global",49,0.5,49.5,101,-0.5,100.5);
  fHistoMap["NoDaughters_inMagnet"] = new TH2I("NoDaughters_inMagnet","Number of daughters in magnet",49,0.5,49.5,101,-0.5,100.5);
  fHistoMap["NoDaughters_inTarget"] = new TH2I("NoDaughters_inTarget","Number of daughters in target",49,0.5,49.5,101,-0.5,100.5);
  fHistoMap["MotherDaughter_global"] = new TH2I("MotherDaughter_global","mother / daughter decay global",49,0.5,49.5,49,0.5,49.5);
  fHistoMap["MotherDaughter_inMagnet"] = new TH2I("MotherDaughter_inMagnet","mother / daughter decay in magnet",49,0.5,49.5,49,0.5,49.5);
  fHistoMap["MotherDaughter_inTarget"] = new TH2I("MotherDaughter_inTarget","mother / daughter decay in target",49,0.5,49.5,49,0.5,49.5);
  fHistoMap["MotherDaughterZBirth"] = new TH3I("MotherDaughterZBirth","mother / daughter / z-birth",10000,0,1000,49,0.5,49.5,49,0.5,49.5);
  fHistoMap["motherGrani_gamma_global"] = new TH2I("motherGrani_gamma_global","mother / grandmother of #gamma decay global",49,0.5,49.5,49,0.5,49.5);
  fHistoMap["motherGrani_posi_global"] = new TH2I("motherGrani_posi_global","mother / grandmother of e^{+} decay global",49,0.5,49.5,49,0.5,49.5);
  fHistoMap["motherGrani_elec_global"] = new TH2I("motherGrani_elec_global","mother / grandmother of e^{-} decay global",49,0.5,49.5,49,0.5,49.5);
  fHistoMap["motherGrani_gamma_inMagnet"] = new TH2I("motherGrani_gamma_inMagnet","mother / grandmother of #gamma decay in magnet",49,0.5,49.5,49,0.5,49.5);
  fHistoMap["motherGrani_posi_inMagnet"] = new TH2I("motherGrani_posi_inMagnet","mother / grandmother of e^{+} decay in magnet",49,0.5,49.5,49,0.5,49.5); 
  fHistoMap["motherGrani_elec_inMagnet"] = new TH2I("motherGrani_elec_inMagnet","mother / grandmother of e^{-} decay in magnet",49,0.5,49.5,49,0.5,49.5);
  fHistoMap["motherGrani_gamma_inTarget"] = new TH2I("motherGrani_gamma_inTarget","mother / grandmother of #gamma decay in target",49,0.5,49.5,49,0.5,49.5);
  fHistoMap["motherGrani_posi_inTarget"] = new TH2I("motherGrani_posi_inTarget","mother / grandmother of e^{+} decay in target",49,0.5,49.5,49,0.5,49.5);
  fHistoMap["motherGrani_elec_inTarget"] = new TH2I("motherGrani_elec_inTarget","mother / grandmother of e^{-} decay in target",49,0.5,49.5,49,0.5,49.5);

  fHistoMap["InvMPairMother"] = new TH2I("InvMPairMother","Invariant Mass of mixed e^{+} & e^{-} pairs vs. mother id",2000,0,2,49,0.5,49.5);
  fHistoMap["PtPairMother"] = new TH2I("PtPairMother","p_{T} of mixed e^{+} & e^{-} pairs vs. mother id",200,0,2,49,0.5,49.5);
  fHistoMap["PPairMother"] = new TH2I("PPairMother","p of mixed e^{+} & e^{-} pairs vs. mother id}",200,0,2,49,0.5,49.5);
  fHistoMap["OpenAnglePairMother"] = new TH2I("OpenAnglePairMother","Opening Angle of mixed e^{+} & e^{-} pairs vs. mother id",1800,0,180,49,0.5,49.5);
  fHistoMap["InvMPairSameMother"] = new TH2I("InvMPairSameMother","Invariant Mass of e^{+} & e^{-} pairs vs. same mother id",2000,0,2,49,0.5,49.5);
  fHistoMap["PtPairSameMother"] = new TH2I("PtPairSameMother","p_{T} of e^{+} & e^{-} pairs vs. same mother id",200,0,2,49,0.5,49.5);
  fHistoMap["PPairSameMother"] = new TH2I("PPairSameMother","p of e^{+} & e^{-} pairs vs. same mother id",200,0,2,49,0.5,49.5);
  fHistoMap["OpenAnglePairSameMother"] = new TH2I("OpenAnglePairSameMother","Opening Angle of e^{+} & e^{-} pairs vs. same mother id",1800,0,180,49,0.5,49.5);


  for (Int_t bin = 0; bin < 49; bin++) {
    fHistoMap["GT_MC_PID"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["InvMPairMother"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["PtPairMother"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["PPairMother"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["OpenAnglePairMother"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["InvMPairSameMother"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["PtPairSameMother"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["PPairSameMother"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["OpenAnglePairSameMother"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MCPid_global"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MCPid_inMagnet"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MCPid_inTarget"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["GTPid"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["Pt_global"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["Pt_inMagnet"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["Pt_inTarget"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["P_global"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["P_inMagnet"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["P_inTarget"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["DalizMother"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["ePlusMinusMother"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["ePlusAndMinusMother"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["gammaAndGammaMother"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["gammaMother"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["gammaDaughter"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["ZBirthAll"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MotherDaughter_global"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MotherDaughter_global"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MotherDaughter_inMagnet"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MotherDaughter_inMagnet"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MotherDaughter_inTarget"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MotherDaughter_inTarget"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MotherDaughterZBirth"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["MotherDaughterZBirth"]->GetZaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["NoDaughters_global"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["NoDaughters_inMagnet"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["NoDaughters_inTarget"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_gamma_global"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_gamma_global"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_posi_global"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_posi_global"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_elec_global"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_elec_global"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_gamma_inMagnet"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_gamma_inMagnet"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_posi_inMagnet"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_posi_inMagnet"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_elec_inMagnet"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_elec_inMagnet"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_gamma_inTarget"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_gamma_inTarget"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_posi_inTarget"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_posi_inTarget"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["motherGrani_elec_inTarget"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]); 
    fHistoMap["motherGrani_elec_inTarget"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]); 

  }

  TString coo[3] = {"x","y","z"};
  Int_t low[3] = {-50,-50,0};
  Int_t high[3] = {50,50,100};
  for (Int_t i = 0; i < 3; i++) {
    fHistoMap["PairAllVertex_" + coo[i]] = new TH1I("PairAllVertex_" + coo[i], "all e^{+} e^{-} pair vertex " + coo[i], 1000, low[i], high[i]);
    fHistoMap["PairGammaVertex_" + coo[i]] = new TH1I("PairGammaVertex_" + coo[i], "e^{+} e^{-} pair from #gamma vertex " + coo[i], 1000, low[i], high[i]);
    fHistoMap["PairPi0Vertex_" + coo[i]] = new TH1I("PairPi0Vertex_" + coo[i], "e^{+} e^{-} pair from #pi^{0} vertex " + coo[i], 1000, low[i], high[i]);

    fHistoMap["pi02GammaVertex_" + coo[i]] = new TH1I("pi02GammaVertex_" + coo[i], "decay vertex of #pi^{0} to 2#gamma or e^{+}e^{-}#gamma" + coo[i], 1000, low[i], high[i]);
    fHistoMap["gamma2PairVertex_" + coo[i]] = new TH1I("gamma2PairVertex_" + coo[i], "decay vertex of #gamma to e^{+} e^{-} pair " + coo[i], 1000, low[i], high[i]);

    NiceHisto1(fHistoMap["PairAllVertex_" + coo[i]],1,0,0,TString(coo[i]+"-position [cm]"),"");
    NiceHisto1(fHistoMap["PairGammaVertex_" + coo[i]],2,0,0,TString(coo[i]+"-position [cm]"),"");
    NiceHisto1(fHistoMap["PairPi0Vertex_" + coo[i]],3,0,0,TString(coo[i]+"-position [cm]"),"");

    NiceHisto1(fHistoMap["pi02GammaVertex_" + coo[i]],1,0,0,TString(coo[i]+"-position [cm]"),"");
    NiceHisto1(fHistoMap["gamma2PairVertex_" + coo[i]],2,0,0,TString(coo[i]+"-position [cm]"),"");
  }
  /*
    fHistoMap["MCPid"] = new TH1I("MCPid","MC Pid",4501,-1000.5,3500.5);
    fHistoMap["GTPid"] = new TH1I("GTPid","GT Pid",4501,-1000.5,3500.5);
    fHistoMap["ePlusMinusMother"] = new TH1I("fHistoMap["ePlusMinusMother","Mother ID for e^{+}/e^{-}",4501,-1000.5,3500.5);
  */
  fHistoMap["EPPairVertexDistance_global"] = new TH1I("EPPairVertexDistance_global","Vertex distance for e^{+}e^{-} pairs global",2500000,0,2500);
  fHistoMap["EPPairVertexDistance_inMagnet"] = new TH1I("EPPairVertexDistance_inMagnet","Vertex distance for e^{+}e^{-} pairs in Magnet",1e7,0,1);
  fHistoMap["EPPairVertexDistance_inTarget"] = new TH1I("EPPairVertexDistance_inTarget","Vertex distance for e^{+}e^{-} pairs in Target",1e7,0,1);
  fHistoMap["ZBirth[0]"] = new TH1I("ZBirth_gamma","z birth #gamma",10000,0,1000);
  fHistoMap["ZBirth[1]"] = new TH1I("ZBirth_positron","z birth e^{+}",10000,0,1000);
  fHistoMap["ZBirth[2]"] = new TH1I("ZBirth_electron","z birth e^{-}",10000,0,1000);
  fHistoMap["ZBirth[3]"] = new TH1I("ZBirth_pi0","z birth #pi^{0}",10000,0,1000);
  fBirthPi0 = new TH3I("BirthPi0","#pi^{0} vertex"           ,100,-500,500,100,-500,500,100,0,1000);
  fBirthPair = new TH3I("BirthPair","e^{-}e^{+} pairs vertex",100,-500,500,100,-500,500,100,0,1000);
  fBirthGamma = new TH3I("BirthGamma","#gamma vertex"        ,100,-500,500,100,-500,500,100,0,1000);
  fHistoMap["PairHistory"] = new TH1I("PairHistory","history for #gamma",10,-0.5,9.5);
  fHistoMap["PairHistory"]->GetXaxis()->SetBinLabel( 1,"all e^{-} e^{+} pairs");
  fHistoMap["PairHistory"]->GetXaxis()->SetBinLabel( 2,"pairs from #gamma");
  fHistoMap["PairHistory"]->GetXaxis()->SetBinLabel( 3,"pairs from #pi^{0}#rightarrow e^{+}+e^{-}+#gamma");
  fHistoMap["PairHistory"]->GetXaxis()->SetBinLabel( 4,"all pairs produced within magnet");
  fHistoMap["PairHistory"]->GetXaxis()->SetBinLabel( 5,"pairs from #gamma within magnet");
  fHistoMap["PairHistory"]->GetXaxis()->SetBinLabel( 6,"pairs from #pi^{0} within magnet");
  fHistoMap["PairHistory"]->GetXaxis()->SetBinLabel( 7,"all pairs produced within target");
  fHistoMap["PairHistory"]->GetXaxis()->SetBinLabel( 8,"pairs from #gamma within target");
  fHistoMap["PairHistory"]->GetXaxis()->SetBinLabel( 9,"pairs from #pi^{0} within target");
  fHistoMap["PairHistory"]->GetXaxis()->SetBinLabel(10,"");

  fHistoMap["InvMassSpectrumGammaTruePairs"] = new TH1I("InvMassSpectrumGammaTruePairs","Invariant mass spectrum from true MC-#gamma-#gamma pairs",2000,0,2);
  fHistoMap["InvMassSpectrumGammaAllPairs"] = new TH1I("InvMassSpectrumGammaAllPairs","Invariant mass spectrum from all MC-#gamma-#gamma pairs",2000,0,2);
  fHistoMap["InvMassSpectrumGammaEPPairs"] = new TH1I("InvMassSpectrumGammaEPPairs","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+/-}e^{-/+} pairs outside magnet",2000,0,2);
  fHistoMap["InvMassSpectrumGammaEPPairsInTarget"] = new TH1I("InvMassSpectrumGammaEPPairsInTarget","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs in target",2000,0,2);
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"] = new TH1I("InvMassSpectrumGammaEPPairsInMagnet","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs in magnet",2000,0,2);
  fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"] = new TH1I("InvMassSpectrumGammaEPPairsOpenAngle","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs oping angle cut",2000,0,2);
  fHistoMap["InvMassSpectrumGammaEPPairsGamma"] = new TH1I("InvMassSpectrumGammaEPPairsGamma","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs mother=#gamma",2000,0,2);
  fHistoMap["InvMassSpectrumGammaEPPairsPi0"] = new TH1I("InvMassSpectrumGammaEPPairsPi0","Invariant mass spectrum from all MC-#gamma pairs from MC-e^{+}e^{-} pairs mother=#pi^{0}",2000,0,2);

  fHistoMap["InvMassSpectrumTrueEPPairs"] = new TH1I("InvMassSpectrumTrueEPPairs","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",2000,0,2);
  fHistoMap["InvMassSpectrumAllEPPairs"] = new TH1I("InvMassSpectrumAllEPPairs","Invariant mass spectrum from all MC-e^{+}e^{-} pairs",2000,0,2);
  fHistoMap["InvMassSpectrumEPPairsInTarget"] = new TH1I("InvMassSpectrumAllEPPairsInTarget","Invariant mass spectrum from all MC-e^{+}e^{-} pairs in target",2000,0,2);
  fHistoMap["InvMassSpectrumEPPairsInMagnet"] = new TH1I("InvMassSpectrumAllEPPairsInMagnet","Invariant mass spectrum from all MC-e^{+}e^{-} pairs in magnet",2000,0,2);
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnetBackground"] = new TH1D("InvMassSpectrumAllEPPairsInMagnetBackground","Invariant mass spectrum from all MC-e^{+}e^{-} pairs in magnet background",2000,0,2);

  fHistoMap["EPPairOpeningAngle"] = new TH1I("EPPairOpeningAngle","opening angle #theta of MC-e^{+}e^{-} pairs",1800,0,180);
  fHistoMap["EPPairOpeningAngleGamma"] = new TH1I("EPPairOpeningAngleGamma ","opening angle #theta of MC-e^{+}e^{-} pairs mother #gamma",1800,0,180);
  fHistoMap["EPPairOpeningAnglePi0"] = new TH1I("EPPairOpeningAnglePi0","opening angle #theta of MC-e^{+}e^{-} pairs mother #pi^{0}",1800,0,180);
  fHistoMap["EPPairOpeningAngleSameMother"] = new TH1I("EPPairOpeningAngleSameMother","opening angle #theta of MC-e^{+}e^{-} pairs same mother",1800,0,180);
  fHistoMap["EPPairOpeningAngleInTarget"] = new TH1I("EPPairOpeningAngleInTarget","opening angle #theta of MC-e^{+}e^{-} pairs in target",1800,0,180);
  fHistoMap["EPPairOpeningAngleInMagnet"] = new TH1I("EPPairOpeningAngleInMagnet","opening angle #theta of MC-e^{+}e^{-} pairs in magnet",1800,0,180);

  fHistoMap["PairOpeningAngleAll"] = new TH1I("PairOpeningAngleAll","opening angle #theta of MC-e^{+}e^{-} pairs",1800,0,180);
  fHistoMap["PairOpeningAngleGamma"] = new TH1I("PairOpeningAngleGamma","opening angle #theta of MC-e^{+}e^{-} pairs from #gamma",1800,0,180);
  fHistoMap["PairOpeningAnglePi0"] = new TH1I("PairOpeningAnglePi0","opening angle #theta of MC-e^{+}e^{-} pairs from #pi^{0}",1800,0,180);
  fHistoMap["PairOpeningAngleGammaWoPi0"] = new TH1I("PairOpeningAngleGammaWoPi0","opening angle #theta of MC-e^{+}e^{-} pairs from #gamma - pairs from #pi^{0}",1800,0,180);

  fHistoMap["InvMassSpectrumGammaEPPairsInTargetPt"] = new TH2I("InvMassSpectrumGammaEPPairsInTargetPt","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,250,0,5); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnetPt"] = new TH2I("InvMassSpectrumGammaEPPairsInMagnetPt","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,250,0,5); // new
  fHistoMap["InvMassSpectrumGammaEPPairsPt"] = new TH2I("InvMassSpectrumGammaEPPairsPt","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,500,0,5); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInTargetP"] = new TH2I("InvMassSpectrumGammaEPPairsInTargetP","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,100,0,10); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnetP"] = new TH2I("InvMassSpectrumGammaEPPairsInMagnetP","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,100,0,10); // new
  fHistoMap["InvMassSpectrumGammaEPPairsP"] = new TH2I("InvMassSpectrumGammaEPPairsP","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,1000,0,10); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInTargetOA"] = new TH2I("InvMassSpectrumGammaEPPairsInTargetOA","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,180,0,180); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnetOA"] = new TH2I("InvMassSpectrumGammaEPPairsInMagnetOA","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,180,0,180); // new
  fHistoMap["InvMassSpectrumGammaEPPairsOA"] = new TH2I("InvMassSpectrumGammaEPPairsOA","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,1800,0,180); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInTargetVD"] = new TH2I("InvMassSpectrumGammaEPPairsInTargetVD","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,1000,0,1e-10); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnetVD"] = new TH2I("InvMassSpectrumGammaEPPairsInMagnetVD","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",200,0,2,1000,0,1e-10); // new
  fHistoMap["InvMassSpectrumGammaEPPairsVD"] = new TH2I("InvMassSpectrumGammaEPPairsVD","Invariant mass spectrum from true MC-e^{+}e^{-} pairs",2000,0,2,1000,0,1e-7);
  fHistoMap["PidWkn"] = new TH1I("PidWkn","PidWkn",400,-2.1,2.1);
  fHistoMap["PidANN"] = new TH1I("PidANN","PidANN",400,-2.1,2.1);
  fHistoMap["PidLikeEL"] = new TH1I("PidLikeEL","PidLikeEL",400,-2.1,2.1);
  fHistoMap["PidLikePI"] = new TH1I("PidLikePI","PidLikePI",400,-2.1,2.1);
  fHistoMap["PidLikeKA"] = new TH1I("PidLikeKA","PidLikeKA",400,-2.1,2.1);
  fHistoMap["PidLikePR"] = new TH1I("PidLikePR","PidLikePR",400,-2.1,2.1);
  fHistoMap["PidLikeMU"] = new TH1I("PidLikeMU","PidLikeMU",400,-2.1,2.1);
  fHistoMap["GT_Qp_first"] = new TH1I("GT_Qp_first","GT_Qp_first",200,-20,20);
  fHistoMap["GT_Qp_last"] = new TH1I("GT_Qp_last","GT_Qp_last",200,-20,20);
  fHistoMap["GT_Vertex_x_first"] = new TH1I("GT_Vertex_x_first","GT_Vertex_x_first",1000,-50,50);
  fHistoMap["GT_Vertex_y_first"] = new TH1I("GT_Vertex_y_first","GT_Vertex_y_first",1000,-50,50);
  fHistoMap["GT_Vertex_z_first"] = new TH1I("GT_Vertex_z_first","GT_Vertex_z_first",10000,0,1000);
  fHistoMap["GT_Vertex_x_last"] = new TH1I("GT_Vertex_x_last","GT_Vertex_x_last",1000,-50,50);
  fHistoMap["GT_Vertex_y_last"] = new TH1I("GT_Vertex_y_last","GT_Vertex_y_last",1000,-50,50);
  fHistoMap["GT_Vertex_z_last"] = new TH1I("GT_Vertex_z_last","GT_Vertex_z_last",10000,0,1000);

  fHistoMap["KF_PrimVertex_x"] = new TH1I("KF_PrimVertex_x","KF_PrimVertex_x",1000,-50,50);
  fHistoMap["KF_PrimVertex_y"] = new TH1I("KF_PrimVertex_y","KF_PrimVertex_y",1000,-50,50);
  fHistoMap["KF_PrimVertex_z"] = new TH1I("KF_PrimVertex_z","KF_PrimVertex_z",10000,-0.5,1000);
  fHistoMap["KF_P"] = new TH1I("KF_P","KF_P",1000,0,10);
  fHistoMap["KF_Pt"] = new TH1I("KF_Pt","KF_Pt",200,0,2);
  fHistoMap["KF_E"] = new TH1I("KF_E","KF_E",2000,0,20);
  fHistoMap["KF_PID_MC_PID"] = new  TH2I("KF_PID_MC_PID","KF_PID_MC_PID",49,0.5,49.5,49,0.5,49.5);
  for (Int_t bin = 0; bin < 49; bin++) {
    fHistoMap["KF_PID_MC_PID"]->GetYaxis()->SetBinLabel(bin+1,particleID[bin]);
    fHistoMap["KF_PID_MC_PID"]->GetXaxis()->SetBinLabel(bin+1,particleID[bin]);
  }
  fHistoMap["DeltaP"] = new  TH2I("DeltaP","DeltaP",100,0,10,200,-10,10);
  fHistoMap["DeltaPt"] = new  TH2I("DeltaPt","DeltaPt",200,0,2,400,-2,2);
  fHistoMap["DeltaPGT_PKF"] = new  TH2I("DeltaPGT_PKF","DeltaPGT_PKF",100,0,10,400,-20,20);
  fHistoMap["TRD_GT_dEdx_KF_P"] = new  TH2I("TRD_GT_dEdx_KF_P","TRD_GT_dEdx_KF_P",100,0,10,500,0,100);
  fHistoMap["RICH_GT_radiusA_KF_P"] = new  TH2I("RICH_GT_radiusA_KF_P","RICH_GT_radiusA_KF_P",100,0,10,100,0,10);
  fHistoMap["RICH_GT_radiusB_KF_P"] = new  TH2I("RICH_GT_radiusB_KF_P","RICH_GT_radiusB_KF_P",100,0,10,100,0,10);
  fHistoMap["TOF_GT_time_KF_P"] = new  TH2I("TOF_GT_time_KF_P","TOF_GT_time_KF_P",100,0,5,100,0.5,1.1);

  fHistoMap["EPCandPairOpeningAngle"] = new TH1I("EPCandPairOpeningAngle","opening angle #theta of e^{+}e^{-} cand. pairs",1800,0,180);
  fHistoMap["CandPairOpeningAngle"] = new TH1I("CandPairOpeningAngle","opening angle #theta of e^{+}e^{-} cand. pairs",1800,0,180);
  fHistoMap["InvMassSpectrumEPCandPairs"] = new TH1I("InvMassSpectrumEPCandPairs","Invariant mass spectrum from e^{+}e^{-} cand. pairs",2000,0,2);
  fHistoMap["InvMassSpectrumCandPairs"] = new TH1I("InvMassSpectrumCandPairs","Invariant mass spectrum from e^{+}e^{-} cand. pairs",2000,0,2);
  fHistoMap["InvMassSpectrumGammaEPCandPairs"] = new TH1I("InvMassSpectrumGammaEPCandPairs","Invariant mass spectrum from #gamma pairs from e^{+/-}e^{-/+} cand. pairs",2000,0,2);
  fHistoMap["InvMassSpectrumGammaCandPairs"] = new TH1I("InvMassSpectrumGammaCandPairs","Invariant mass spectrum from all #gamma pairs from e^{+/-}e^{-/+} cand. pairs",2000,0,2);
  fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"] = new TH1I("InvMassSpectrumGammaCandPairsOpenAngle","Invariant mass spectrum from all #gamma pairs from e^{+/-}e^{-/+} cand. pairs",2000,0,2);

  fHistoMap["EPPairFromPi0DetectionEfficiency"] = new TH1I("EPPairFromPi0DetectionEfficiency","e^{+}e^{-} cand. pair from same #pi^{0} detection efficiency",8,0.5,8.5);
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetXaxis()->SetBinLabel(1,"#pi^{0}_{MC}#rightarrow #gamma#gamma#rightarrow 2e^{+}2e^{-}");     
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetXaxis()->SetBinLabel(2,"1 e^{+}#vee e^{-}");   
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetXaxis()->SetBinLabel(3,"2 e^{+}#vee e^{-}");    
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetXaxis()->SetBinLabel(4,"3 e^{+}#vee e^{-}");
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetXaxis()->SetBinLabel(5,"4 e^{+}#vee e^{-}");
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetXaxis()->SetBinLabel(6,"#pi^{0}_{MC}#rightarrow e^{+}e^{-}#gamma");
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetXaxis()->SetBinLabel(7,"1 e^{+}#vee e^{-}");
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->GetXaxis()->SetBinLabel(8,"2 e^{+}#vee e^{-}");

  NiceHisto1(fHistoMap["EPPairFromPi0DetectionEfficiency"],2,20,1,"","");

  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"] = new TH1I("EPPairFromPi0DetectionEfficiencyAll","e^{+}e^{-} pair from same #pi^{0} detection efficiency",8,0.5,8.5);
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetXaxis()->SetBinLabel(1,"#pi^{0}_{MC}#rightarrow #gamma#gamma#rightarrow 2e^{+}2e^{-}");     
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetXaxis()->SetBinLabel(2,"1 e^{+}#vee e^{-}");   
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetXaxis()->SetBinLabel(3,"2 e^{+}#vee e^{-}");    
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetXaxis()->SetBinLabel(4,"3 e^{+}#vee e^{-}");
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetXaxis()->SetBinLabel(5,"4 e^{+}#vee e^{-}");
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetXaxis()->SetBinLabel(6,"#pi^{0}_{MC}#rightarrow e^{+}e^{-}#gamma");
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetXaxis()->SetBinLabel(7,"1 e^{+}#vee e^{-}");
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->GetXaxis()->SetBinLabel(8,"2 e^{+}#vee e^{-}");

  NiceHisto1(fHistoMap["EPPairFromPi0DetectionEfficiencyAll"],1,24,1,"","");

  NiceHisto1(fHistoMap["MCPid_global"],1,20,1,"","");
  NiceHisto1(fHistoMap["GT_MC_PID"],1,20,1,"STS track MC PID for global tracks","");
  NiceHisto1(fHistoMap["MCPid_inMagnet"],2,20,1,"","");
  NiceHisto1(fHistoMap["MCPid_inTarget"],3,20,1,"","");
  NiceHisto1(fHistoMap["GTPid"],1,20,1,"","");
  NiceHisto2((TH2*)fHistoMap["Pt_global"],1,1,1,"p_{T} [Gev/c]","","");
  NiceHisto2((TH2*)fHistoMap["P_global"],1,1,1,"p [Gev/c]","","");
  NiceHisto2((TH2*)fHistoMap["Pt_inMagnet"],1,1,1,"p_{T} [Gev/c]","","");
  NiceHisto2((TH2*)fHistoMap["P_inMagnet"],1,1,1,"p [Gev/c]","","");
  NiceHisto2((TH2*)fHistoMap["Pt_inTarget"],1,1,1,"p_{T} [Gev/c]","","");
  NiceHisto2((TH2*)fHistoMap["P_inTarget"],1,1,1,"p [Gev/c]","","");
  NiceHisto2((TH2*)fHistoMap["Pt_OpenAngle_global"],1,1,1,"p_{T} [Gev/c]","Opening angle #theta [#circ]","");
  NiceHisto2((TH2*)fHistoMap["Pt_OpenAngle_inMagnet"],1,1,1,"p_{T} [Gev/c]","Opening angle #theta [#circ]","");
  NiceHisto2((TH2*)fHistoMap["Pt_OpenAngle_inTarget"],1,1,1,"p_{T} [Gev/c]","Opening angle #theta [#circ]","");
  NiceHisto2((TH2*)fHistoMap["Pt_OpenAngle_inTarget_EPPairsPi0"],1,1,1,"p_{T} [Gev/c]","Opening angle #theta [#circ]","");
  NiceHisto2((TH2*)fHistoMap["Pt_OpenAngle_inTarget_EPPairsGamma"],1,1,1,"p_{T} [Gev/c]","Opening angle #theta [#circ]","");
  NiceHisto1(fHistoMap["ePlusMinusMother"],1,20,1,"","");
  NiceHisto1(fHistoMap["gammaMother"],1,20,1,"","");
  NiceHisto1(fHistoMap["gammaDaughter"],1,20,1,"","");
  NiceHisto1(fHistoMap["ZBirth[0]"],1,20,1,"z-position [cm]","");
  NiceHisto1(fHistoMap["ZBirth[1]"],2,20,1,"z-position [cm]","");
  NiceHisto1(fHistoMap["ZBirth[2]"],3,20,1,"z-position [cm]","");
  NiceHisto1(fHistoMap["ZBirth[3]"],4,20,1,"z-position [cm]","");
  NiceHisto1(fHistoMap["PairHistory"],1,20,1,"","");
  NiceHisto2((TH2*)fHistoMap["NoDaughters_global"],1,1,1,"","Number of daughters","");
  NiceHisto2((TH2*)fHistoMap["NoDaughters_inMagnet"],1,1,1,"","Number of daughters","");
  NiceHisto2((TH2*)fHistoMap["NoDaughters_inTarget"],1,1,1,"","Number of daughters","");
  NiceHisto2((TH2*)fHistoMap["ZBirthAll"],1,1,1,"z-position [cm]","","#");
  NiceHisto2((TH2*)fHistoMap["MotherDaughter_global"],1,1,1,"Mother","Daughter","#");
  NiceHisto2((TH2*)fHistoMap["MotherDaughter_inMagnet"],1,1,1,"Mother","Daughter","#");
  NiceHisto2((TH2*)fHistoMap["MotherDaughter_inTarget"],1,1,1,"Mother","Daughter","#");
  NiceHisto2((TH2*)fHistoMap["motherGrani_gamma_global"],1,1,1,"Mother","Grandmother","#");
  NiceHisto2((TH2*)fHistoMap["motherGrani_posi_global"],1,1,1,"Mother","Grandmother","#");
  NiceHisto2((TH2*)fHistoMap["motherGrani_elec_global"],1,1,1,"Mother","Grandmother","#");
  NiceHisto2((TH2*)fHistoMap["motherGrani_gamma_inMagnet"],1,1,1,"Mother","Grandmother","#");
  NiceHisto2((TH2*)fHistoMap["motherGrani_posi_inMagnet"],1,1,1,"Mother","Grandmother","#");
  NiceHisto2((TH2*)fHistoMap["motherGrani_elec_inMagnet"],1,1,1,"Mother","Grandmother","#");
  NiceHisto2((TH2*)fHistoMap["motherGrani_gamma_inTarget"],1,1,1,"Mother","Grandmother","#");
  NiceHisto2((TH2*)fHistoMap["motherGrani_posi_inTarget"],1,1,1,"Mother","Grandmother","#");
  NiceHisto2((TH2*)fHistoMap["motherGrani_elec_inTarget"],1,1,1,"Mother","Grandmother","#");
  NiceHisto3((TH3*)fBirthPi0,1,7,1,"x-position [cm]","y-position [cm]","z-position [cm]");
  NiceHisto3((TH3*)fBirthGamma,2,7,1,"x-position [cm]","y-position [cm]","z-position [cm]");
  NiceHisto3((TH3*)fBirthPair,3,7,1,"x-position [cm]","y-position [cm]","z-position [cm]");
  //NiceHisto3((TH3*)fHistoMap["MotherDaughterZBirth"],1,1,1,"z-position [cm]","","");

  NiceHisto1(fHistoMap["EPPairVertexDistance_global"],1,1,1,"vertex distance [cm]","");
  NiceHisto1(fHistoMap["EPPairVertexDistance_inMagnet"],2,1,1,"vertex distance [cm]","");
  NiceHisto1(fHistoMap["EPPairVertexDistance_inTarget"],3,1,1,"vertex distance [cm]","");

  NiceHisto2((TH2*)fHistoMap["InvMPairMother"],1,1,1,"Invariant mass [GeV/c^{2}]","Mother","");
  NiceHisto2((TH2*)fHistoMap["PtPairMother"],1,1,1,"p_{T} [Gev/c]","Mother","");
  NiceHisto2((TH2*)fHistoMap["PPairMother"],1,1,1,"p [Gev/c]","Mother","");
  NiceHisto2((TH2*)fHistoMap["OpenAnglePairMother"],1,1,1,"opening angle #theta [#circ]","Mother","");


  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsInTargetPt"],1,1,1,"Invariant mass [GeV/c^{2}]","p_{T} [Gev/c]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsInMagnetPt"],1,1,1,"Invariant mass [GeV/c^{2}]","p_{T} [Gev/c]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsPt"],1,1,1,"Invariant mass [GeV/c^{2}]","p_{T} [Gev/c]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsInTargetP"],1,1,1,"Invariant mass [GeV/c^{2}]","p [Gev/c]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsInMagnetP"],1,1,1,"Invariant mass [GeV/c^{2}]","p [Gev/c]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsP"],1,1,1,"Invariant mass [GeV/c^{2}]","p [Gev/c]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsInTargetOA"],1,1,1,"Invariant mass [GeV/c^{2}]","pair opening angle #theta [#circ]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsInMagnetOA"],1,1,1,"Invariant mass [GeV/c^{2}]","pair opening angle #theta [#circ]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsOA"],1,1,1,"Invariant mass [GeV/c^{2}]","#gamma pair opening angle #theta [#circ]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsInTargetVD"],1,1,1,"Invariant mass [GeV/c^{2}]","#gamma pair vertex distance [cm]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsInMagnetVD"],1,1,1,"Invariant mass [GeV/c^{2}]","#gamma pair vertex distance [cm]",""); // new
  NiceHisto2((TH2*)fHistoMap["InvMassSpectrumGammaEPPairsVD"],1,1,1,"Invariant mass [GeV/c^{2}]","#gamma pair vertex distance [cm]","");

  NiceHisto1(fHistoMap["InvMassSpectrumGammaAllPairs"],1,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumGammaTruePairs"],2,1,1,"Invariant mass [GeV/c^{2}]","");

  NiceHisto1(fHistoMap["InvMassSpectrumAllEPPairs"],1,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumEPPairsInMagnet"],2,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumGammaEPPairsInMagnetBackground"],800,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumEPPairsInTarget"],3,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumTrueEPPairs"],4,1,1,"Invariant mass [GeV/c^{2}]","");

  NiceHisto1(fHistoMap["InvMassSpectrumGammaEPPairs"],1,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"],2,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumGammaEPPairsInTarget"],3,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"],4,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumGammaEPPairsGamma"],6,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumGammaEPPairsPi0"],7,1,1,"Invariant mass [GeV/c^{2}]","");

  NiceHisto1(fHistoMap["EPPairOpeningAngle"],1,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fHistoMap["EPPairOpeningAngleInMagnet"],2,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fHistoMap["EPPairOpeningAngleInTarget"],3,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fHistoMap["EPPairOpeningAngleSameMother"],4,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fHistoMap["EPPairOpeningAngleGamma"],6,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fHistoMap["EPPairOpeningAnglePi0"],7,1,1,"opening angle #theta [#circ]","");

  NiceHisto1(fHistoMap["PairOpeningAngleAll"],1,1,1,"opening angle #theta cut [#circ]","pair fraction");
  NiceHisto1(fHistoMap["PairOpeningAngleGamma"],2,1,1,"opening angle #theta cut [#circ]","pair fraction");
  NiceHisto1(fHistoMap["PairOpeningAnglePi0"],3,1,1,"opening angle #theta cut [#circ]","pair fraction");
  NiceHisto1(fHistoMap["PairOpeningAngleGammaWoPi0"],4,1,1,"opening angle #theta cut [#circ]","pair fraction");
  NiceHisto1(fHistoMap["PidWkn"],1,1,1,"PID Wkn","");
  NiceHisto1(fHistoMap["PidANN"],1,1,1,"Pid ANN","");
  NiceHisto1(fHistoMap["PidLikeEL"],1,1,1,"Likelihood EL","");
  NiceHisto1(fHistoMap["PidLikePI"],1,1,1,"Likelihood Pi","");
  NiceHisto1(fHistoMap["PidLikeKA"],1,1,1,"Likelihood Ka","");
  NiceHisto1(fHistoMap["PidLikePR"],1,1,1,"Likelihood Pr","");
  NiceHisto1(fHistoMap["PidLikeMU"],1,1,1,"Likelihood Mu","");
  NiceHisto1(fHistoMap["GT_Qp_first"],1,1,1,"Q/p [1/GeV/c]","");
  NiceHisto1(fHistoMap["GT_Qp_last"],1,1,1,"Q/p [1/GeV/c]","");
  NiceHisto1(fHistoMap["GT_Vertex_x_first"],1,1,1,"x-position [cm]","");
  NiceHisto1(fHistoMap["GT_Vertex_y_first"],1,1,1,"y-position [cm]","");
  NiceHisto1(fHistoMap["GT_Vertex_z_first"],1,1,1,"z-position [cm]","");
  NiceHisto1(fHistoMap["GT_Vertex_x_last"],1,1,1,"x-position [cm]","");
  NiceHisto1(fHistoMap["GT_Vertex_y_last"],1,1,1,"y-position [cm]","");
  NiceHisto1(fHistoMap["GT_Vertex_z_last"],1,1,1,"z-position [cm]","");

  NiceHisto1(fHistoMap["KF_PrimVertex_x"],1,1,1,"x-position [cm]","");
  NiceHisto1(fHistoMap["KF_PrimVertex_y"],1,1,1,"y-position [cm]","");
  NiceHisto1(fHistoMap["KF_PrimVertex_z"],1,1,1,"z-position [cm]","");
  NiceHisto1(fHistoMap["KF_P"],1,1,1,"p [1/GeV/c]","");
  NiceHisto1(fHistoMap["KF_Pt"],1,1,1,"p_{T} [1/GeV/c]","");
  NiceHisto1(fHistoMap["KF_E"],1,1,1,"E [GeV]","");
  NiceHisto2((TH2*)fHistoMap["KF_PID_MC_PID"],1,1,1,"MC","KF ANN","");

  NiceHisto2((TH2*)fHistoMap["DeltaP"],1,1,1,"p_{MC} [GeV/c]","#Delta p/p_{MC} [%]","");
  NiceHisto2((TH2*)fHistoMap["DeltaPt"],1,1,1,"p_{T,MC} [GeV/c]","#Delta p_{T}/p_{T,MC} [%]","");
  NiceHisto2((TH2*)fHistoMap["DeltaPGT_PKF"],1,1,1,"p_{MC} [GeV/c]","#Delta p_{GT} - p_{KF} [GeV/c]","");

  NiceHisto2((TH2*)fHistoMap["TRD_GT_dEdx_KF_P"],1,1,1,"p_{MC} [GeV/c]","dE/dx + TR [keV]","");
  NiceHisto2((TH2*)fHistoMap["RICH_GT_radiusA_KF_P"],1,1,1,"p_{MC} [GeV/c]","radius r [cm]","");
  NiceHisto2((TH2*)fHistoMap["RICH_GT_radiusB_KF_P"] ,1,1,1,"p_{MC} [GeV/c]","radius r [cm]","");
  NiceHisto2((TH2*)fHistoMap["TOF_GT_time_KF_P"] ,1,1,1,"p_{MC} [GeV/c]","#beta","");//"time of flight [ns]"

  NiceHisto1(fHistoMap["EPCandPairOpeningAngle"],1,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fHistoMap["CandPairOpeningAngle"],2,1,1,"opening angle #theta [#circ]","");
  NiceHisto1(fHistoMap["InvMassSpectrumEPCandPairs"],1,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumCandPairs"],2,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumGammaEPCandPairs"],3,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumGammaCandPairs"],1,1,1,"Invariant mass [GeV/c^{2}]","");
  NiceHisto1(fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"],4,1,1,"Invariant mass [GeV/c^{2}]","");
}
    void  CbmTrdPhotonAnalysis::NormalizeHistos(TH1* h)
    {
      Int_t nEntries = h->GetEntries();
      h->Scale(1./ Float_t(nEntries));
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
  gDirectory->Cd("..");
  fHistoMap["MC_EventCounter"]->Write("", TObject::kOverwrite);
  gDirectory->pwd();
  if (!gDirectory->Cd("PhotonAnalysis")) 
    gDirectory->mkdir("PhotonAnalysis");
  gDirectory->Cd("PhotonAnalysis");
  gDirectory->pwd();

  if (!gDirectory->Cd("MC")) 
    gDirectory->mkdir("MC");
  gDirectory->Cd("MC");
  gDirectory->pwd();
  //===============================================================MC_Statistic
  if (!gDirectory->Cd("MC_Statistic")) 
    gDirectory->mkdir("MC_Statistic");
  gDirectory->Cd("MC_Statistic");
  gDirectory->pwd();
  fHistoMap["MCPid_global"]->Write("", TObject::kOverwrite);
  fHistoMap["MCPid_inMagnet"]->Write("", TObject::kOverwrite);
  fHistoMap["MCPid_inTarget"]->Write("", TObject::kOverwrite);

  fHistoMap["Pt_global"]->Write("", TObject::kOverwrite);
  fHistoMap["P_global"]->Write("", TObject::kOverwrite);
  fHistoMap["Pt_inMagnet"]->Write("", TObject::kOverwrite);
  fHistoMap["P_inMagnet"]->Write("", TObject::kOverwrite);
  fHistoMap["Pt_inTarget"]->Write("", TObject::kOverwrite);
  fHistoMap["P_inTarget"]->Write("", TObject::kOverwrite);  
  fHistoMap["InvMPairMother"]->Write("", TObject::kOverwrite);
  fHistoMap["PtPairMother"]->Write("", TObject::kOverwrite);
  fHistoMap["PPairMother"]->Write("", TObject::kOverwrite);
  fHistoMap["OpenAnglePairMother"]->Write("", TObject::kOverwrite);
  fHistoMap["InvMPairSameMother"]->Write("", TObject::kOverwrite);
  fHistoMap["PtPairSameMother"]->Write("", TObject::kOverwrite);
  fHistoMap["PPairSameMother"]->Write("", TObject::kOverwrite);
  fHistoMap["OpenAnglePairSameMother"]->Write("", TObject::kOverwrite);
  fHistoMap["PairHistory"]->Write("", TObject::kOverwrite);
  //==================================================================Relationships
  if (!gDirectory->Cd("Relationships")) 
    gDirectory->mkdir("Relationships");
  gDirectory->Cd("Relationships");
  gDirectory->pwd(); 
  fHistoMap["ePlusMinusMother"]->Write("", TObject::kOverwrite);
  fHistoMap["ePlusAndMinusMother"]->Write("", TObject::kOverwrite);
  fHistoMap["gammaMother"]->Write("", TObject::kOverwrite);
  fHistoMap["gammaAndGammaMother"]->Write("", TObject::kOverwrite);
  fHistoMap["gammaDaughter"]->Write("", TObject::kOverwrite);
  fHistoMap["DalizMother"]->Write("", TObject::kOverwrite);
  fHistoMap["MotherDaughter_global"]->Write("", TObject::kOverwrite);
  fHistoMap["MotherDaughter_inMagnet"]->Write("", TObject::kOverwrite);
  fHistoMap["MotherDaughter_inTarget"]->Write("", TObject::kOverwrite);
  fHistoMap["NoDaughters_global"]->Write("", TObject::kOverwrite);
  fHistoMap["NoDaughters_inMagnet"]->Write("", TObject::kOverwrite);
  fHistoMap["NoDaughters_inTarget"]->Write("", TObject::kOverwrite);
  fHistoMap["motherGrani_gamma_global"]->Write("", TObject::kOverwrite);
  fHistoMap["motherGrani_posi_global"]->Write("", TObject::kOverwrite);
  fHistoMap["motherGrani_elec_global"]->Write("", TObject::kOverwrite);
  fHistoMap["motherGrani_gamma_inMagnet"]->Write("", TObject::kOverwrite);
  fHistoMap["motherGrani_posi_inMagnet"]->Write("", TObject::kOverwrite); 
  fHistoMap["motherGrani_elec_inMagnet"]->Write("", TObject::kOverwrite);
  fHistoMap["motherGrani_gamma_inTarget"]->Write("", TObject::kOverwrite);
  fHistoMap["motherGrani_posi_inTarget"]->Write("", TObject::kOverwrite);
  fHistoMap["motherGrani_elec_inTarget"]->Write("", TObject::kOverwrite);
  gDirectory->Cd("..");
  //==================================================================Relationships
  //==================================================================Vertizes
  if (!gDirectory->Cd("Vertizes")) 
    gDirectory->mkdir("Vertizes");
  gDirectory->Cd("Vertizes");
  gDirectory->pwd();
  fHistoMap["ZBirthAll"]->Write("", TObject::kOverwrite);
  fHistoMap["ZBirthEPfromGamma"]->Write("", TObject::kOverwrite);
  //for (Int_t i = 0; i < 4; i++) {
  //fZBirth[i]"]->Write("", TObject::kOverwrite);
  //}
  fBirthGamma->Write("", TObject::kOverwrite);
  fBirthPi0->Write("", TObject::kOverwrite);
  fBirthPair->Write("", TObject::kOverwrite);
  TString coo[3] = {"x","y","z"};
  for (Int_t i = 0; i < 3; i++) {
    fHistoMap["PairAllVertex_"+coo[i]]->Write("", TObject::kOverwrite);
    fHistoMap["PairGammaVertex_"+coo[i]]->Write("", TObject::kOverwrite);
    fHistoMap["PairPi0Vertex_"+coo[i]]->Write("", TObject::kOverwrite);

    fHistoMap["pi02GammaVertex_"+coo[i]]->Write("", TObject::kOverwrite);
    fHistoMap["gamma2PairVertex_"+coo[i]]->Write("", TObject::kOverwrite);
  }
  fHistoMap["EPPairVertexDistance_global"]->Write("", TObject::kOverwrite);
  fHistoMap["EPPairVertexDistance_inMagnet"]->Write("", TObject::kOverwrite);
  fHistoMap["EPPairVertexDistance_inTarget"]->Write("", TObject::kOverwrite);
  gDirectory->Cd("..");
  //==================================================================Vertizes
  gDirectory->Cd("..");
  //===============================================================MC_Statistic
  //==================================================================InvMassSpectrum
  if (!gDirectory->Cd("InvMassSpectrum_GammaPairs")) 
    gDirectory->mkdir("InvMassSpectrum_GammaPairs");
  gDirectory->Cd("InvMassSpectrum_GammaPairs");
  gDirectory->pwd();
  //==================================================================GammaPairs
  if (!gDirectory->Cd("GammaPairs")) 
    gDirectory->mkdir("GammaPairs");
  gDirectory->Cd("GammaPairs");
  gDirectory->pwd();
  //NormalizeHistos(fInvMassSpectrumGammaAllPairs);
  fHistoMap["InvMassSpectrumGammaAllPairs"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumGammaTruePairs);
  fHistoMap["InvMassSpectrumGammaTruePairs"]->Write("", TObject::kOverwrite);
  //==================================================================FromEPPairs
  if (!gDirectory->Cd("FromEPPairs")) 
    gDirectory->mkdir("FromEPPairs");
  gDirectory->Cd("FromEPPairs");
  gDirectory->pwd();
  //NormalizeHistos(fInvMassSpectrumGammaEPPairs);
  fHistoMap["InvMassSpectrumGammaEPPairs"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumGammaEPPairsInMagnet);
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"]->Write("", TObject::kOverwrite);
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnetBackground"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumGammaEPPairsInTarget);
  fHistoMap["InvMassSpectrumGammaEPPairsInTarget"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumGammaEPPairsOpenAngle);
  fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"]->Write("", TObject::kOverwrite);
  fHistoMap["InvMassSpectrumGammaEPPairsGamma"]->Write("", TObject::kOverwrite);
  fHistoMap["InvMassSpectrumGammaEPPairsPi0"]->Write("", TObject::kOverwrite);
  //==================================================================Cuts
  if (!gDirectory->Cd("Cuts")) 
    gDirectory->mkdir("Cuts");
  gDirectory->Cd("Cuts");
  gDirectory->pwd();
  fHistoMap["InvMassSpectrumGammaEPPairsInTargetPt"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnetPt"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsPt"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInTargetP"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnetP"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsP"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInTargetOA"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnetOA"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsOA"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInTargetVD"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsInMagnetVD"]->Write("", TObject::kOverwrite); // new
  fHistoMap["InvMassSpectrumGammaEPPairsVD"]->Write("", TObject::kOverwrite);
  fHistoMap["Pt_OpenAngle_global"]->Write("", TObject::kOverwrite);
  fHistoMap["Pt_OpenAngle_inMagnet"]->Write("", TObject::kOverwrite);
  fHistoMap["Pt_OpenAngle_inTarget"]->Write("", TObject::kOverwrite);
  fHistoMap["Pt_OpenAngle_inTarget_EPPairsPi0"]->Write("", TObject::kOverwrite); 
  fHistoMap["Pt_OpenAngle_inTarget_EPPairsGamma"]->Write("", TObject::kOverwrite); 
  gDirectory->Cd("..");
  //==================================================================Cuts
  gDirectory->Cd("..");
  //==================================================================FromEPPairs
  gDirectory->Cd("..");
  //==================================================================GammaPairs
  //==================================================================EPPairs
  if (!gDirectory->Cd("EPPairs")) 
    gDirectory->mkdir("EPPairs");
  gDirectory->Cd("EPPairs");
  gDirectory->pwd();
  //NormalizeHistos(fInvMassSpectrumAllEPPairs);
  fHistoMap["InvMassSpectrumAllEPPairs"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumEPPairsInTarget);
  fHistoMap["InvMassSpectrumEPPairsInTarget"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumEPPairsInMagnet);
  fHistoMap["InvMassSpectrumEPPairsInMagnet"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fInvMassSpectrumTrueEPPairs);
  fHistoMap["InvMassSpectrumTrueEPPairs"]->Write("", TObject::kOverwrite);
  gDirectory->Cd("..");
  //==================================================================EPPairs
  gDirectory->Cd("..");
  //==================================================================InvMassSpectrum
  //==================================================================OpeningAngle_EPPairs
  if (!gDirectory->Cd("OpeningAngle_EPPairs")) 
    gDirectory->mkdir("OpeningAngle_EPPairs");
  gDirectory->Cd("OpeningAngle_EPPairs");
  gDirectory->pwd();
  //NormalizeHistos(fEPPairOpeningAngle);
  fHistoMap["EPPairOpeningAngle"]->Write("", TObject::kOverwrite);
 
  //NormalizeHistos(fEPPairOpeningAngleInMagnet);
  fHistoMap["EPPairOpeningAngleInMagnet"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fEPPairOpeningAngleInTarget);
  fHistoMap["EPPairOpeningAngleInTarget"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fEPPairOpeningAngleSameMother);
  fHistoMap["EPPairOpeningAngleSameMother"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fEPPairOpeningAngleGamma);
  fHistoMap["EPPairOpeningAngleGamma"]->Write("", TObject::kOverwrite);
  //NormalizeHistos(fEPPairOpeningAnglePi0);
  fHistoMap["EPPairOpeningAnglePi0"]->Write("", TObject::kOverwrite);

  fHistoMap["PairOpeningAngleAll"]->Write("", TObject::kOverwrite);
  fHistoMap["PairOpeningAngleGamma"]->Write("", TObject::kOverwrite);
  fHistoMap["PairOpeningAnglePi0"]->Write("", TObject::kOverwrite);
  fHistoMap["PairOpeningAngleGammaWoPi0"]->Write("", TObject::kOverwrite);
  gDirectory->Cd("..");
  //==================================================================OpeningAngle_EPPairs
  gDirectory->Cd("..");

  if (!gDirectory->Cd("Reco")) 
    gDirectory->mkdir("Reco");
  gDirectory->Cd("Reco");
  gDirectory->pwd();
  //===============================================================MC_Statistic
  if (!gDirectory->Cd("Statistic")) 
    gDirectory->mkdir("Statistic");
  gDirectory->Cd("Statistic");
  gDirectory->pwd();
  fHistoMap["GTPid"]->Write("", TObject::kOverwrite);
  fHistoMap["GT_MC_PID"]->Write("", TObject::kOverwrite);
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->Write("", TObject::kOverwrite);
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Write("", TObject::kOverwrite);
  fHistoMap["PidWkn"]->Write("", TObject::kOverwrite);
  fHistoMap["PidANN"]->Write("", TObject::kOverwrite);
  fHistoMap["PidLikeEL"]->Write("", TObject::kOverwrite);
  fHistoMap["PidLikePI"]->Write("", TObject::kOverwrite);
  fHistoMap["PidLikeKA"]->Write("", TObject::kOverwrite);
  fHistoMap["PidLikePR"]->Write("", TObject::kOverwrite);
  fHistoMap["PidLikeMU"]->Write("", TObject::kOverwrite);
  fHistoMap["GT_Qp_first"]->Write("", TObject::kOverwrite);
  fHistoMap["GT_Qp_last"]->Write("", TObject::kOverwrite);
  fHistoMap["DeltaP"]->Write("", TObject::kOverwrite);
  fHistoMap["DeltaPt"]->Write("", TObject::kOverwrite);
  fHistoMap["DeltaPGT_PKF"]->Write("", TObject::kOverwrite);
  //==================================================================Relationships
  if (!gDirectory->Cd("Relationships")) 
    gDirectory->mkdir("Relationships");
  gDirectory->Cd("Relationships");
  gDirectory->pwd(); 
  fHistoMap["KF_PID_MC_PID"]->Write("", TObject::kOverwrite);
  gDirectory->Cd("..");
  //==================================================================Relationships
  //==================================================================Vertizes
  if (!gDirectory->Cd("Vertizes")) 
    gDirectory->mkdir("Vertizes");
  gDirectory->Cd("Vertizes");
  gDirectory->pwd();
  fHistoMap["GT_Vertex_x_first"]->Write("", TObject::kOverwrite);
  fHistoMap["GT_Vertex_y_first"]->Write("", TObject::kOverwrite);
  fHistoMap["GT_Vertex_z_first"]->Write("", TObject::kOverwrite);
  fHistoMap["GT_Vertex_x_last"]->Write("", TObject::kOverwrite);
  fHistoMap["GT_Vertex_y_last"]->Write("", TObject::kOverwrite);
  fHistoMap["GT_Vertex_z_last"]->Write("", TObject::kOverwrite);
  fHistoMap["KF_PrimVertex_x"]->Write("", TObject::kOverwrite);
  fHistoMap["KF_PrimVertex_y"]->Write("", TObject::kOverwrite);
  fHistoMap["KF_PrimVertex_z"]->Write("", TObject::kOverwrite);
  fHistoMap["KF_P"]->Write("", TObject::kOverwrite);
  fHistoMap["KF_Pt"]->Write("", TObject::kOverwrite);
  fHistoMap["KF_E"]->Write("", TObject::kOverwrite);

  gDirectory->Cd("..");
  //==================================================================Vertizes
  gDirectory->Cd("..");
  //===============================================================MC_Statistic
  //==================================================================InvMassSpectrum
  if (!gDirectory->Cd("InvMassSpectrum_GammaPairs")) 
    gDirectory->mkdir("InvMassSpectrum_GammaPairs");
  gDirectory->Cd("InvMassSpectrum_GammaPairs");
  gDirectory->pwd();
  //==================================================================GammaPairs
  if (!gDirectory->Cd("GammaPairs")) 
    gDirectory->mkdir("GammaPairs");
  gDirectory->Cd("GammaPairs");
  gDirectory->pwd();

  //==================================================================FromEPPairs
  if (!gDirectory->Cd("FromEPPairs")) 
    gDirectory->mkdir("FromEPPairs");
  gDirectory->Cd("FromEPPairs");
  gDirectory->pwd(); 
  fHistoMap["InvMassSpectrumGammaEPCandPairs"]->Write("", TObject::kOverwrite);
  fHistoMap["InvMassSpectrumGammaCandPairs"]->Write("", TObject::kOverwrite);
  fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"]->Write("", TObject::kOverwrite);
  //==================================================================Cuts
  if (!gDirectory->Cd("Cuts")) 
    gDirectory->mkdir("Cuts");
  gDirectory->Cd("Cuts");
  gDirectory->pwd();
  fHistoMap["TRD_GT_dEdx_KF_P"]->Write("", TObject::kOverwrite);
  fHistoMap["RICH_GT_radiusA_KF_P"]->Write("", TObject::kOverwrite);
  fHistoMap["RICH_GT_radiusB_KF_P"]->Write("", TObject::kOverwrite);
  fHistoMap["TOF_GT_time_KF_P"]->Write("", TObject::kOverwrite);
  gDirectory->Cd("..");
  //==================================================================Cuts
  gDirectory->Cd("..");
  //==================================================================FromEPPairs
  gDirectory->Cd("..");
  //==================================================================GammaPairs
  //==================================================================EPPairs
  if (!gDirectory->Cd("EPPairs")) 
    gDirectory->mkdir("EPPairs");
  gDirectory->Cd("EPPairs");
  gDirectory->pwd();
  fHistoMap["InvMassSpectrumEPCandPairs"]->Write("", TObject::kOverwrite);
  fHistoMap["InvMassSpectrumCandPairs"]->Write("", TObject::kOverwrite);
  gDirectory->Cd("..");
  //==================================================================EPPairs
  gDirectory->Cd("..");
  //==================================================================InvMassSpectrum
  //==================================================================OpeningAngle_EPPairs
  if (!gDirectory->Cd("OpeningAngle_EPPairs")) 
    gDirectory->mkdir("OpeningAngle_EPPairs");
  gDirectory->Cd("OpeningAngle_EPPairs");
  gDirectory->pwd();
  fHistoMap["EPCandPairOpeningAngle"]->Write("", TObject::kOverwrite);
  fHistoMap["CandPairOpeningAngle"]->Write("", TObject::kOverwrite);
  gDirectory->Cd("..");
  //==================================================================OpeningAngle_EPPairs
  gDirectory->Cd("..");

  //outFile->Close();
  //c->Close();
}

    void CbmTrdPhotonAnalysis::FinishEvent()
    {

    }

void CbmTrdPhotonAnalysis::FinishTask()
{
  TCanvas *c = new TCanvas("c","c",800,600);
  c->Divide(1,1);
  c->cd(1);
  Bool_t debug = false;

  for (fHistoMapIt = fHistoMap.begin(); fHistoMapIt != fHistoMap.end(); fHistoMapIt++){
    if (fHistoMapIt->second->InheritsFrom("TH2")) {
      c->cd(1)->SetLogx(0);
      c->cd(1)->SetLogy(0);
      c->cd(1)->SetLogz(1);
      fHistoMapIt->second->SetContour(99);
      if (debug)
	fHistoMapIt->second->Draw("colz");
    }
    else {
      c->cd(1)->SetLogx(0);
      c->cd(1)->SetLogy(1);
      c->cd(1)->SetLogz(0);
      if (debug)
	fHistoMapIt->second->Draw("");
    }
    if (debug){
      TString name = fHistoMapIt->second->GetName();
      c->SaveAs(TString("pics/Photon/" + name + ".pdf"));
      c->SaveAs(TString("pics/Photon/" + name + ".png"));
    }
  }
  
  c->cd(1)->SetLogx(0);
  c->cd(1)->SetLogy(0);
  c->cd(1)->SetLogz(0);
  fBirthGamma->Draw("");
  TString name = fBirthGamma->GetName();
  c->SaveAs(TString("pics/Photon/PhD/" + name + ".pdf"));
  c->SaveAs(TString("pics/Photon/PhD/" + name + ".png"));
  fBirthPi0->Draw("");
  name = fBirthPi0->GetName();
  c->SaveAs(TString("pics/Photon/PhD/" + name + ".pdf"));
  c->SaveAs(TString("pics/Photon/PhD/" + name + ".png"));
  fBirthPair->Draw("");
  name = fBirthPair->GetName();
  c->SaveAs(TString("pics/Photon/PhD/" + name + ".pdf"));
  c->SaveAs(TString("pics/Photon/PhD/" + name + ".png"));

  fHistoMap["InvMassSpectrumGammaEPPairsInMagnet"]->Draw();
  fHistoMap["InvMassSpectrumGammaEPPairsInTarget"]->Draw("same");
  fHistoMap["InvMassSpectrumGammaEPPairsOpenAngle"]->Draw("same");
  c->SaveAs("pics/Photon/PhD/MC_InvM_fromGamma_fromEPPair.pdf");
  c->SaveAs("pics/Photon/PhD/MC_InvM_fromGamma_fromEPPair.png");
  fHistoMap["InvMassSpectrumGammaAllPairs"]->Draw();
  c->SaveAs("pics/Photon/PhD/MC_InvM_fromGamma.pdf");
  c->SaveAs("pics/Photon/PhD/MC_InvM_fromGamma.png");

  fHistoMap["InvMassSpectrumGammaCandPairs"]->Draw();
  fHistoMap["InvMassSpectrumGammaEPCandPairs"]->Draw("same"); 
  fHistoMap["InvMassSpectrumGammaEPCandPairsOpenAngle"]->Draw("same"); 
  c->SaveAs("pics/Photon/PhD/KF_InvM_fromGamma_fromEPPair.pdf");
  c->SaveAs("pics/Photon/PhD/KF_InvM_fromGamma_fromEPPair.png");

  fHistoMap["gammaAndGammaMother"]->Draw();
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_gamma_and_gamma.pdf");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_gamma_and_gamma.png");

  fHistoMap["gammaMother"]->Draw();
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_gamma.pdf");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_gamma.png");




  c->cd(1)->SetLogx(0);
  c->cd(1)->SetLogy(1);
  c->cd(1)->SetLogz(0);
  fHistoMap["EPPairFromPi0DetectionEfficiencyAll"]->Draw("PE");
  fHistoMap["EPPairFromPi0DetectionEfficiency"]->Draw("PE,same");
  c->SaveAs("pics/Photon/PhD/EPPairFromPi0DetectionEfficiency.pdf");
  c->SaveAs("pics/Photon/PhD/EPPairFromPi0DetectionEfficiency.png");

  fHistoMap["GT_MC_PID"]->Draw("PE");
  c->SaveAs("pics/Photon/PhD/GT_MC_PID.pdf");
  c->SaveAs("pics/Photon/PhD/GT_MC_PID.png");

  fHistoMap["gammaAndGammaMother"]->SetLineColor(2);
  fHistoMap["gammaAndGammaMother"]->SetMarkerColor(2);
  fHistoMap["gammaAndGammaMother"]->Draw("PE");
  fHistoMap["gammaMother"]->Draw("PE,same");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_gamma_andOr_gamma.pdf");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_gamma_andOr_gamma.png");

  fHistoMap["ePlusMinusMother"]->Draw("PE");
  fHistoMap["ePlusAndMinusMother"]->SetLineColor(2);
  fHistoMap["ePlusAndMinusMother"]->SetMarkerColor(2);
  fHistoMap["ePlusAndMinusMother"]->Draw("PE,same");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_elec_andOr_posi.pdf");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_elec_andOr_posi.png");

  fHistoMap["ePlusMinusMother"]->Draw();
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_elec_or_posi.pdf");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_elec_or_posi.png");
  fHistoMap["ePlusAndMinusMother"]->Draw();
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_elec_and_posi.pdf");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_elec_and_posi.png");

  fHistoMap["MCPid_global"]->Draw("PE");
  fHistoMap["MCPid_inMagnet"]->Draw("PE,same");
  fHistoMap["MCPid_inTarget"]->Draw("PE,same");
  c->SaveAs("pics/Photon/PhD/MC_PID.pdf");
  c->SaveAs("pics/Photon/PhD/MC_PID.png");

  fHistoMap["PairGammaVertex_z"]->Draw();
  fHistoMap["PairPi0Vertex_z"]->Draw("same");
  c->SaveAs("pics/Photon/PhD/MC_zBirth_ep_from_gamma_pi0.pdf");
  c->SaveAs("pics/Photon/PhD/MC_zBirth_ep_from_gamma_pi0.png");
  fHistoMap["EPPairOpeningAngle"]->Draw();
  fHistoMap["EPPairOpeningAnglePi0"]->Draw("same");
  fHistoMap["EPPairOpeningAngleGamma"]->Draw("same");
  c->SaveAs("pics/Photon/PhD/MC_openingAngle_sameMother_Gamma_Pi0.pdf");
  c->SaveAs("pics/Photon/PhD/MC_openingAngle_sameMother_Gamma_Pi0.png");

  c->cd(1)->SetLogx(1);
  c->cd(1)->SetLogy(1);
  c->cd(1)->SetLogz(0);
  fHistoMap["ZBirthEPfromGamma"]->Draw();
  c->SaveAs("pics/Photon/PhD/MC_zBirth_epPairs_motherGamma.pdf");
  c->SaveAs("pics/Photon/PhD/MC_zBirth_epPairs_motherGamma.png");

  c->cd(1)->SetLogx(0);
  c->cd(1)->SetLogy(0);
  c->cd(1)->SetLogz(1);

  fHistoMap["Pt_global"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_PT_PID_global.pdf");
  c->SaveAs("pics/Photon/PhD/MC_PT_PID_global.png");
  fHistoMap["P_global"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_P_PID_global.pdf");
  c->SaveAs("pics/Photon/PhD/MC_P_PID_global.png");
  fHistoMap["Pt_inMagnet"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_PT_PID_Magnet.pdf");
  c->SaveAs("pics/Photon/PhD/MC_PT_PID_Magnet.png");
  fHistoMap["P_inMagnet"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_P_PID_Magnet.pdf");
  c->SaveAs("pics/Photon/PhD/MC_P_PID_Magnet.png");
  fHistoMap["Pt_inTarget"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_PT_PID_target.pdf");
  c->SaveAs("pics/Photon/PhD/MC_PT_PID_target.png");
  fHistoMap["P_inTarget"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_P_PID_target.pdf");
  c->SaveAs("pics/Photon/PhD/MC_P_PID_target.png");
  fHistoMap["MotherDaughter_global"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_DaughterPID.pdf");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_DaughterPID.png");
  fHistoMap["MotherDaughter_inMagnet"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_DaughterPID_magnet.pdf");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_DaughterPID_magnet.png");
  fHistoMap["MotherDaughter_inTarget"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_DaughterPID_target.pdf");
  c->SaveAs("pics/Photon/PhD/MC_MotherPID_DaughterPID_target.png");
  fHistoMap["ZBirthAll"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_zBirth_PID.pdf");
  c->SaveAs("pics/Photon/PhD/MC_zBirth_PID.png");

  fHistoMap["InvMPairMother"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_invM_elec_posi_MotherPID.pdf");
  c->SaveAs("pics/Photon/PhD/MC_invM_elec_posi_MotherPID.png");
  fHistoMap["PtPairMother"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_PT_elec_posi_MotherPID.pdf");
  c->SaveAs("pics/Photon/PhD/MC_PT_elec_posi_MotherPID.png");
  fHistoMap["PPairMother"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_P_elec_posi_MotherPID.pdf");
  c->SaveAs("pics/Photon/PhD/MC_P_elec_posi_MotherPID.png");
  fHistoMap["OpenAnglePairMother"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/MC_openingAngle_elec_posi_MotherPID.pdf");
  c->SaveAs("pics/Photon/PhD/MC_openingAngle_elec_posi_MotherPID.png");

  fHistoMap["TRD_GT_dEdx_KF_P"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/TRD_GT_dEdx_KF_P.pdf");
  c->SaveAs("pics/Photon/PhD/TRD_GT_dEdx_KF_P.png");
  fHistoMap["RICH_GT_radiusA_KF_P"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/RICH_GT_radiusA_KF_P.pdf");
  c->SaveAs("pics/Photon/PhD/RICH_GT_radiusA_KF_P.png");
  fHistoMap["RICH_GT_radiusB_KF_P"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/RICH_GT_radiusB_KF_P.pdf");
  c->SaveAs("pics/Photon/PhD/RICH_GT_radiusB_KF_P.png");
  fHistoMap["TOF_GT_time_KF_P"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/TOF_GT_time_KF_P.pdf");
  c->SaveAs("pics/Photon/PhD/TOF_GT_time_KF_P.png");

  fHistoMap["DeltaP"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/DeltaP.pdf");
  c->SaveAs("pics/Photon/PhD/DeltaP.png");
  fHistoMap["DeltaPt"] ->Draw("colz");
  c->SaveAs("pics/Photon/PhD/DeltaPt.pdf");
  c->SaveAs("pics/Photon/PhD/DeltaPt.png");
  fHistoMap["DeltaPGT_PKF"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/DeltaPGT_PKF.pdf");
  c->SaveAs("pics/Photon/PhD/DeltaPGT_PKF.png");
  fHistoMap["KF_PID_MC_PID"]->Draw("colz");
  c->SaveAs("pics/Photon/PhD/KF_PID_MC_PID.pdf");
  c->SaveAs("pics/Photon/PhD/KF_PID_MC_PID.png");



  c->Close();
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
    void CbmTrdPhotonAnalysis::Statusbar(Int_t i, Int_t n) {
      if (int(i * 100 / float(n)) - int((i-1) * 100 / float(n)) >= 1) {
	if (int(i * 100 / float(n)) == 1 || i == 1 || i == 0) 
	  cout << "[" << flush;
	cout << "-" << flush;
	if (int(i * 10 / float(n)) - int((i-1) * 10 / float(n)) >= 1) 
	  cout << "|";
	if (int(i * 100 / float(n)) >=99) 
	  cout << "]" <<endl;
      }
    }
