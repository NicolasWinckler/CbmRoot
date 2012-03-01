/*
 *====================================================================
 *
 *  CBM KF Track Quality
 *  
 *  Authors: M.Zyzak
 *
 *  e-mail : 
 *
 *====================================================================
 *
 *  KF Fit performance 
 *
 *====================================================================
 */
#include "CbmKFParticlesFinderQA.h"
#include "CbmKFParticlesFinder.h"

#include "CbmKF.h"
#include "CbmKFParticle.h"
#include "CbmKFVertex.h"

#include "CbmStsHit.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmStsCluster.h"
#include "CbmStsDigi.h"

#include "CbmVertex.h"
#include "CbmMCTrack.h"
#include "CbmMvdPoint.h"
#include "CbmMvdHitMatch.h"
#include "CbmStsPoint.h"

#include "CbmL1Counters.h"
#include "CbmL1PartEfficiencies.h"

#include "TClonesArray.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TAxis.h"

#include <iostream>
#include <algorithm>
#include <map>
#include <cmath>
using namespace std;
using std::vector;

ClassImp(CbmKFParticlesFinderQA)

CbmKFParticlesFinderQA::CbmKFParticlesFinderQA(CbmKFParticlesFinder *pf, Int_t iVerbose, int findParticlesMode, int perf, const char *name, const char *title):
  FairTask(name,iVerbose),
  fFindParticlesMode(findParticlesMode),
  fPerformance(perf),
  fMinNStations(4),
  fMinRecoMom(0.1),
  fPF(pf),
  fMCTrackPoints(),
  vMCParticles(),
  MCtoRParticleId(),
  RtoMCParticleId(),
  flistStsPts(0),
  flistMvdPts(0),
  flistMCTracks(0),
  flistStsTracksMatch(0),
  flistStsTracks(0),
  flistStsHits(0),
  flistMvdHits(0),
  flistMvdHitMatches(0),
  flistStsClusters(0),
  flistStsDigi(0),
  fPrimVtx(0),
  outfileName("CbmParticlesFinderQA.root"),
  histodir(0),
  vStsHitMatch(),
  vStsPointMatch(),
  vMvdPointMatch(),
  vMCTrackMatch()
{
}

CbmKFParticlesFinderQA::~CbmKFParticlesFinderQA()
{
}

InitStatus CbmKFParticlesFinderQA::ReInit()
{
  return Init();
}

InitStatus CbmKFParticlesFinderQA::Init()
{
  FairRootManager *fManger = FairRootManager::Instance();

  flistMCTracks = dynamic_cast<TClonesArray*>( fManger->GetObject("MCTrack") );
  flistStsPts = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsPoint") );
  flistStsTracks = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsTrack") );
  flistStsTracksMatch = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsTrackMatch") );
  flistStsHits = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsHit") );
  flistStsClusters = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsCluster") );
  flistStsDigi = dynamic_cast<TClonesArray*>(  fManger->GetObject("StsDigi") );
  fPrimVtx = (CbmVertex*) fManger->GetObject("PrimaryVertex");

  if( CbmKF::Instance()->vMvdMaterial.size() == 0 ) 
  {
    flistMvdPts = 0;
    flistMvdHits = 0;
    flistMvdHitMatches = 0;
  }
  else 
  {
    flistMvdPts = dynamic_cast<TClonesArray*>(  fManger->GetObject("MvdPoint") );
    flistMvdHits = dynamic_cast<TClonesArray*>(  fManger->GetObject("MvdHit") );
    flistMvdHitMatches = dynamic_cast<TClonesArray*>(  fManger->GetObject("MvdHitMatch") );
  }

  return kSUCCESS;
}

void CbmKFParticlesFinderQA::Exec(Option_t * option)
{
  vStsHitMatch.clear();
  vStsPointMatch.clear();
  vMvdPointMatch.clear();
  vMCTrackMatch.clear();
  vStsHitMatch.resize(flistStsHits->GetEntriesFast());
  vStsPointMatch.resize(flistStsPts->GetEntriesFast());
  if(flistMvdPts)
    vMvdPointMatch.resize(flistMvdPts->GetEntriesFast());
  vMCTrackMatch.resize(flistMCTracks->GetEntriesFast());
  for(unsigned int iP=0; iP<vStsPointMatch.size(); iP++)
    vStsPointMatch[iP] = -1;
  for(unsigned int iP=0; iP<vMvdPointMatch.size(); iP++)
    vMvdPointMatch[iP] = -1;
  for(unsigned int iTr=0; iTr<vMCTrackMatch.size(); iTr++)
    vMCTrackMatch[iTr] = -1;
    
  if(flistMvdHits)
  {
    for(int iH=0; iH<flistMvdHits->GetEntriesFast(); iH++)
    {
      CbmMvdHitMatch *match = static_cast<CbmMvdHitMatch*>( flistMvdHitMatches->At(iH) );
      if( match){
        int iMC = match->GetPointId();
        vMvdPointMatch[iMC] = iH;
      }
    }
  }
  
  StsHitMatch();

  for(int iTr=0; iTr<flistStsTracksMatch->GetEntriesFast(); iTr++)
  {
    CbmTrackMatch* StsTrackMatch = (CbmTrackMatch*)flistStsTracksMatch->At(iTr);
    if(StsTrackMatch -> GetNofMCTracks() == 0) continue;
    const int mcTrackId = StsTrackMatch->GetMCTrackId();
    vMCTrackMatch[mcTrackId] = iTr;
  }
  
  fMCTrackPoints.clear();
  fMCTrackPoints.resize(flistMCTracks->GetEntriesFast());
  if(CbmKF::Instance()->vMvdMaterial.size() != 0)
    for (Int_t iMvd=0; iMvd<flistMvdPts->GetEntriesFast(); iMvd++)
    {
      CbmMvdPoint* MvdPoint = (CbmMvdPoint*)flistMvdPts->At(iMvd);
      fMCTrackPoints[MvdPoint->GetTrackID()].MvdArray.push_back(MvdPoint);
      int iHit = vMvdPointMatch[iMvd];
      if(iHit<0) continue;
      CbmMvdHit* MvdHit = (CbmMvdHit*)flistStsHits->At(iHit);
      fMCTrackPoints[MvdPoint->GetTrackID()].MvdHitsArray.push_back(MvdHit);
    }
  for (Int_t iSts=0; iSts<flistStsPts->GetEntriesFast(); iSts++)
  {
    CbmStsPoint* StsPoint = (CbmStsPoint*)flistStsPts->At(iSts);
    fMCTrackPoints[StsPoint->GetTrackID()].StsArray.push_back(StsPoint);
    int iHit = vStsPointMatch[iSts];
    if(iHit<0) continue;
    CbmStsHit* StsHit = (CbmStsHit*)flistStsHits->At(iHit);
    fMCTrackPoints[StsPoint->GetTrackID()].StsHitsArray.push_back(StsHit);
  }
  
  GetMCParticles();
  FindReconstructableMCParticles();
  MatchParticles();
  PartEffPerformance();
  PartHistoPerformance();
}

void CbmKFParticlesFinderQA::Finish()
{
  TDirectory *curr = gDirectory;
  TFile *currentFile = gFile;
  // Open output file and write histograms
  TFile* outfile = new TFile(outfileName.Data(),"RECREATE");
  outfile->cd();
  WriteHistos(histodir);
  outfile->Close();
  outfile->Delete();
  gFile = currentFile;
  gDirectory = curr;
}

void CbmKFParticlesFinderQA::WriteHistos( TObject *obj ){
  if( !obj->IsFolder() ) obj->Write();
  else{
    TDirectory *cur = gDirectory;
    TDirectory *sub = cur->mkdir(obj->GetName());
    sub->cd();
    TList *listSub = (static_cast<TDirectory*>(obj))->GetList();
    TIter it(listSub);
    while( TObject *obj1=it() ) WriteHistos(obj1);
    cur->cd();
  }
}

void CbmKFParticlesFinderQA::StsHitMatch()
{
  const bool useLinks = 1; // 0 - use HitMatch, one_to_one; 1 - use FairLinks, many_to_many. Set 0 to switch to old definition of efficiency.
  // TODO: fix trunk problem with links. Set useLinks = 1
  
  for (int iH = 0; iH < flistStsHits->GetEntriesFast(); iH++){
    CbmStsHit *stsHit = dynamic_cast<CbmStsHit*>(flistStsHits->At(iH));
    vStsHitMatch[iH] = -1;
    if (useLinks){
      if (flistStsClusters){
  //      if ( NLinks != 2 ) cout << "HitMatch: Error. Hit wasn't matched with 2 clusters." << endl;
          // 1-st cluster
        vector<int> stsPointIds; // save here all mc-points matched with first cluster
        vector<int> stsPointIds_hit;
        int iL = 0;
        FairLink link = stsHit->GetLink(iL);
        CbmStsCluster *stsCluster = dynamic_cast<CbmStsCluster*>( flistStsClusters->At( link.GetIndex() ) );
        int NLinks2 = stsCluster->GetNLinks();
        for ( int iL2 = 0; iL2 < NLinks2; iL2++){
          FairLink link2 = stsCluster->GetLink(iL2);
          CbmStsDigi *stsDigi = dynamic_cast<CbmStsDigi*>( flistStsDigi->At( link2.GetIndex() ) );
          const int NLinks3 = stsDigi->GetNLinks();
          for ( int iL3 = 0; iL3 < NLinks3; iL3++){
            FairLink link3 = stsDigi->GetLink(iL3);
            int stsPointId = link3.GetIndex();
            stsPointIds.push_back( stsPointId );
          } // for mcPoint
        } // for digi
          // 2-nd cluster
        iL = 1;
        link = stsHit->GetLink(iL);
        stsCluster = dynamic_cast<CbmStsCluster*>( flistStsClusters->At( link.GetIndex() ) );
        NLinks2 = stsCluster->GetNLinks();
        for ( int iL2 = 0; iL2 < NLinks2; iL2++){
          FairLink link2 = stsCluster->GetLink(iL2);
          CbmStsDigi *stsDigi = dynamic_cast<CbmStsDigi*>( flistStsDigi->At( link2.GetIndex() ) );
          const int NLinks3 = stsDigi->GetNLinks();
          for ( int iL3 = 0; iL3 < NLinks3; iL3++){
            FairLink link3 = stsDigi->GetLink(iL3);
            int stsPointId = link3.GetIndex();
            
            if ( !find(&(stsPointIds[0]), &(stsPointIds[stsPointIds.size()]), stsPointId) ) continue; // check if first cluster matched with same mc-point
//            CbmStsPoint *pt = dynamic_cast<CbmStsPoint*>( listStsPts->At(stsPointId) );
//            if(!pt) continue;
//          CbmMCTrack *MCTrack = dynamic_cast<CbmMCTrack*>( listMCTracks->At( pt->GetTrackID() ) );
//            if ( !MCTrack ) continue;
//            if ( abs(MCTrack->GetPdgCode()) >= 10000 ) continue;
            bool save = 1;
            for(unsigned int iP=0; iP < stsPointIds_hit.size(); iP ++) if(vStsHitMatch[iH] == stsPointIds_hit[iP]) save=0;
            stsPointIds_hit.push_back(stsPointId);
            vStsHitMatch[iH] = stsPointId;
            vStsPointMatch[stsPointId] = iH;
          } // for mcPoint
        } // for digi
      } // if clusters
    } // if useLinks
  } // for hits
}

void CbmKFParticlesFinderQA::GetMCParticles()
{
  vMCParticles.clear();
    // convert
  for(int i=0; i < flistMCTracks->GetEntriesFast(); i++)
  {
    CbmMCTrack &mtra = *(static_cast<CbmMCTrack*>(flistMCTracks->At(i)));
    CbmL1PFMCParticle part;
    part.SetMCTrackID( i );
    part.SetMotherId ( mtra.GetMotherId() );
    part.SetPDG      ( mtra.GetPdgCode() );
    vMCParticles.push_back( part );
  }
    // find relations
  const unsigned int nParticles = vMCParticles.size();
  for ( unsigned int iP = 0; iP < nParticles; iP++ ) {
    CbmL1PFMCParticle &part = vMCParticles[iP];
    for(unsigned int iP2 = 0; iP2 < nParticles; iP2++) {
      CbmL1PFMCParticle &part2 = vMCParticles[iP2];

      if(part.GetMotherId() == part2.GetMCTrackID()) {
        part2.AddDaughter(iP);
      }
    }
  }
}

void CbmKFParticlesFinderQA::FindReconstructableMCParticles()
{
  const unsigned int nParticles = vMCParticles.size();

  for ( unsigned int iP = 0; iP < nParticles; iP++ ) {
    CbmL1PFMCParticle &part = vMCParticles[iP];
    CheckMCParticleIsReconstructable(part);
  }
}

void CbmKFParticlesFinderQA::CheckMCParticleIsReconstructable(CbmL1PFMCParticle &part)
{
  if ( part.IsReconstructable() ) return;
  const int nParticles = 5;
  int partPDG[nParticles] = {310,3122,3312,-3312,3334};
  vector<int> partDaughterPdg[nParticles];

  partDaughterPdg[0].push_back( 211);
  partDaughterPdg[0].push_back(-211);

  partDaughterPdg[1].push_back(2212);
  partDaughterPdg[1].push_back(-211);

  partDaughterPdg[2].push_back(3122);
  partDaughterPdg[2].push_back(-211);

  partDaughterPdg[3].push_back(3122);
  partDaughterPdg[3].push_back( 211);

  partDaughterPdg[4].push_back(3122);
  partDaughterPdg[4].push_back(-321);


    // tracks
  if ( /*part.NDaughters() == 0*/ part.GetPDG() == -211 ||
                                  part.GetPDG() ==  211 ||
                                  part.GetPDG() == 2212 ||
                                  part.GetPDG() ==  321 ||
                                  part.GetPDG() == -321 ) { // TODO other particles

    switch ( fFindParticlesMode ) {
      case 1:
        part.SetAsReconstructable();
        break;
      case 2:
        {
          int iMCPart = part.GetMCTrackID();
          CbmMCTrack* mcTr = (static_cast<CbmMCTrack*>(flistMCTracks->At(iMCPart)));
          if(fMCTrackPoints[iMCPart].IsReconstructable(mcTr, fMinNStations, fPerformance, fMinRecoMom))
            part.SetAsReconstructable();
        }
        break;
      case 3:
        {
          int iMCPart = part.GetMCTrackID();
          if ( vMCTrackMatch[iMCPart]>=0 )
            part.SetAsReconstructable();
        }
        break;
      default:
        part.SetAsReconstructable();
    };
  }
    //  mother particles
  else
  {
    for(int iPart=0; iPart<nParticles; iPart++)
    {
      if(part.GetPDG() == partPDG[iPart])
      {
        const unsigned int nDaughters = partDaughterPdg[iPart].size();
        if( part.GetDaughterIds().size() != nDaughters ) return;
        int pdg[nDaughters];

        for(unsigned int iD=0; iD<nDaughters; iD++)
          pdg[iD] = vMCParticles[part.GetDaughterIds()[iD]].GetPDG();

        bool isDaughterFound[nDaughters];
        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          isDaughterFound[iDMC] = 0;

        bool isReco = 1;
        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          for(unsigned int iD=0; iD<nDaughters; iD++)
            if(pdg[iD] == partDaughterPdg[iPart][iDMC]) isDaughterFound[iDMC] = 1;

        for(unsigned int iDMC=0; iDMC<nDaughters; iDMC++)
          isReco = isReco && isDaughterFound[iDMC];

        if(!isReco) return;
      }
    }


    const vector<int>& dIds = part.GetDaughterIds();
    const unsigned int nD = dIds.size();
    bool reco = 1;
    for ( unsigned int iD = 0; iD < nD && reco; iD++ ) {
      CbmL1PFMCParticle &dp = vMCParticles[dIds[iD]];
      CheckMCParticleIsReconstructable(dp);
      reco &= dp.IsReconstructable();
    }
    if (reco) part.SetAsReconstructable();
  }
}

  /// Procedure for match Reconstructed and MC Particles. Should be called before Performances
void CbmKFParticlesFinderQA::MatchParticles()
{
    // get all reco particles ( temp )
  MCtoRParticleId.clear();
  RtoMCParticleId.clear();
  MCtoRParticleId.resize(vMCParticles.size());
  RtoMCParticleId.resize(fPF->GetParticles().size() );
  
    // match tracks ( particles which are direct copy of tracks )
  for( unsigned int iRP = 0; iRP < fPF->GetParticles().size(); iRP++ ) {
    CbmKFParticle &rPart = fPF->GetParticles()[iRP];

    if (rPart.NDaughters() != 1) continue;
    
    const int rTrackId = rPart.DaughterIds()[0];
    CbmTrackMatch* StsTrackMatch = (CbmTrackMatch*)flistStsTracksMatch->At(rTrackId);
    if(StsTrackMatch -> GetNofMCTracks() == 0) continue;
    const int mcTrackId = StsTrackMatch->GetMCTrackId();

    for ( unsigned int iMP = 0; iMP < vMCParticles.size(); iMP++ ) {
      CbmL1PFMCParticle &mPart = vMCParticles[iMP];
      if ( mPart.GetMCTrackID() == mcTrackId ) { // match is found
        if( mPart.GetPDG() == rPart.GetPDG() ) {
          MCtoRParticleId[iMP].ids.push_back(iRP);
          RtoMCParticleId[iRP].ids.push_back(iMP);
        }
        else {
          MCtoRParticleId[iMP].idsMI.push_back(iRP);
          RtoMCParticleId[iRP].idsMI.push_back(iMP);
        }
      }
    }
  }

    // match created mother particles
  for( unsigned int iRP = 0; iRP < fPF->GetParticles().size(); iRP++ ) {
    CbmKFParticle &rPart = fPF->GetParticles()[iRP];
    const unsigned int NRDaughters = rPart.NDaughters();
    if (NRDaughters < 2) continue;
    
    unsigned int iD = 0;
    int mmId = -2; // mother MC id
    {
      const int rdId = rPart.DaughterIds()[iD];
      if ( !RtoMCParticleId[rdId].IsMatched() ) continue;
      const int mdId = RtoMCParticleId[rdId].GetBestMatch();
      mmId = vMCParticles[mdId].GetMotherId();
    }
    iD++;
    for ( ; iD < NRDaughters; iD++ ) {
      const int rdId = rPart.DaughterIds()[iD];
      if ( !RtoMCParticleId[rdId].IsMatched() ) break;
      const int mdId = RtoMCParticleId[rdId].GetBestMatch();
      if( vMCParticles[mdId].GetMotherId() != mmId ) break;
    }
    if ( iD == NRDaughters && mmId != -1 ) { // match is found and it is not primary vertex
      CbmL1PFMCParticle &mmPart = vMCParticles[mmId];
      if( mmPart.GetPDG()     == rPart.GetPDG()     &&
          mmPart.NDaughters() == rPart.NDaughters() ) {
        MCtoRParticleId[mmId].ids.push_back(iRP);
        RtoMCParticleId[iRP].ids.push_back(mmId);
      }
      else {
        MCtoRParticleId[mmId].idsMI.push_back(iRP);
        RtoMCParticleId[iRP].idsMI.push_back(mmId);
      }
    }
  }
}

void CbmKFParticlesFinderQA::PartEffPerformance()
{
  static CbmL1PartEfficiencies PARTEFF; // average efficiencies

  static int NEVENTS               = 0;

  CbmL1PartEfficiencies partEff; // efficiencies for current event

  const int nParticles = 6;
  int partPDG[nParticles] = {310,3122,3312,-3312,3334,3000};
  TString partName[nParticles] = {"ks","lambda","ksi-","ksi+","omega-","Hdb"};

  const int NRP = fPF->GetParticles().size();
  for ( int iP = 0; iP < NRP; ++iP ) {
    const CbmKFParticle &part = fPF->GetParticles()[iP];
    const int pdg = part.GetPDG();
      
    const bool isBG = RtoMCParticleId[iP].idsMI.size() != 0;
    const bool isGhost = !RtoMCParticleId[iP].IsMatched();

    for(int iPart=0; iPart<nParticles; iPart++)
      if ( pdg == partPDG[iPart] )
        partEff.IncReco(isGhost, isBG, partName[iPart].Data());
  }

    
  const int NMP = vMCParticles.size();
  for ( int iP = 0; iP < NMP; ++iP ) {
    const CbmL1PFMCParticle &part = vMCParticles[iP];
    if ( !part.IsReconstructable() ) continue;
    const int pdg = part.GetPDG();
    const int mId = part.GetMotherId();
      
    const bool isReco = MCtoRParticleId[iP].ids.size() != 0;

    for(int iPart=0; iPart<nParticles; iPart++)
    {
      if ( pdg == partPDG[iPart] ) {
        partEff.Inc(isReco, partName[iPart].Data());
        if ( mId == -1 )
          partEff.Inc(isReco, (partName[iPart]+"_prim").Data());
        else
          partEff.Inc(isReco, (partName[iPart]+"_sec").Data());
      }
    }
  }

  NEVENTS++;

  PARTEFF += partEff;

  partEff.CalcEff();
  PARTEFF.CalcEff();

    //   cout.precision(3);
  if( fVerbose ){
    cout << " ---- KF Particle finder --- " << endl;
      // cout << "L1 STAT    : " << NEVENTS << " EVENT "               << endl << endl;
      // partEff.PrintEff();
      // cout << endl;
    cout << "ACCUMULATED STAT    : " << NEVENTS << " EVENTS "               << endl << endl;
    PARTEFF.PrintEff();

    cout<<endl;
      // cout<<"CA Track Finder: " << L1_CATIME/L1_NEVENTS << " s/ev" << endl << endl;
  }
}

void CbmKFParticlesFinderQA::PartHistoPerformance()
{
  static const int NParticles = 6; //Ks, Lambda

  static const int nFitQA = 16;
  static TH1F *hFitDaughtersQA[NParticles][nFitQA];
  static TH1F *hFitQA[NParticles][nFitQA];

  static const int nHistoPartParam = 11;
  static TH1F *hPartParam[NParticles][nHistoPartParam]; // mass, p, pt, Y, decay length, c*tau, chi/ndf, prob, theta, phi, z
  static TH1F *hPartParamBG[NParticles][nHistoPartParam];
  static TH1F *hPartParamSignal[NParticles][nHistoPartParam];
  static const int nHistoPartParamQA = 3;
  static TH1F *hPartParamQA[NParticles][nHistoPartParamQA*2]; // residuals and pulls of these parameters

  static const int nHistoPartParam2D = 1;
  static TH2F *hPartParam2D[NParticles][nHistoPartParam2D]; // y-pt,
  static TH2F *hPartParam2DBG[NParticles][nHistoPartParam2D];
  static TH2F *hPartParam2DSignal[NParticles][nHistoPartParam2D];
  
  static const int nHistosPV = 6;
  static TH1F *hPVFitQa[nHistosPV];

  TString patName[NParticles] = {"K0s","Lambda","Ksi-","Ksi+","Omega-","Hdb"};

  static bool first_call = 1;

  if ( first_call )
  {
    first_call = 0;

    TDirectory *currentDir = gDirectory;
    gDirectory->mkdir("KFParticlesFinder");
    gDirectory->cd("KFParticlesFinder");
    histodir = gDirectory;
    gDirectory->mkdir("Particles");
    gDirectory->cd("Particles");
    {
      for(int iPart=0; iPart<NParticles; ++iPart)
      {
        gDirectory->mkdir(patName[iPart].Data());
        gDirectory->cd(patName[iPart].Data());
        {
          TString res = "res";
          TString pull = "pull";

          gDirectory->mkdir("DaughtersQA");
          gDirectory->cd("DaughtersQA");
          {
            TString parName[nFitQA/2] = {"X","Y","Z","Px","Py","Pz","E","M"};
            int nBins = 100;
            float xMax[nFitQA/2] = {0.15,0.15,0.03,0.01,0.01,0.06,0.06,0.01};

            for( int iH=0; iH<nFitQA/2; iH++ ){
              hFitDaughtersQA[iPart][iH]   = new TH1F((res+parName[iH]).Data(),
                                                      (res+parName[iH]).Data(), 
                                                      nBins, -xMax[iH],xMax[iH]);
              hFitDaughtersQA[iPart][iH+8] = new TH1F((pull+parName[iH]).Data(),
                                                      (pull+parName[iH]).Data(), 
                                                      nBins, -6,6);
            }
          }
          gDirectory->cd(".."); //particle directory

          gDirectory->mkdir("FitQA");
          gDirectory->cd("FitQA");
          {
            TString parName[nFitQA/2] = {"X","Y","Z","Px","Py","Pz","E","M"};
            int nBins = 50;
            float xMax[nFitQA/2] = {0.15,0.15,1.2,0.02,0.02,0.15,0.15,0.006};

            for( int iH=0; iH<nFitQA/2; iH++ ){
              hFitQA[iPart][iH]   = new TH1F((res+parName[iH]).Data(),
                                             (res+parName[iH]).Data(), 
                                             nBins, -xMax[iH],xMax[iH]);
              hFitQA[iPart][iH+8] = new TH1F((pull+parName[iH]).Data(),
                                             (pull+parName[iH]).Data(), 
                                             nBins, -6,6);
            }
          }
          gDirectory->cd(".."); //particle directory

          gDirectory->mkdir("Parameters");
          gDirectory->cd("Parameters");
          {
            TString parName[nHistoPartParam] = {"M","p","p_{t}","y","DL","c#tau","chi2ndf","prob","#theta","phi","z"};
            TString parAxisName[nHistoPartParam] = {"m [GeV/c^{2}]","p [GeV/c]","p_{t} [GeV/c]",
                                                    "y","Decay length [cm]","Life time c#tau [cm]",
                                                    "chi2/ndf","prob","#theta [rad]",
                                                    "phi [rad]","z [cm]"};
            int nBins[nHistoPartParam] = {1000,100,100,100,100,100,100,100,100,100,100};
            float xMin[nHistoPartParam] = {0.3f,  0., 0., 0.,  0.,  0.,  0., 0., -2., -2., -5.};
            float xMax[nHistoPartParam] = {1.3f, 10., 3., 6., 30., 30., 20., 1.,  2.,  2., 55.};
            if(iPart == 1)
            {
              xMin[0] = 1.0; 
              xMax[0] = 2.0;
            }
            if(iPart == 2)
            {
              xMin[0] = 1.0; 
              xMax[0] = 3.0;
            }
            if(iPart == 4)
            {
              xMin[0] = 1.0; 
              xMax[0] = 3.0;
            }
            if(iPart == 5)
            {
              xMin[0] = 1.5; 
              xMax[0] = 3.5;
            }

            for(int iH=0; iH<nHistoPartParam; iH++)
            {
              hPartParam[iPart][iH]       = new TH1F(parName[iH].Data(),parName[iH].Data(),
                                              nBins[iH],xMin[iH],xMax[iH]);
              hPartParam[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
            }

            hPartParam2D[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                              nBins[3],xMin[3],xMax[3],
                                              nBins[2],xMin[2],xMax[2]);
            hPartParam2D[iPart][0]->GetXaxis()->SetTitle("y");
            hPartParam2D[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");

            gDirectory->mkdir("Signal");
            gDirectory->cd("Signal");
            {
              for(int iH=0; iH<nHistoPartParam; iH++)
              {
                hPartParamSignal[iPart][iH] = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
                                              nBins[iH],xMin[iH],xMax[iH]);
                hPartParamSignal[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
              }

              hPartParam2DSignal[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                                      nBins[3],xMin[3],xMax[3],
                                                      nBins[2],xMin[2],xMax[2]);
              hPartParam2DSignal[iPart][0]->GetXaxis()->SetTitle("y");
              hPartParam2DSignal[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");

              gDirectory->mkdir("QA");
              gDirectory->cd("QA");
              {
                int nBinsQA = 50;
                float xMaxQA[nHistoPartParamQA] = {0.01,0.001,0.001};
                for( int iH=0; iH<nHistoPartParamQA; iH++ ){
                  hPartParamQA[iPart][iH] = 
                    new TH1F((res+parName[iH]).Data(), (res+parName[iH]).Data(), nBinsQA, -xMaxQA[iH],xMaxQA[iH]);
                  hPartParamQA[iPart][iH+nHistoPartParamQA] = 
                    new TH1F((pull+parName[iH]).Data(), (pull+parName[iH]).Data(), nBinsQA, -6,6);
                }
              }
              gDirectory->cd(".."); // particle directory / Parameters / Signal

            }
            gDirectory->cd(".."); // particle directory / Parameters
            gDirectory->mkdir("Background");
            gDirectory->cd("Background");
            {
              for(int iH=0; iH<nHistoPartParam; iH++)
              {
                hPartParamBG[iPart][iH]     = new TH1F((parName[iH]).Data(),(parName[iH]).Data(),
                                              nBins[iH],xMin[iH],xMax[iH]);
                hPartParamBG[iPart][iH]->GetXaxis()->SetTitle(parAxisName[iH].Data());
              }

              hPartParam2DBG[iPart][0] = new TH2F("y-p_{t}","y-p_{t}",
                                                  nBins[3],xMin[3],xMax[3],
                                                  nBins[2],xMin[2],xMax[2]);
              hPartParam2DBG[iPart][0]->GetXaxis()->SetTitle("y");
              hPartParam2DBG[iPart][0]->GetYaxis()->SetTitle("p_{t} [GeV/c]");
            }
            gDirectory->cd(".."); // particle directory
          }
          gDirectory->cd(".."); //particle directory
        }
        gDirectory->cd(".."); //L1
      }
    }
    gDirectory->cd(".."); //L1
    gDirectory->mkdir("PrimaryVertexQA");
    gDirectory->cd("PrimaryVertexQA");
    {
      struct {
        string name;
        string title;
        Int_t n;
        Double_t l,r;
      } Table[nHistosPV]=
      {
        {"PVResX", "x_{rec}-x_{mc}, cm",     100, -0.006f, 0.006f},
        {"PVResY", "y_{rec}-y_{mc}, cm",     100, -0.006f, 0.006f},
        {"PVResZ", "z_{rec}-z_{mc}, cm",     100, -0.06f, 0.06f},
        {"PVPullX", "Pull X",     100, -6.f, 6.f},
        {"PVPullY", "Pull Y",     100, -6.f, 6.f},
        {"PVPullZ", "Pull Z",     100, -6.f, 6.f}
      };
      for(int iHPV=0; iHPV<nHistosPV; ++iHPV){
        hPVFitQa[iHPV] = new TH1F(Table[iHPV].name.data(),Table[iHPV].title.data(),
                                  Table[iHPV].n, Table[iHPV].l, Table[iHPV].r);
      }
    }
    gDirectory = currentDir;
  }

  for(unsigned int iP=0; iP < fPF->GetParticles().size(); iP++)
  {
    int iParticle = -1;
    switch( fPF->GetParticles()[iP].GetPDG() ) {
      case 310:
      {
        iParticle = 0;
      }
      break;
      case 3122:
      {
        iParticle = 1;
      }
      break;
      case 3312:
      {
        iParticle = 2;
      }
      break;
      case -3312:
      {
        iParticle = 3;
      }
      break;
      case 3334:
      {
        iParticle = 4;
      }
      break;
      case 3000:
      {
        iParticle = 5;
      }
      break;
    }
    if(iParticle < 0) continue;

    Double_t M, ErrM;
    Double_t dL, ErrdL; // decay length
    Double_t cT, ErrcT; // c*tau
    Double_t P, ErrP;
    Double_t Pt;
    Double_t Rapidity;
    Double_t Theta;
    Double_t Phi;
    Double_t Z;
    CbmKFParticle TempPart = fPF->GetParticles()[iP];
    TempPart.GetMass(M,ErrM);
    TempPart.GetMomentum(P,ErrP);
    Pt = TempPart.GetPt();
    Rapidity = TempPart.GetRapidity();
    TempPart.GetDecayLength(dL,ErrdL);
    TempPart.GetLifeTime(cT,ErrcT);
    Double_t chi2 = TempPart.GetChi2();
    Int_t ndf = TempPart.GetNDF();
    Double_t prob = TMath::Prob(chi2,ndf);
    Theta = TempPart.GetTheta();
    Phi = TempPart.GetPhi();
    Z = TempPart.GetZ();

    hPartParam[iParticle][0]->Fill(M);
    hPartParam[iParticle][1]->Fill(P);
    hPartParam[iParticle][2]->Fill(Pt);
    hPartParam[iParticle][3]->Fill(Rapidity);
    hPartParam[iParticle][4]->Fill(dL);
    hPartParam[iParticle][5]->Fill(cT);
    hPartParam[iParticle][6]->Fill(chi2/ndf);
    hPartParam[iParticle][7]->Fill(prob);
    hPartParam[iParticle][8]->Fill(Theta);
    hPartParam[iParticle][9]->Fill(Phi);
    hPartParam[iParticle][10]->Fill(Z);

    hPartParam2D[iParticle][0]->Fill(Rapidity,Pt,1);

    if(!RtoMCParticleId[iP].IsMatchedWithPdg()) //background
    {
      hPartParamBG[iParticle][0]->Fill(M);
      hPartParamBG[iParticle][1]->Fill(P);
      hPartParamBG[iParticle][2]->Fill(Pt);
      hPartParamBG[iParticle][3]->Fill(Rapidity);
      hPartParamBG[iParticle][4]->Fill(dL);
      hPartParamBG[iParticle][5]->Fill(cT);
      hPartParamBG[iParticle][6]->Fill(chi2/ndf);
      hPartParamBG[iParticle][7]->Fill(prob);
      hPartParamBG[iParticle][8]->Fill(Theta);
      hPartParamBG[iParticle][9]->Fill(Phi);
      hPartParamBG[iParticle][10]->Fill(Z);
      
      hPartParam2DBG[iParticle][0]->Fill(Rapidity,Pt,1);
      continue;
    }
    hPartParamSignal[iParticle][0]->Fill(M);
    hPartParamSignal[iParticle][1]->Fill(P);
    hPartParamSignal[iParticle][2]->Fill(Pt);
    hPartParamSignal[iParticle][3]->Fill(Rapidity);
    hPartParamSignal[iParticle][4]->Fill(dL);
    hPartParamSignal[iParticle][5]->Fill(cT);
    hPartParamSignal[iParticle][6]->Fill(chi2/ndf);
    hPartParamSignal[iParticle][7]->Fill(prob);
    hPartParamSignal[iParticle][8]->Fill(Theta);
    hPartParamSignal[iParticle][9]->Fill(Phi);
    hPartParamSignal[iParticle][10]->Fill(Z);

    hPartParam2DSignal[iParticle][0]->Fill(Rapidity,Pt,1);

    int iMCPart = RtoMCParticleId[iP].GetBestMatchWithPdg();
    CbmL1PFMCParticle &mcPart = vMCParticles[iMCPart];
    // Fit quality of the mother particle
    {
      int iMCTrack = mcPart.GetMCTrackID();
      CbmMCTrack &mcTrack = *(static_cast<CbmMCTrack*>(flistMCTracks->At(iMCTrack)));
      int mcDaughterId = mcPart.GetDaughterIds()[0];
      CbmMCTrack &mcDaughter = *(static_cast<CbmMCTrack*>(flistMCTracks->At(mcDaughterId)));

      Double_t decayVtx[3] = { mcDaughter.GetStartX(), mcDaughter.GetStartY(), mcDaughter.GetStartZ() };
      Double_t recParam[8] = { 0 };
      Double_t errParam[8] = { 0 };

      for(int iPar=0; iPar<3; iPar++)
      {
        recParam[iPar] = TempPart.GetParameter(iPar);
        Double_t error = TempPart.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        errParam[iPar] = TMath::Sqrt(error);
      }
      TempPart.Extrapolate(TempPart.r , TempPart.GetDStoPoint(decayVtx));
      for(int iPar=3; iPar<7; iPar++)
      {
        recParam[iPar] = TempPart.GetParameter(iPar);
        Double_t error = TempPart.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        errParam[iPar] = TMath::Sqrt(error);
      }

      Double_t Emc = sqrt(mcTrack.GetP()*mcTrack.GetP() + mcTrack.GetMass()*mcTrack.GetMass());
      Double_t res[8] = {0}, 
               pull[8] = {0}, 
               mcParam[8] = { decayVtx[0], decayVtx[1], decayVtx[2],
                              mcTrack.GetPx(), mcTrack.GetPy(), mcTrack.GetPz(), Emc, mcTrack.GetMass() };
      for(int iPar=0; iPar < 7; iPar++ )
      {
        res[iPar]  = recParam[iPar] - mcParam[iPar];
        if(fabs(errParam[iPar]) > 1.e-20) pull[iPar] = res[iPar]/errParam[iPar];
      }
      res[7] = M - mcParam[7];
      if(fabs(ErrM) > 1.e-20) pull[7] = res[7]/ErrM;

      for(int iPar=0; iPar < 8; iPar++ )
      {
        hFitQA[iParticle][iPar]->Fill(res[iPar]);
        hFitQA[iParticle][iPar+8]->Fill(pull[iPar]);
      }

//      Double_t mcT = mcDaughter.GetStartT() - mcTrack.GetStartT();
      
    }
    // Fit quality of daughters
    for(int iD=0; iD<mcPart.NDaughters(); ++iD)
    {
      int mcDaughterId = mcPart.GetDaughterIds()[iD];
      if(!MCtoRParticleId[mcDaughterId].IsMatchedWithPdg()) continue;
      CbmMCTrack &mcTrack = *(static_cast<CbmMCTrack*>(flistMCTracks->At(mcDaughterId)));
      int recDaughterId = MCtoRParticleId[mcDaughterId].GetBestMatchWithPdg();
      CbmKFParticle Daughter = fPF->GetParticles()[recDaughterId];
      Daughter.GetMass(M,ErrM);

      Double_t decayVtx[3] = {mcTrack.GetStartX(), mcTrack.GetStartY(), mcTrack.GetStartZ()};
      Daughter.Extrapolate(Daughter.r , Daughter.GetDStoPoint(decayVtx));

      Double_t Emc = sqrt(mcTrack.GetP()*mcTrack.GetP() + mcTrack.GetMass()*mcTrack.GetMass());
      Double_t res[8] = {0}, 
               pull[8] = {0}, 
               mcParam[8] = { mcTrack.GetStartX(), mcTrack.GetStartY(), mcTrack.GetStartZ(),
                            mcTrack.GetPx(), mcTrack.GetPy(), mcTrack.GetPz(), Emc, mcTrack.GetMass() };
      for(int iPar=0; iPar < 7; iPar++ )
      {
        Double_t error = Daughter.GetCovariance(iPar,iPar);
        if(error < 0.) { error = 1.e20;}
        error = TMath::Sqrt(error);
        res[iPar]  = Daughter.GetParameter(iPar) - mcParam[iPar];
        if(fabs(error) > 1.e-20) pull[iPar] = res[iPar]/error;
      }
      res[7] = M - mcParam[7];
      if(fabs(ErrM) > 1.e-20) pull[7] = res[7]/ErrM;

      for(int iPar=0; iPar < 8; iPar++ )
      {
        hFitDaughtersQA[iParticle][iPar]->Fill(res[iPar]);
        hFitDaughtersQA[iParticle][iPar+8]->Fill(pull[iPar]);
      }
    }
  }
  //Find MC parameters of the primary vertex 
  float mcPVx[3]={0.f};
  for(int iMC=0; iMC<flistMCTracks->GetEntriesFast(); ++iMC)
  {
    CbmMCTrack* mcTrack = (CbmMCTrack*)flistMCTracks->At(iMC);
    if(mcTrack->GetMotherId() < 0)
    {
      mcPVx[0]=mcTrack->GetStartX();
      mcPVx[1]=mcTrack->GetStartY();
      mcPVx[2]=mcTrack->GetStartZ();
      break;
    }
  }
  CbmKFVertex vtx(*fPrimVtx);
  float dRPVr[3] = {vtx.GetRefX()-mcPVx[0],
                    vtx.GetRefY()-mcPVx[1],
                    vtx.GetRefZ()-mcPVx[2]};
  float dRPVp[3] = {dRPVr[0]/sqrt(vtx.GetCovMatrix()[0]),
                    dRPVr[1]/sqrt(vtx.GetCovMatrix()[2]),
                    dRPVr[2]/sqrt(vtx.GetCovMatrix()[5])};
  for(unsigned int iHPV=0; iHPV<3; ++iHPV)
    hPVFitQa[iHPV]->Fill(dRPVr[iHPV]);
  for(unsigned int iHPV=3; iHPV<6; ++iHPV)
    hPVFitQa[iHPV]->Fill(dRPVp[iHPV-3]);

} // void CbmKFParticlesFinderQA::ParticlesEfficienciesPerformance()
