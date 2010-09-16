/*
 *====================================================================
 *
 *  CBM Level 1 Reconstruction 
 *  
 *  Authors: I.Kisel,  S.Gorbunov
 *
 *  e-mail : ikisel@kip.uni-heidelberg.de 
 *
 *====================================================================
 *
 *  L1 Fit performance 
 *
 *====================================================================
 */
#include "CbmL1.h"

#include "L1Algo/L1Algo.h"
#include "L1Algo/L1StsHit.h"
#include "L1Algo/L1Extrapolation.h" // for vertex pulls
#include "L1Algo/L1AddMaterial.h" // for vertex pulls
#include "FairTrackParam.h" // for vertex pulls
#include "CbmKFTrack.h" // for vertex pulls

#include "CbmKF.h"
#include "CbmKFMath.h"

#include "CbmStsDigi.h"
#include "CbmStsSensor.h" // for field FieldCheck.
#include "CbmStsSector.h" // for field FieldCheck.
#include "CbmStsStation.h" // for field FieldCheck.

#include "CbmL1Counters.h"

#include <iostream>
#include <vector>
#include <list>
#include <map>

using std::cout;
using std::endl;
using std::ios;
using std::vector;
using std::pair;
using std::map;

void CbmL1::TrackMatch(){
  map <int, CbmL1MCTrack*> pMCTrackMap; pMCTrackMap.clear();

    // fill pMCTrackMap
  for( vector<CbmL1MCTrack>::iterator i = vMCTracks.begin(); i != vMCTracks.end(); ++i){
    CbmL1MCTrack &MC = *i;
    
    if (pMCTrackMap.find(MC.ID) == pMCTrackMap.end()){
      pMCTrackMap.insert(pair<int, CbmL1MCTrack*>(MC.ID, &MC));
    }
    else{
      cout << "*** L1: Track ID " << MC.ID << " encountered twice! ***" << endl;
    }
  }
      // -- prepare information about reconstructed tracks
  const int nRTracks = vRTracks.size();
  for (int iR = 0; iR < nRTracks; iR++){
    CbmL1Track* prtra = &(vRTracks[iR]);
    
    int hitsum  = prtra->StsHits.size(); // number of hits in track

      // count how many hits from each mcTrack belong to current recoTrack
    map<int, int > &hitmap = prtra->hitMap; // how many hits from each mcTrack belong to current recoTrack
    for (vector<int>::iterator ih  = (prtra->StsHits).begin(); ih != (prtra->StsHits).end(); ++ih){
      
      const int nMCPoints = vStsHits[*ih].mcPointIds.size();
      for (int iP = 0; iP < nMCPoints; iP++){
        int iMC = vStsHits[*ih].mcPointIds[iP];
        int ID = -1;
        if (iMC >= 0) ID = vMCPoints[iMC].ID;
        if(hitmap.find(ID) == hitmap.end())
          hitmap[ID] = 1;
        else{
          hitmap[ID] += 1;
        }
      } // for iPoint
    } // for iHit

      // RTrack <-> MCTrack identification
    double max_percent = 0.0;  // [%]. maximum persent of hits, which belong to one mcTrack.
    for( map<int, int >::iterator posIt = hitmap.begin(); posIt != hitmap.end(); posIt++ ){ // loop over all touched MCTracks
      
      if (posIt->first < 0) continue; // not a MC track - based on fake hits
      
        // count max-purity
      if (100.0*double(posIt->second) > max_percent*double(hitsum))
        max_percent = 100.0*double(posIt->second)/double(hitsum);
      
        // set relation to the mcTrack
      if ( double(posIt->second) > MinPurity * double(hitsum) ){ // found correspondent MCTrack
        if (pMCTrackMap.find(posIt->first) == pMCTrackMap.end()) continue;
        CbmL1MCTrack* pmtra = pMCTrackMap[posIt->first];

        pmtra->AddRecoTrack(prtra);
        prtra->AddMCTrack(pmtra);
      } 
      else{
        if (pMCTrackMap.find(posIt->first) == pMCTrackMap.end()) continue;
        CbmL1MCTrack* pmtra = pMCTrackMap[posIt->first];

        pmtra->AddTouchTrack(prtra);
      }
    } // for hitmap
    prtra->SetMaxPurity(max_percent);
  } // for reco tracks
} // 



struct TL1PerfEfficiencies: public TL1Efficiencies
{
  TL1PerfEfficiencies():TL1Efficiencies(){
          // add total efficiency
    AddCounter("long_fast_prim" ,"LongRPrim efficiency");
    AddCounter("fast_prim"      ,"RefPrim   efficiency");
    AddCounter("fast_sec"       ,"RefSec    efficiency");
    AddCounter("fast"           ,"Refset    efficiency");
    AddCounter("total"          ,"Allset    efficiency");
    AddCounter("slow_prim"      ,"ExtraPrim efficiency");
    AddCounter("slow_sec"       ,"ExtraSec  efficiency");
    AddCounter("slow"           ,"Extra     efficiency");
    AddCounter("d0"             ,"D0        efficiency");
  }

  virtual void AddCounter(TString shortname, TString name){
    TL1Efficiencies::AddCounter(shortname, name);
    ratio_killed.AddCounter();
    ratio_clone.AddCounter();
    ratio_length.AddCounter();
    killed.AddCounter();
    clone.AddCounter();
    reco_length.AddCounter();
  };
  
  TL1PerfEfficiencies& operator+=(TL1PerfEfficiencies& a){
    TL1Efficiencies::operator+=(a);
    killed += a.killed; clone += a.clone;
    reco_length += a.reco_length;
    return *this;
  };
  
  void CalcEff(){
    TL1Efficiencies::CalcEff();
    ratio_killed = killed/mc;
    ratio_clone = clone/mc;
    ratio_length = reco_length/reco;
  };
  
  void Inc(bool isReco, bool isKilled, double _ratio_length, int _nclones, TString name){
    TL1Efficiencies::Inc(isReco, name);

    const int index = indices[name];

    if (isKilled) killed.counters[index]++;
    reco_length.counters[index] += _ratio_length;
    clone.counters[index] += _nclones;
  };

  void PrintEff(){
    cout.setf(ios::fixed);
    cout.setf(ios::showpoint);
    cout.precision(3);
    cout << "Track category         : " << " Eff "        <<" / "<< "Killed" <<" / "<< "Length" <<" / "<< "Clones" <<" | "<< "All MC"  << endl;
    
    int NCounters = mc.NCounters;
    for (int iC = 0; iC < NCounters; iC++){
      if (( names[iC] != "D0        efficiency") || (mc.counters[iC] != 0))
        cout << names[iC]  << "   : "
            << ratio_reco.counters[iC]
            << "  / " << ratio_killed.counters[iC]
            << "  / " << ratio_length.counters[iC]
            << "  / " << ratio_clone.counters[iC]
            << "  | " << mc.counters[iC]  << endl;
    }
    cout << "Ghost     probability  : " << ratio_ghosts <<" | "<< ghosts << endl;
  };

  TL1TracksCatCounters<double> ratio_killed;
  TL1TracksCatCounters<double> ratio_clone;
  TL1TracksCatCounters<double> ratio_length;

  TL1TracksCatCounters<int> killed;
  TL1TracksCatCounters<int> clone;
  TL1TracksCatCounters<double> reco_length;
};


void CbmL1::EfficienciesPerformance()
{
  static TL1PerfEfficiencies L1_NTRA; // average efficiencies

  static int L1_NEVENTS               = 0;
  static double L1_CATIME             = 0.0;

  TL1PerfEfficiencies ntra; // efficiencies for current event

  for (vector<CbmL1Track>::iterator rtraIt = vRTracks.begin(); rtraIt != vRTracks.end(); ++rtraIt){
    ntra.ghosts += rtraIt->IsGhost();
//     if(rtraIt->IsGhost()){ // Debug.
//       cout << " " << rtraIt->GetNOfHits() << " " << 1./rtraIt->T[5] << " " << rtraIt->GetMaxPurity() << " | ";
//       for( map<int, int>::iterator posIt = rtraIt->hitMap.begin(); posIt != rtraIt->hitMap.end(); posIt++ ){
//         cout << " (" << posIt->second << " " << posIt->first << ") ";
//       }
//       cout << endl;
//     }
  }

  int sta_nhits[algo->NStations], sta_nfakes[algo->NStations];

  for (int i = 0; i < algo->NStations; i++){
    sta_nhits[i]  = 0;
    sta_nfakes[i] = 0;
  }

  for ( vector<CbmL1MCTrack>::iterator mtraIt = vMCTracks.begin(); mtraIt != vMCTracks.end(); mtraIt++ ) {
    CbmL1MCTrack &mtra = *(mtraIt);
    // No Sts hits?
    
    if( ! mtra.IsReconstructable() ) continue;

      // -- find used constans --
      // is track reconstructed
    const bool reco = (mtra.IsReconstructed());
      // is track killed. At least one hit of it belong to some recoTrack
    const bool killed = !reco && mtra.IsDisturbed();
      // ration length for current mcTrack
    vector< CbmL1Track* >& rTracks = mtra.GetRecoTracks(); // for length calculations
    double ratio_length = 0;
    if (reco){
      double mc_length = mtra.NStations();
      for (unsigned int irt = 0; irt < rTracks.size(); irt++)
        ratio_length += double(rTracks[irt]->GetNOfHits())/mc_length;
    }
      // number of clones
    int nclones = 0;
    if (reco) nclones = mtra.GetNClones();
//     if (nclones){ // Debug. Look at clones
//       for (int irt = 0; irt < rTracks.size(); irt++){
//         const int ista = vHitStore[rTracks[irt]->StsHits[0]].iStation;
//         cout << rTracks[irt]->GetNOfHits() << "(" << ista << ") ";
//       }
//       cout << mtra.NStations() << endl;
//     }

    ntra.Inc(reco, killed, ratio_length, nclones, "total");
    
    if (( mtra.IsPrimary() )&&(mtra.z > 0)){ // D0
      ntra.Inc(reco, killed, ratio_length, nclones, "d0");
    }

    if ( mtra.p > MinRefMom ){                        // reference tracks
      ntra.Inc(reco, killed, ratio_length, nclones, "fast");
      
      if ( mtra.IsPrimary() ){                         // reference primary
        if ( mtra.NStations() == NStation ){ // long reference primary
          ntra.Inc(reco, killed, ratio_length, nclones, "long_fast_prim");
        }
        ntra.Inc(reco, killed, ratio_length, nclones, "fast_prim");
      }
      else{                                             // reference secondary
        ntra.Inc(reco, killed, ratio_length, nclones, "fast_sec");
      }
    }
    else{                                               // extra set of tracks
      ntra.Inc(reco, killed, ratio_length, nclones, "slow");
       
      if ( mtra.IsPrimary() ){             // extra primary
        ntra.Inc(reco, killed, ratio_length, nclones, "slow_prim");
      }
      else{
        ntra.Inc(reco, killed, ratio_length, nclones, "slow_sec");
      }
    } // if extra

  } // for mcTracks

  
  L1_CATIME += algo->CATime;
  L1_NEVENTS++;

  L1_NTRA += ntra;

  ntra.CalcEff();
  L1_NTRA.CalcEff();

  //   cout.precision(3);
  if( fVerbose ){
    if( fVerbose > 1 ){
      ntra.PrintEff();
      cout << "Number of true and fake hits in stations: " << endl;
      for (int i = 0; i < algo->NStations; i++){
        cout << sta_nhits[i]-sta_nfakes[i] << "+" << sta_nfakes[i] << "   ";
      }
      cout << endl;
    } // fVerbose > 1
    cout << endl;
    cout << "L1 ACCUMULATED STAT    : " << L1_NEVENTS << " EVENTS "               << endl << endl;
    L1_NTRA.PrintEff();
    cout << "MC tracks/event found  : " << int(double(L1_NTRA.reco.counters[L1_NTRA.indices["total"]])/double(L1_NEVENTS)) << endl;
    cout<<endl;
    cout<<"CA Track Finder: " << L1_CATIME/L1_NEVENTS << " s/ev" << endl << endl;
  }
} // void CbmL1::Performance()

void CbmL1::HistoPerformance() // TODO: check if works correctly. Change vHitRef on match data in CbmL1**Track classes
{

  //CbmKF &KF = *CbmKF::Instance();

  static TProfile *p_eff_all_vs_mom, *p_eff_prim_vs_mom, *p_eff_sec_vs_mom, *p_eff_d0_vs_mom;
  
  static TH1F *h_reg_mom_prim, *h_reg_mom_sec, *h_reg_nhits_prim, *h_reg_nhits_sec;
  static TH1F *h_acc_mom_prim, *h_acc_mom_sec, *h_acc_nhits_prim, *h_acc_nhits_sec;
  static TH1F *h_reco_mom_prim, *h_reco_mom_sec, *h_reco_nhits_prim, *h_reco_nhits_sec;
  static TH1F *h_rest_mom_prim, *h_rest_mom_sec, *h_rest_nhits_prim, *h_rest_nhits_sec;

  //static TH1F *h_hit_density[10];

  static TH1F
      *h_ghost_mom, *h_ghost_nhits, *h_ghost_fstation,
  *h_ghost_chi2, *h_ghost_prob, *h_ghost_tx, *h_ghost_ty;
  static TH1F *h_reco_mom,  *h_reco_d0_mom, *h_reco_nhits, *h_reco_station,
  *h_reco_chi2, *h_reco_prob, *h_reco_clean, *h_reco_time;
  static TProfile *h_reco_timeNtr, *h_reco_timeNhit ;
  static TProfile *h_reco_fakeNtr, *h_reco_fakeNhit ;
  static TH1F *h_tx, *h_ty, *h_sec_r, *h_ghost_r;

  static TH1F *h_notfound_mom, *h_notfound_nhits, *h_notfound_station, *h_notfound_r, *h_notfound_tx, *h_notfound_ty;
  
  static TH1F *h_mcp, *h_nmchits, *h_chi2, *h_prob, *MC_vx, *MC_vy, *MC_vz, *VtxChiPrim, *VtxChiSec;

  static TH2F *P_vs_P ;

  static TH2F *h2_vertex, *h2_prim_vertex, *h2_sec_vertex;
  //static TH3F *h3_vertex, *h3_prim_vertex, *h3_sec_vertex;

  static TH2F *h2_reg_nhits_vs_mom_prim, *h2_reg_nhits_vs_mom_sec,
  *h2_reg_fstation_vs_mom_prim, *h2_reg_fstation_vs_mom_sec, *h2_reg_lstation_vs_fstation_prim, *h2_reg_lstation_vs_fstation_sec;
  static TH2F *h2_acc_nhits_vs_mom_prim, *h2_acc_nhits_vs_mom_sec,
  *h2_acc_fstation_vs_mom_prim, *h2_acc_fstation_vs_mom_sec, *h2_acc_lstation_vs_fstation_prim, *h2_acc_lstation_vs_fstation_sec;
  static TH2F *h2_reco_nhits_vs_mom_prim, *h2_reco_nhits_vs_mom_sec,
  *h2_reco_fstation_vs_mom_prim, *h2_reco_fstation_vs_mom_sec, *h2_reco_lstation_vs_fstation_prim, *h2_reco_lstation_vs_fstation_sec;
  static TH2F *h2_ghost_nhits_vs_mom, *h2_ghost_fstation_vs_mom, *h2_ghost_lstation_vs_fstation;
  static TH2F *h2_rest_nhits_vs_mom_prim, *h2_rest_nhits_vs_mom_sec,
  *h2_rest_fstation_vs_mom_prim, *h2_rest_fstation_vs_mom_sec, *h2_rest_lstation_vs_fstation_prim, *h2_rest_lstation_vs_fstation_sec;

  static bool first_call = 1;

  if ( first_call )
  {
    first_call = 0;

      //TDirectory *maindir = gDirectory;
      //histodir->cd();

    TDirectory *curdir = gDirectory;
      //histodir = gROOT->mkdir("L1");
    histodir->cd();

    p_eff_all_vs_mom = new TProfile("p_eff_all_vs_mom", "AllSet Efficiency vs Momentum",
                                    100, 0.0, 5.0, 0.0, 100.0);
    p_eff_prim_vs_mom = new TProfile("p_eff_prim_vs_mom", "All Primary Set Efficiency vs Momentum",
                                     100, 0.0, 5.0, 0.0, 100.0);
    p_eff_sec_vs_mom = new TProfile("p_eff_sec_vs_mom", "All Secondary Set Efficiency vs Momentum",
                                    100, 0.0, 5.0, 0.0, 100.0);
    p_eff_d0_vs_mom = new TProfile("p_eff_d0_vs_mom", "D0 Secondary Tracks Efficiency vs Momentum",
                                   150, 0.0, 15.0, 0.0, 100.0);

    h_reg_mom_prim   = new TH1F("h_reg_mom_prim", "Momentum of registered primary tracks", 500, 0.0, 5.0);
    h_reg_mom_sec   = new TH1F("h_reg_mom_sec", "Momentum of registered secondary tracks", 500, 0.0, 5.0);
    h_acc_mom_prim   = new TH1F("h_acc_mom_prim", "Momentum of accepted primary tracks", 500, 0.0, 5.0);
    h_acc_mom_sec   = new TH1F("h_acc_mom_sec", "Momentum of accepted secondary tracks", 500, 0.0, 5.0);
    h_reco_mom_prim   = new TH1F("h_reco_mom_prim", "Momentum of reconstructed primary tracks", 500, 0.0, 5.0);
    h_reco_mom_sec   = new TH1F("h_reco_mom_sec", "Momentum of reconstructed secondary tracks", 500, 0.0, 5.0);
    h_rest_mom_prim   = new TH1F("h_rest_mom_prim", "Momentum of not found primary tracks", 500, 0.0, 5.0);
    h_rest_mom_sec   = new TH1F("h_rest_mom_sec", "Momentum of not found secondary tracks", 500, 0.0, 5.0);

    h_reg_nhits_prim = new TH1F("h_reg_nhits_prim", "Number of hits in registered primary track", 51, -0.1, 10.1);
    h_reg_nhits_sec = new TH1F("h_reg_nhits_sec", "Number of hits in registered secondary track", 51, -0.1, 10.1);
    h_acc_nhits_prim = new TH1F("h_acc_nhits_prim", "Number of hits in accepted primary track", 51, -0.1, 10.1);
    h_acc_nhits_sec = new TH1F("h_acc_nhits_sec", "Number of hits in accepted secondary track", 51, -0.1, 10.1);
    h_reco_nhits_prim = new TH1F("h_reco_nhits_prim", "Number of hits in reconstructed primary track", 51, -0.1, 10.1);
    h_reco_nhits_sec = new TH1F("h_reco_nhits_sec", "Number of hits in reconstructed secondary track", 51, -0.1, 10.1);
    h_rest_nhits_prim = new TH1F("h_rest_nhits_prim", "Number of hits in not found primary track", 51, -0.1, 10.1);
    h_rest_nhits_sec = new TH1F("h_rest_nhits_sec", "Number of hits in not found secondary track", 51, -0.1, 10.1);

    h_ghost_mom = new TH1F("h_ghost_mom", "Momentum of ghost track", 500, 0.0, 5.0);
    h_ghost_nhits = new TH1F("h_ghost_nhits", "Number of hits in ghost track", 51, -0.1, 10.1);
    h_ghost_fstation = new TH1F("h_ghost_fstation", "First station of ghost track", 50, -0.5, 10.0);
    h_ghost_chi2 = new TH1F("h_ghost_chi2", "Chi2/NDF of ghost track", 50, -0.5, 10.0);
    h_ghost_prob = new TH1F("h_ghost_prob", "Prob of ghost track", 50, 0., 1.01);
    h_ghost_r = new TH1F("h_ghost_r", "R of ghost track at the first hit", 50, 0.0, 15.0);
    h_ghost_tx = new TH1F("h_ghost_tx", "tx of ghost track at the first hit", 50, -5.0, 5.0);
    h_ghost_ty = new TH1F("h_ghost_ty", "ty of ghost track at the first hit", 50, -1.0, 1.0);

    h_reco_mom = new TH1F("h_reco_mom", "Momentum of reco track", 50, 0.0, 5.0);
    h_reco_nhits = new TH1F("h_reco_nhits", "Number of hits in reco track", 50, 0.0, 10.0);
    h_reco_station = new TH1F("h_reco_station", "First station of reco track", 50, -0.5, 10.0);
    h_reco_chi2 = new TH1F("h_reco_chi2", "Chi2/NDF of reco track", 50, -0.5, 10.0);
    h_reco_prob = new TH1F("h_reco_prob", "Prob of reco track", 50, 0., 1.01);
    h_reco_clean = new TH1F("h_reco_clean", "Percentage of correct hits", 100, -0.5, 100.5);
    h_reco_time = new TH1F("h_reco_time", "CA Track Finder Time (s/ev)", 20, 0.0, 20.0);
    h_reco_timeNtr = new TProfile("h_reco_timeNtr",
                                  "CA Track Finder Time (s/ev) vs N Tracks",
                                  200, 0.0, 1000.0);
    h_reco_timeNhit = new TProfile("h_reco_timeNhit",
                                   "CA Track Finder Time (s/ev) vs N Hits",
                                   200, 0.0, 30000.0);
    h_reco_fakeNtr = new TProfile("h_reco_fakeNtr",
                                  "N Fake hits vs N Tracks",
                                  200, 0.0, 1000.0);
    h_reco_fakeNhit = new TProfile("h_reco_fakeNhit",
                                   "N Fake hits vs N Hits",
                                   200, 0.0, 30000.0);

    h_reco_d0_mom = new TH1F("h_reco_d0_mom", "Momentum of reco D0 track", 150, 0.0, 15.0);
      
//     h_hit_density[0] = new TH1F("h_hit_density0", "Hit density station 1", 50, 0.0,  5.00);
//     h_hit_density[1] = new TH1F("h_hit_density1", "Hit density station 2", 100, 0.0, 10.00);
//     h_hit_density[2] = new TH1F("h_hit_density2", "Hit density station 3", 140, 0.0, 13.99);
//     h_hit_density[3] = new TH1F("h_hit_density3", "Hit density station 4", 180, 0.0, 18.65);
//     h_hit_density[4] = new TH1F("h_hit_density4", "Hit density station 5", 230, 0.0, 23.32);
//     h_hit_density[5] = new TH1F("h_hit_density5", "Hit density station 6", 270, 0.0, 27.98);
//     h_hit_density[6] = new TH1F("h_hit_density6", "Hit density station 7", 340, 0.0, 34.97);
//     h_hit_density[7] = new TH1F("h_hit_density7", "Hit density station 8", 460, 0.0, 46.63);
//     h_hit_density[8] = new TH1F("h_hit_density8", "Hit density station 9", 500, 0.0, 50.00);
//     h_hit_density[9] = new TH1F("h_hit_density8", "Hit density station 9", 500, 0.0, 50.00);
//     h_hit_density[10] = new TH1F("h_hit_density8", "Hit density station 9", 500, 0.0, 50.00);
      
    h_tx = new TH1F("h_tx", "tx of MC track at the vertex", 50, -0.5, 0.5);
    h_ty = new TH1F("h_ty", "ty of MC track at the vertex", 50, -0.5, 0.5);

    h_sec_r = new TH1F("h_sec_r", "R of sec MC track at the first hit", 50, 0.0, 15.0);

    h_notfound_mom = new TH1F("h_notfound_mom", "Momentum of not found track", 50, 0.0, 5.0);
    h_notfound_nhits = new TH1F("h_notfound_nhits", "Num hits of not found track", 50, 0.0, 10.0);
    h_notfound_station = new TH1F("h_notfound_station", "First station of not found track", 50, 0.0, 10.0);
    h_notfound_r = new TH1F("h_notfound_r", "R at first hit of not found track", 50, 0.0, 15.0);
    h_notfound_tx = new TH1F("h_notfound_tx", "tx of not found track at the first hit", 50, -5.0, 5.0);
    h_notfound_ty = new TH1F("h_notfound_ty", "ty of not found track at the first hit", 50, -1.0, 1.0);

    h_chi2 = new TH1F("chi2", "Chi^2", 100, 0.0, 10.);
    h_prob = new TH1F("prob", "Prob", 100, 0.0, 1.01);
    VtxChiPrim = new TH1F("vtxChiPrim", "Chi to primary vtx for primary tracks", 100, 0.0, 10.);
    VtxChiSec = new TH1F("vtxChiSec", "Chi to primary vtx for secondary tracks", 100, 0.0, 10.);
    h_mcp = new TH1F("h_mcp", "MC P ", 500, 0.0, 5.0);
    MC_vx = new TH1F("MC_vx", "MC Vertex X", 100, -.05, .05);
    MC_vy = new TH1F("MC_vy", "MC Vertex Y", 100, -.05, .05);
    MC_vz = new TH1F("MC_vz", "MC Vertex Z", 100, -.05, .05);
    h_nmchits = new TH1F("h_nmchits", "N STS hits on MC track", 50, 0.0, 10.0);
 
    P_vs_P = new TH2F("P_vs_P", "Resolution P/Q vs P", 20, 0., 20.,100, -.05, .05);
      
    h2_vertex = new TH2F("h2_vertex", "2D vertex distribution", 105, -5., 100., 100, -50., 50.);
    h2_prim_vertex = new TH2F("h2_primvertex", "2D primary vertex distribution", 105, -5., 100., 100, -50., 50.);
    h2_sec_vertex = new TH2F("h2_sec_vertex", "2D secondary vertex distribution", 105, -5., 100., 100, -50., 50.);

      //h3_vertex = new TH3F("h3_vertex", "3D vertex distribution", 20, -5., 100., 100, -50., 50., 100, -50., 50.);
      //h3_prim_vertex = new TH3F("h3_primvertex", "3D vertex distribution", 20, -5., 100., 100, -50., 50., 100, -50., 50.);
      //h3_sec_vertex = new TH3F("h3_sec_vertex", "3D vertex distribution", 20, -5., 100., 100, -50., 50., 100, -50., 50.);

    h2_reg_nhits_vs_mom_prim = new TH2F("h2_reg_nhits_vs_mom_prim", "NHits vs. Momentum (Reg. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reg_nhits_vs_mom_sec = new TH2F("h2_reg_nhits_vs_mom_sec", "NHits vs. Momentum (Reg. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_acc_nhits_vs_mom_prim = new TH2F("h2_acc_nhits_vs_mom_prim", "NHits vs. Momentum (Acc. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_acc_nhits_vs_mom_sec = new TH2F("h2_acc_nhits_vs_mom_sec", "NHits vs. Momentum (Acc. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reco_nhits_vs_mom_prim = new TH2F("h2_reco_nhits_vs_mom_prim", "NHits vs. Momentum (Reco Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reco_nhits_vs_mom_sec = new TH2F("h2_reco_nhits_vs_mom_sec", "NHits vs. Momentum (Reco Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_ghost_nhits_vs_mom = new TH2F("h2_ghost_nhits_vs_mom", "NHits vs. Momentum (Ghost Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_rest_nhits_vs_mom_prim = new TH2F("h2_rest_nhits_vs_mom_prim", "NHits vs. Momentum (!Found Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_rest_nhits_vs_mom_sec = new TH2F("h2_rest_nhits_vs_mom_sec", "NHits vs. Momentum (!Found Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);

    h2_reg_fstation_vs_mom_prim = new TH2F("h2_reg_fstation_vs_mom_prim", "First Station vs. Momentum (Reg. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reg_fstation_vs_mom_sec = new TH2F("h2_reg_fstation_vs_mom_sec", "First Station vs. Momentum (Reg. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_acc_fstation_vs_mom_prim = new TH2F("h2_acc_fstation_vs_mom_prim", "First Station vs. Momentum (Acc. Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_acc_fstation_vs_mom_sec = new TH2F("h2_acc_fstation_vs_mom_sec", "First Station vs. Momentum (Acc. Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reco_fstation_vs_mom_prim = new TH2F("h2_reco_fstation_vs_mom_prim", "First Station vs. Momentum (Reco Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_reco_fstation_vs_mom_sec = new TH2F("h2_reco_fstation_vs_mom_sec", "First Station vs. Momentum (Reco Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_ghost_fstation_vs_mom = new TH2F("h2_ghost_fstation_vs_mom", "First Station vs. Momentum (Ghost Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_rest_fstation_vs_mom_prim = new TH2F("h2_rest_fstation_vs_mom_prim", "First Station vs. Momentum (!Found Primary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);
    h2_rest_fstation_vs_mom_sec = new TH2F("h2_rest_fstation_vs_mom_sec", "First Station vs. Momentum (!Found Secondary Tracks)", 51, -0.05, 5.05, 11, -0.5, 10.5);

    h2_reg_lstation_vs_fstation_prim = new TH2F("h2_reg_lstation_vs_fstation_prim", "Last vs. First Station (Reg. Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_reg_lstation_vs_fstation_sec = new TH2F("h2_reg_lstation_vs_fstation_sec", "Last vs. First Station (Reg. Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_acc_lstation_vs_fstation_prim = new TH2F("h2_acc_lstation_vs_fstation_prim", "Last vs. First Station (Acc. Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_acc_lstation_vs_fstation_sec = new TH2F("h2_acc_lstation_vs_fstation_sec", "Last vs. First Station (Acc. Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_reco_lstation_vs_fstation_prim = new TH2F("h2_reco_lstation_vs_fstation_prim", "Last vs. First Station (Reco Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_reco_lstation_vs_fstation_sec = new TH2F("h2_reco_lstation_vs_fstation_sec", "Last vs. First Station (Reco Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_ghost_lstation_vs_fstation = new TH2F("h2_ghost_lstation_vs_fstation", "Last vs. First Station (Ghost Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_rest_lstation_vs_fstation_prim = new TH2F("h2_rest_lstation_vs_fstation_prim", "Last vs. First Station (!Found Primary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);
    h2_rest_lstation_vs_fstation_sec = new TH2F("h2_rest_lstation_vs_fstation_sec", "Last vs. First Station (!Found Secondary Tracks)", 11, -0.5, 10.5, 11, -0.5, 10.5);

      //maindir->cd(); 

      // ----- Create list of all histogram pointers
      
    curdir->cd();
      
  }// first_call

    
//   //hit density calculation: h_hit_density[10]
  //
//   for (vector<CbmL1HitStore>::iterator hIt = vHitStore.begin(); hIt != vHitStore.end(); ++hIt){
//     float x = hIt->x;
//     float y = hIt->y;
//     float r = sqrt(x*x+y*y);
//     h_hit_density[hIt->iStation]->Fill(r, 1.0/(2.0*3.1415*r));
//   }
  

  //
  for (vector<CbmL1Track>::iterator rtraIt = vRTracks.begin(); rtraIt != vRTracks.end(); ++rtraIt){
    CbmL1Track* prtra = &(*rtraIt);

    {  // fill histos
      if( fabs(prtra->T[4])>1.e-10 ) h_reco_mom->Fill(fabs(1.0/prtra->T[4]));
      h_reco_nhits->Fill((prtra->StsHits).size());
      CbmL1HitStore &mh = vHitStore[prtra->StsHits[0]];
      h_reco_station->Fill(mh.iStation);
      if (prtra->NDF > 0){
        h_reco_chi2->Fill(prtra->chi2/prtra->NDF);
        h_reco_prob->Fill(TMath::Erf(sqrt(fabs(prtra->chi2/prtra->NDF))));
      }
    }

    h_reco_clean->Fill( prtra->GetMaxPurity() );

      // fill ghost histos
    if ( prtra->IsGhost() ){
      if( fabs(prtra->T[4])>1.e-10)
        h_ghost_mom->Fill(fabs(1.0/prtra->T[4]));
      h_ghost_nhits->Fill((prtra->StsHits).size());
      CbmL1HitStore &h1 = vHitStore[prtra->StsHits[0]];
      CbmL1HitStore &h2 = vHitStore[prtra->StsHits[1]];
      h_ghost_fstation->Fill(h1.iStation);
      if (prtra->NDF > 0){
        h_ghost_chi2->Fill(prtra->chi2/prtra->NDF);
        h_ghost_prob->Fill(TMath::Erf(sqrt(fabs(prtra->chi2/prtra->NDF))));
      }
      h_ghost_r->Fill(sqrt(fabs(h1.x*h1.x+h1.y*h1.y)));
      double z1 = algo->vStations[h1.iStation].z[0];
      double z2 = algo->vStations[h2.iStation].z[0];
      if( fabs(z2-z1)>1.e-4 ){
        h_ghost_tx->Fill((h2.x-h1.x)/(z2-z1));
        h_ghost_ty->Fill((h2.y-h1.y)/(z2-z1));
      }

      if( fabs(prtra->T[4])>1.e-10)
        h2_ghost_nhits_vs_mom->Fill(fabs(1.0/prtra->T[4]), (prtra->StsHits).size());
      CbmL1HitStore &hf = vHitStore[prtra->StsHits[0]];
      CbmL1HitStore &hl = vHitStore[prtra->StsHits[(prtra->StsHits).size()-1]];
      if( fabs(prtra->T[4])>1.e-10)
        h2_ghost_fstation_vs_mom->Fill(fabs(1.0/prtra->T[4]), hf.iStation+1);
      if (hl.iStation >= hf.iStation)
        h2_ghost_lstation_vs_fstation->Fill(hf.iStation+1, hl.iStation+1);
    }

  } // for reco tracks

  int mc_total = 0; // total amount of reconstructable mcTracks
  for ( vector<CbmL1MCTrack>::iterator mtraIt = vMCTracks.begin(); mtraIt != vMCTracks.end(); mtraIt++ ) {
    CbmL1MCTrack &mtra = *(mtraIt);
    // No Sts hits? 
    int nmchits = mtra.StsHits.size();
    if (nmchits == 0) continue;

    double momentum = mtra.p;

    h_mcp->Fill(momentum);
    h_nmchits->Fill(nmchits);

    int nSta = mtra.NStations();

    CbmL1HitStore &fh = vHitStore[*(mtra.StsHits.begin())];
    CbmL1HitStore &lh = vHitStore[*(mtra.StsHits.rbegin())];

    if ( mtra.IsPrimary() ){
      h_reg_mom_prim->Fill(momentum);
      h_reg_nhits_prim->Fill(nSta);
      h2_reg_nhits_vs_mom_prim->Fill(momentum, nSta);
      h2_reg_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
      if (lh.iStation >= fh.iStation)
        h2_reg_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
    }else{
      h_reg_mom_sec->Fill(momentum);
      h_reg_nhits_sec->Fill(nSta);
      if (momentum > 0.01){
        h2_reg_nhits_vs_mom_sec->Fill(momentum, nSta);
        h2_reg_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
        if (lh.iStation >= fh.iStation)
          h2_reg_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
      }
    }

    if( ! mtra.IsReconstructable() ) continue;
    mc_total++;
    
    if ( mtra.IsPrimary() ){
      h_acc_mom_prim->Fill(momentum);
      h_acc_nhits_prim->Fill(nSta);
      h2_acc_nhits_vs_mom_prim->Fill(momentum, nSta);
      h2_acc_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
      if (lh.iStation >= fh.iStation)
        h2_acc_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
    }else{
      h_acc_mom_sec->Fill(momentum);
      h_acc_nhits_sec->Fill(nSta);
      if (momentum > 0.01){
        h2_acc_nhits_vs_mom_sec->Fill(momentum, nSta);
        h2_acc_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
        if (lh.iStation >= fh.iStation)
          h2_acc_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
      }
    }
      

      // vertex distribution of primary and secondary tracks   
    h2_vertex->Fill(mtra.z, mtra.y);
    //h3_vertex->Fill(mtra.z, mtra.x, mtra.y);
    if (mtra.mother_ID < 0){ // primary
      h2_prim_vertex->Fill(mtra.z, mtra.y);
      //h3_prim_vertex->Fill(mtra.z, mtra.x, mtra.y);
    }else{
      h2_sec_vertex->Fill(mtra.z, mtra.y);
      //h3_sec_vertex->Fill(mtra.z, mtra.x, mtra.y);
    }

    
    int iph = mtra.StsHits[0];
    CbmL1HitStore &ph = vHitStore[iph];
    
    h_sec_r->Fill(sqrt(fabs(ph.x*ph.x+ph.y*ph.y)));
    if( fabs( mtra.pz )>1.e-8 ){
      h_tx->Fill(mtra.px/mtra.pz);
      h_ty->Fill(mtra.py/mtra.pz);
    }

      // reconstructed tracks
    bool reco = (mtra.IsReconstructed());

    if (reco){
      p_eff_all_vs_mom->Fill(momentum, 100.0);
      if (mtra.mother_ID < 0){ // primary
        p_eff_prim_vs_mom->Fill(momentum, 100.0);
      }else{
        p_eff_sec_vs_mom->Fill(momentum, 100.0);
      }
      if (mtra.mother_ID < 0){ // primary
        h_reco_mom_prim->Fill(momentum);
        h_reco_nhits_prim->Fill(nSta);
        h2_reco_nhits_vs_mom_prim->Fill(momentum, nSta);
        h2_reco_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
        if (lh.iStation >= fh.iStation)
          h2_reco_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
      }else{
        h_reco_mom_sec->Fill(momentum);
        h_reco_nhits_sec->Fill(nSta);
        if (momentum > 0.01){
          h2_reco_nhits_vs_mom_sec->Fill(momentum, nSta);
          h2_reco_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
          if (lh.iStation >= fh.iStation)
            h2_reco_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
        }
      }
    }else{
      h_notfound_mom->Fill(momentum);
      p_eff_all_vs_mom->Fill(momentum, 0.0);
      if (mtra.mother_ID < 0){ // primary
        p_eff_prim_vs_mom->Fill(momentum, 0.0);
      }else{
        p_eff_sec_vs_mom->Fill(momentum, 0.0);
      }
      if (mtra.mother_ID < 0){ // primary
        h_rest_mom_prim->Fill(momentum);
        h_rest_nhits_prim->Fill(nSta);
        h2_rest_nhits_vs_mom_prim->Fill(momentum, nSta);
        h2_rest_fstation_vs_mom_prim->Fill(momentum, fh.iStation+1);
        if (lh.iStation >= fh.iStation)
          h2_rest_lstation_vs_fstation_prim->Fill(fh.iStation+1, lh.iStation+1);
      }else{
        h_rest_mom_sec->Fill(momentum);
        h_rest_nhits_sec->Fill(nSta);
        if (momentum > 0.01){
          h2_rest_nhits_vs_mom_sec->Fill(momentum, nSta);
          h2_rest_fstation_vs_mom_sec->Fill(momentum, fh.iStation+1);
          if (lh.iStation >= fh.iStation)
            h2_rest_lstation_vs_fstation_sec->Fill(fh.iStation+1, lh.iStation+1);
        }
      }
    }

          // killed tracks. At least one hit of they belong to some recoTrack
    bool killed = 0;
    if(!reco){
      h_notfound_nhits->Fill(nmchits);
      h_notfound_station->Fill(ph.iStation);
      h_notfound_r->Fill(sqrt(fabs(ph.x*ph.x+ph.y*ph.y)));
      CbmL1HitStore &ph21 = vHitStore[mtra.StsHits[0]];
      CbmL1HitStore &ph22 = vHitStore[mtra.StsHits[1]];
      
      double z21 = algo->vStations[ph21.iStation].z[0];
      double z22 = algo->vStations[ph22.iStation].z[0];
      if( fabs(z22-z21)>1.e-4 ){
        h_notfound_tx->Fill((ph22.x-ph21.x)/(z22-z21));
        h_notfound_ty->Fill((ph22.y-ph21.y)/(z22-z21));
      }

      if( mtra.IsDisturbed() ) killed = 1;
    }



    
    if (( mtra.IsPrimary() )&&(mtra.z > 0)){ // D0
      h_reco_d0_mom->Fill(momentum);
      if (reco) p_eff_d0_vs_mom->Fill(momentum, 100.0);
      else      p_eff_d0_vs_mom->Fill(momentum,   0.0);
    }

  } // for mcTracks

  int NFakes = 0;
  for( unsigned int ih=0; ih<algo->vStsHits.size(); ih++){
    int iMC = vHitMCRef[ih];
    if (iMC < 0) NFakes++;
  }

  h_reco_time->Fill(algo->CATime);
  h_reco_timeNtr->Fill(mc_total,algo->CATime);
  h_reco_timeNhit->Fill(algo->vStsHits.size(),algo->CATime);

  h_reco_fakeNtr->Fill(mc_total,NFakes);
  h_reco_fakeNhit->Fill(algo->vStsHits.size()-NFakes,NFakes);

} // void CbmL1::HistoPerformance()


void CbmL1::TrackFitPerformance()
{
  const int Nh_fit = 12;
  static TH1F *h_fit[Nh_fit], *h_fitL[Nh_fit], *h_fitSV[Nh_fit], *h_fitPV[Nh_fit], *h_fit_chi2;

  static bool first_call = 1;

  if ( first_call )
  {
    first_call = 0;

      //TDirectory *maindir = gDirectory;
      //histodir->cd();

      //TDirectory *curdir = gDirectory;
      //histodir = gROOT->mkdir("L1");
    histodir->cd();
    histodir->mkdir("Fit");
    gDirectory->cd("Fit");
    {
      
      struct {
        const char *name;
        const char *title;
        Int_t n;
        Double_t l,r;
      } Table[Nh_fit]=
      {
        {"x",  "Residual X [#mum]",                   100, -100., 100.},
        {"y",  "Residual Y [#mum]",                   100, -100., 100.},
        {"tx", "Residual Tx [mrad]",                  100,   -2.,   2.},
        {"ty", "Residual Ty [mrad]",                  100,   -2.,   2.},
        {"P",  "Resolution P/Q [100%]",               100,   -1,  1 },
        {"px", "Pull X [residual/estimated_error]",   100,  -10.,  10.},
        {"py", "Pull Y [residual/estimated_error]",   100,  -10.,  10.},
        {"ptx","Pull Tx [residual/estimated_error]",  100,  -10.,  10.},
        {"pty","Pull Ty [residual/estimated_error]",  100,  -10.,  10.},
        {"pQP","Pull Q/P [residual/estimated_error]", 100,  -10.,  10.},
        {"QPreco","Reco Q/P ", 100,  -10.,  10.},
        {"QPmc","MC Q/P ", 100,  -10.,  10.}
      };
      
      for( int i=0; i<Nh_fit; i++ ){
        char n[225], t[255];
        sprintf(n,"fst_%s",Table[i].name);
        sprintf(t,"First point %s",Table[i].title);
        h_fit[i] = new TH1F(n,t, Table[i].n, Table[i].l, Table[i].r);
        sprintf(n,"lst_%s",Table[i].name);
        sprintf(t,"Last point %s",Table[i].title);
        h_fitL[i] = new TH1F(n,t, Table[i].n, Table[i].l, Table[i].r);
        sprintf(n,"svrt_%s",Table[i].name);
        sprintf(t,"Secondary vertex point %s",Table[i].title);
        h_fitSV[i] = new TH1F(n,t, Table[i].n, Table[i].l, Table[i].r);
        sprintf(n,"pvrt_%s",Table[i].name);
        sprintf(t,"Primary vertex point %s",Table[i].title);
        h_fitPV[i] = new TH1F(n,t, Table[i].n, Table[i].l, Table[i].r);
      }
      h_fit_chi2 = new TH1F("h_fit_chi2", "Chi2/NDF", 50, -0.5, 10.0);
    }
    gDirectory->cd("..");
  } // if first call
  
  for (vector<CbmL1Track>::iterator it = vRTracks.begin(); it != vRTracks.end(); ++it){

    if ( it->IsGhost() ) continue;
    { // first hit
      int iMC = vHitMCRef[it->StsHits.front()]; // TODO: adapt to linking
      if (iMC < 0) continue;
      CbmL1MCPoint &mc = vMCPoints[iMC];
      h_fit[0]->Fill( (mc.x-it->T[0]) *1.e4);
      h_fit[1]->Fill( (mc.y-it->T[1]) *1.e4);
      h_fit[2]->Fill((mc.px/mc.pz-it->T[2])*1.e3);
      h_fit[3]->Fill((mc.py/mc.pz-it->T[3])*1.e3);
      h_fit[4]->Fill(it->T[4]/mc.q*mc.p-1);
      if( finite(it->C[0]) && it->C[0]>0 )h_fit[5]->Fill( (mc.x-it->T[0])/sqrt(it->C[0]));
      if( finite(it->C[2]) && it->C[2]>0 )h_fit[6]->Fill( (mc.y-it->T[1])/sqrt(it->C[2]));
      if( finite(it->C[5]) && it->C[5]>0 )h_fit[7]->Fill( (mc.px/mc.pz-it->T[2])/sqrt(it->C[5]));
      if( finite(it->C[9]) && it->C[9]>0 )h_fit[8]->Fill( (mc.py/mc.pz-it->T[3])/sqrt(it->C[9]));
      if( finite(it->C[14]) && it->C[14]>0 )h_fit[9]->Fill( (mc.q/mc.p-it->T[4])/sqrt(it->C[14]));
      h_fit[10]->Fill(it->T[4]);
      h_fit[11]->Fill(mc.q/mc.p);
    }
    
    { // last hit
      int iMC = vHitMCRef[it->StsHits.back()];
      if (iMC < 0) continue;
      CbmL1MCPoint &mc = vMCPoints[iMC];
      h_fitL[0]->Fill( (mc.x-it->TLast[0]) *1.e4);
      h_fitL[1]->Fill( (mc.y-it->TLast[1]) *1.e4);
      h_fitL[2]->Fill((mc.px/mc.pz-it->TLast[2])*1.e3);
      h_fitL[3]->Fill((mc.py/mc.pz-it->TLast[3])*1.e3);
      h_fitL[4]->Fill(it->T[4]/mc.q*mc.p-1);
      if( finite(it->CLast[0]) && it->CLast[0]>0 ) h_fitL[5]->Fill( (mc.x-it->TLast[0])/sqrt(it->CLast[0]));
      if( finite(it->CLast[2]) && it->CLast[2]>0 ) h_fitL[6]->Fill( (mc.y-it->TLast[1])/sqrt(it->CLast[2]));
      if( finite(it->CLast[5]) && it->CLast[5]>0 ) h_fitL[7]->Fill( (mc.px/mc.pz-it->TLast[2])/sqrt(it->CLast[5]));
      if( finite(it->CLast[9]) && it->CLast[9]>0 ) h_fitL[8]->Fill( (mc.py/mc.pz-it->TLast[3])/sqrt(it->CLast[9]));
      if( finite(it->CLast[14]) && it->CLast[14]>0 ) h_fitL[9]->Fill( (mc.q/mc.p-it->TLast[4])/sqrt(it->CLast[14]));
      h_fitL[10]->Fill(it->TLast[4]);
      h_fitL[11]->Fill(mc.q/mc.p);
    }

    { // vertex
      CbmL1MCTrack mc = *(it->GetMCTracks()[0]);
      L1TrackPar trPar(it->T,it->C);
      
      if (mc.mother_ID != -1){ // secondary

        {  // extrapolate to vertex
          L1FieldRegion fld _fvecalignment;
          L1FieldValue B[3] _fvecalignment;
          float z[3];
          for (int ih = 0; ih < 3; ih++){
            const int iMCP = mc.Points[ih];
            CbmL1MCPoint &mcP = vMCPoints[iMCP];
            L1Station &st = algo->vStations[mcP.iStation];
            z[ih] = st.z[0];
            st.fieldSlice.GetFieldValue( mcP.x, mcP.y, B[ih] );
          };
          fld.Set( B[0], z[0], B[1], z[1], B[2], z[2] );
          
          L1Extrapolate(trPar, mc.z, trPar.qp, fld);
            // add material
          const int fSta = vHitStore[it->StsHits[0]].iStation;
          const int dir = int((mc.z - algo->vStations[fSta].z[0])/fabs(mc.z - algo->vStations[fSta].z[0]));
  //         if (abs(mc.z - algo->vStations[fSta].z[0]) > 10.) continue; // can't extrapolate on large distance
          for (int iSta = fSta/*+dir*/; (iSta >= 0) && (iSta < NStation) && (dir*(mc.z - algo->vStations[iSta].z[0]) > 0); iSta += dir){
  //           cout << iSta << " " << dir << endl;
            L1AddMaterial( trPar, algo->vStations[iSta].materialInfo, trPar.qp );
            if (iSta+dir == NMvdStations-1) L1AddPipeMaterial( trPar, trPar.qp );
          }
        }
        if (mc.z != trPar.z[0]) continue;
  //       static int good = 0;
  //       static int bad = 0;
  //       if (mc.z != trPar.z[0]){
  //         bad++;
  //         continue;
  //       }
  //       else good++;
  //       cout << "bad\\good" << bad << " " << good << endl;
        
        // calculate pulls
        h_fitSV[0]->Fill( (mc.x-trPar.x[0]) *1.e4);
        h_fitSV[1]->Fill( (mc.y-trPar.y[0]) *1.e4);
        h_fitSV[2]->Fill((mc.px/mc.pz-trPar.tx[0])*1.e3);
        h_fitSV[3]->Fill((mc.py/mc.pz-trPar.ty[0])*1.e3);
        h_fitSV[4]->Fill(trPar.qp[0]/mc.q*mc.p-1);
        if( finite(trPar.C00[0]) && trPar.C00[0]>0 ) h_fitSV[5]->Fill( (mc.x-trPar.x[0])/sqrt(trPar.C00[0]));
        if( finite(trPar.C11[0]) && trPar.C11[0]>0 ) h_fitSV[6]->Fill( (mc.y-trPar.y[0])/sqrt(trPar.C11[0]));
        if( finite(trPar.C22[0]) && trPar.C22[0]>0 ) h_fitSV[7]->Fill( (mc.px/mc.pz-trPar.tx[0])/sqrt(trPar.C22[0]));
        if( finite(trPar.C33[0]) && trPar.C33[0]>0 ) h_fitSV[8]->Fill( (mc.py/mc.pz-trPar.ty[0])/sqrt(trPar.C33[0]));
        if( finite(trPar.C44[0]) && trPar.C44[0]>0 ) h_fitSV[9]->Fill( (mc.q/mc.p-trPar.qp[0])/sqrt(trPar.C44[0]));
        h_fitSV[10]->Fill(trPar.qp[0]);
        h_fitSV[11]->Fill(mc.q/mc.p);
      }
      else{ // primary
        if (vHitStore[it->StsHits[0]].iStation != 0) continue; // only mvd tracks

// #define L1EXTRAPOLATE
#ifdef L1EXTRAPOLATE
        {  // extrapolate to vertex
          L1FieldRegion fld _fvecalignment;
          L1FieldValue B[3], targB _fvecalignment;
          float z[3];
          for (int ih = 0; ih < 3; ih++){
            const int iMCP = mc.Points[ih];
            CbmL1MCPoint &mcP = vMCPoints[iMCP];
            L1Station &st = algo->vStations[mcP.iStation];
            z[ih] = st.z[0];
//             cout << mcP.z << endl;
            st.fieldSlice.GetFieldValue( mcP.x, mcP.y, B[ih] );
          };
//           cout << mc.x << " " << mc.y << " " << mc.z << endl;

//           targetFieldSlice->GetFieldValue( mc.x, mc.y, targB );
          targB = algo->vtxFieldValue;
          fld.Set( targB, 0., B[0], z[0], B[1], z[1] );

          L1Extrapolate(trPar, mc.z, trPar.qp, fld);
//           L1Extrapolate0(trPar, mc.z, fld);
            // add material
          const int fSta = vHitStore[it->StsHits[0]].iStation;
          
          const int dir = (mc.z - algo->vStations[fSta].z[0])/abs(mc.z - algo->vStations[fSta].z[0]);
  //         if (abs(mc.z - algo->vStations[fSta].z[0]) > 10.) continue; // can't extrapolate on large distance
          for (int iSta = fSta/*+dir*/; (iSta >= 0) && (iSta < NStation) && (dir*(mc.z - algo->vStations[iSta].z[0]) > 0); iSta += dir){
  //           cout << iSta << " " << dir << endl;
            L1AddMaterial( trPar, algo->vStations[iSta].materialInfo, trPar.qp );
            if (iSta+dir == NMvdStations-1) L1AddPipeMaterial( trPar, trPar.qp );
          }
        }
        if (mc.z != trPar.z[0]) continue;
  //       static int good = 0;
  //       static int bad = 0;
  //       if (mc.z != trPar.z[0]){
  //         bad++;
  //         continue;
  //       }
  //       else good++;
  //       cout << "bad\\good" << bad << " " << good << endl;

        // calculate pulls
        h_fitPV[0]->Fill( (mc.x-trPar.x[0]) *1.e4);
        h_fitPV[1]->Fill( (mc.y-trPar.y[0]) *1.e4);
        h_fitPV[2]->Fill((mc.px/mc.pz-trPar.tx[0])*1.e3);
        h_fitPV[3]->Fill((mc.py/mc.pz-trPar.ty[0])*1.e3);
        h_fitPV[4]->Fill(trPar.qp[0]/mc.q*mc.p-1);
        if( finite(trPar.C00[0]) && trPar.C00[0]>0 ) h_fitPV[5]->Fill( (mc.x-trPar.x[0])/sqrt(trPar.C00[0]));
        if( finite(trPar.C11[0]) && trPar.C11[0]>0 ) h_fitPV[6]->Fill( (mc.y-trPar.y[0])/sqrt(trPar.C11[0]));
        if( finite(trPar.C22[0]) && trPar.C22[0]>0 ) h_fitPV[7]->Fill( (mc.px/mc.pz-trPar.tx[0])/sqrt(trPar.C22[0]));
        if( finite(trPar.C33[0]) && trPar.C33[0]>0 ) h_fitPV[8]->Fill( (mc.py/mc.pz-trPar.ty[0])/sqrt(trPar.C33[0]));
        if( finite(trPar.C44[0]) && trPar.C44[0]>0 ) h_fitPV[9]->Fill( (mc.q/mc.p-trPar.qp[0])/sqrt(trPar.C44[0]));
        h_fitPV[10]->Fill(trPar.qp[0]);
        h_fitPV[11]->Fill(mc.q/mc.p);
#else
        FairTrackParam fTP;
        fTP.SetX(it->T[0]);
        fTP.SetY(it->T[1]);
        fTP.SetZ(it->T[5]);
        fTP.SetTx(it->T[2]);
        fTP.SetTy(it->T[3]);
        fTP.SetQp(it->T[4]);
        fTP.SetCovariance(0,0, trPar.C00[0]);
        fTP.SetCovariance(1,0, trPar.C10[0]);
        fTP.SetCovariance(1,1, trPar.C11[0]);
        fTP.SetCovariance(2,0, trPar.C20[0]);
        fTP.SetCovariance(2,1, trPar.C21[0]);
        fTP.SetCovariance(2,2, trPar.C22[0]);
        fTP.SetCovariance(3,0, trPar.C30[0]);
        fTP.SetCovariance(3,1, trPar.C31[0]);
        fTP.SetCovariance(3,2, trPar.C32[0]);
        fTP.SetCovariance(3,3, trPar.C33[0]);
        fTP.SetCovariance(4,0, trPar.C40[0]);
        fTP.SetCovariance(4,1, trPar.C41[0]);
        fTP.SetCovariance(4,2, trPar.C42[0]);
        fTP.SetCovariance(4,3, trPar.C43[0]);
        fTP.SetCovariance(4,4, trPar.C44[0]);
        CbmKFTrack kfTr;
        kfTr.SetTrackParam(fTP);
        kfTr.Propagate(mc.z);
        CbmL1Track it2;
        for (int ipar = 0; ipar < 6; ipar++) it2.T[ipar] = kfTr.GetTrack()[ipar];
        for (int ipar = 0; ipar < 15; ipar++) it2.C[ipar] = kfTr.GetCovMatrix()[ipar];


        // calculate pulls
        h_fitPV[0]->Fill( (mc.x-it2.T[0]) *1.e4);
        h_fitPV[1]->Fill( (mc.y-it2.T[1]) *1.e4);
        h_fitPV[2]->Fill((mc.px/mc.pz-it2.T[2])*1.e3);
        h_fitPV[3]->Fill((mc.py/mc.pz-it2.T[3])*1.e3);
        h_fitPV[4]->Fill(it2.T[4]/mc.q*mc.p-1);
        if( finite(it2.C[0]) && it2.C[0]>0 )h_fitPV[5]->Fill( (mc.x-it2.T[0])/sqrt(it2.C[0]));
        if( finite(it2.C[2]) && it2.C[2]>0 )h_fitPV[6]->Fill( (mc.y-it2.T[1])/sqrt(it2.C[2]));
        if( finite(it2.C[5]) && it2.C[5]>0 )h_fitPV[7]->Fill( (mc.px/mc.pz-it2.T[2])/sqrt(it2.C[5]));
        if( finite(it2.C[9]) && it2.C[9]>0 )h_fitPV[8]->Fill( (mc.py/mc.pz-it2.T[3])/sqrt(it2.C[9]));
        if( finite(it2.C[14]) && it2.C[14]>0 )h_fitPV[9]->Fill( (mc.q/mc.p-it2.T[4])/sqrt(it2.C[14]));
        h_fitPV[10]->Fill(it2.T[4]);
        h_fitPV[11]->Fill(mc.q/mc.p);
#endif // L1EXTRAPOLATE
      }
    }
    
    h_fit_chi2->Fill(it->chi2/it->NDF);
  }

} // void CbmL1::TrackFitPerformance()



void CbmL1::FieldApproxCheck()
{
  TDirectory *curr = gDirectory;
  TFile* fout = new TFile("FieldApprox.root","RECREATE");
  fout->cd();

  FairField *MF = CbmKF::Instance()->GetMagneticField();
  for ( int ist = 0; ist<NStation; ist++ )
  {
    double z = 0;
    double Xmax=-100, Ymax=-100;
    if( ist<NMvdStations ){
      CbmKFTube &t = CbmKF::Instance()->vMvdMaterial[ist];
      z = t.z;
      Xmax = Ymax = t.R;
    }else{
      CbmStsStation *st = StsDigi.GetStation(ist - NMvdStations);
      z = st->GetZ();

      double x,y;
      for(int isec = 0; isec < st->GetNSectors(); isec++)
      {
        CbmStsSector *sect = (CbmStsSector*) st->GetSector(isec);
        for(int isen = 0; isen < sect->GetNSensors(); isen++)
        {
          x = sect->GetSensor(isen)->GetX0() + sect->GetSensor(isen)->GetLx()/2.;
          y = sect->GetSensor(isen)->GetY0() + sect->GetSensor(isen)->GetLy()/2.;
          if(x>Xmax) Xmax = x;
          if(y>Ymax) Ymax = y;
        }
      }
      cout << "Station  "<<  ist << ",  Xmax  " << Xmax<<",  Ymax" << Ymax<<endl;
  
    } // if mvd


    float step = 1.;

    int NbinsX = (int) (2*Xmax/step);
    int NbinsY = (int) (2*Ymax/step);
    float ddx = 2*Xmax/NbinsX;
    float ddy = 2*Ymax/NbinsY;

    TH2F *stB  = new TH2F(Form("station %i, dB", ist+1) ,Form("station %i, dB, z = %0.f cm", ist+1,z) , (int) (NbinsX+1),-(Xmax+ddx/2.),(Xmax+ddx/2.), (int) (NbinsY+1),-(Ymax+ddy/2.),(Ymax+ddy/2.));
    TH2F *stBx = new TH2F(Form("station %i, dBx", ist+1),Form("station %i, dBx, z = %0.f cm", ist+1,z), (int) (NbinsX+1),-(Xmax+ddx/2.),(Xmax+ddx/2.), (int) (NbinsY+1),-(Ymax+ddy/2.),(Ymax+ddy/2.));
    TH2F *stBy = new TH2F(Form("station %i, dBy", ist+1),Form("station %i, dBy, z = %0.f cm", ist+1,z), (int) (NbinsX+1),-(Xmax+ddx/2.),(Xmax+ddx/2.), (int) (NbinsY+1),-(Ymax+ddy/2.),(Ymax+ddy/2.));
    TH2F *stBz = new TH2F(Form("station %i, dBz", ist+1),Form("station %i, dBz, z = %0.f cm", ist+1,z), (int) (NbinsX+1),-(Xmax+ddx/2.),(Xmax+ddx/2.), (int) (NbinsY+1),-(Ymax+ddy/2.),(Ymax+ddy/2.));

    Double_t r[3],B[3];
    L1FieldSlice FSl;
    L1FieldValue B_L1;
    Double_t bbb, bbb_L1;

    const int M=5; // polinom order
    const int N=(M+1)*(M+2)/2;
    L1Station &st = algo->vStations[ist];
    for(int i=0; i<N; i++)
    {
      FSl.cx[i] = st.fieldSlice.cx[i][0];
      FSl.cy[i] = st.fieldSlice.cy[i][0];
      FSl.cz[i] = st.fieldSlice.cz[i][0];
    }

    Int_t i=1,j=1;

    double x,y;
    for(int ii = 1; ii <=NbinsX+1; ii++)
    {
      j=1;
      x = -Xmax+(ii-1)*ddx;
      for(int jj = 1; jj <=NbinsY+1; jj++)
      {
        y = -Ymax+(jj-1)*ddy;
        double rrr = sqrt(fabs(x*x/Xmax/Xmax+y/Ymax*y/Ymax));
        if(rrr>1. )
        {
          j++;
          continue;
        }
        r[2] = z; r[0] = x; r[1] = y;
        MF->GetFieldValue( r, B );
        bbb = sqrt(B[0]*B[0]+B[1]*B[1]+B[2]*B[2]);

        FSl.GetFieldValue(x,y,B_L1);
        bbb_L1 = sqrt(B_L1.x[0]*B_L1.x[0] + B_L1.y[0]*B_L1.y[0] + B_L1.z[0]*B_L1.z[0]);

        stB  -> SetBinContent(ii,jj,(bbb - bbb_L1));
        stBx -> SetBinContent(ii,jj,(B[0] - B_L1.x[0]));
        stBy -> SetBinContent(ii,jj,(B[1] - B_L1.y[0]));
        stBz -> SetBinContent(ii,jj,(B[2] - B_L1.z[0]));
        j++;
      }
      i++;
    }

    stB   ->GetXaxis()->SetTitle("X, cm");
    stB   ->GetYaxis()->SetTitle("Y, cm");
    stB   ->GetXaxis()->SetTitleOffset(1);
    stB   ->GetYaxis()->SetTitleOffset(1);
    stB   ->GetZaxis()->SetTitle("B_map - B_L1, kGauss");
    stB   ->GetZaxis()->SetTitleOffset(1.3);

    stBx  ->GetXaxis()->SetTitle("X, cm");
    stBx  ->GetYaxis()->SetTitle("Y, cm");
    stBx  ->GetXaxis()->SetTitleOffset(1);
    stBx  ->GetYaxis()->SetTitleOffset(1);
    stBx  ->GetZaxis()->SetTitle("Bx_map - Bx_L1, kGauss");
    stBx  ->GetZaxis()->SetTitleOffset(1.3);

    stBy  ->GetXaxis()->SetTitle("X, cm");
    stBy  ->GetYaxis()->SetTitle("Y, cm");
    stBy  ->GetXaxis()->SetTitleOffset(1);
    stBy  ->GetYaxis()->SetTitleOffset(1);
    stBy  ->GetZaxis()->SetTitle("By_map - By_L1, kGauss");
    stBy  ->GetZaxis()->SetTitleOffset(1.3);

    stBz  ->GetXaxis()->SetTitle("X, cm");
    stBz  ->GetYaxis()->SetTitle("Y, cm");
    stBz  ->GetXaxis()->SetTitleOffset(1);
    stBz  ->GetYaxis()->SetTitleOffset(1);
    stBz  ->GetZaxis()->SetTitle("Bz_map - Bz_L1, kGauss");
    stBz  ->GetZaxis()->SetTitleOffset(1.3);

    stB  -> Write();
    stBx -> Write();
    stBy -> Write();
    stBz -> Write();
    
  } // for ista

  fout->Close();
  curr->cd();
} // void CbmL1::FieldApproxCheck()

void CbmL1::InputPerformance()
{
  static TH1I *nStripFHits, *nStripBHits, *nStripFMC, *nStripBMC;

  static TH1F *resX, *resY/*, *pullZ*/;
  static TH1F *pullX, *pullY/*, *pullZ*/;
  
  static bool first_call = 1;
  if ( first_call ){
    first_call = 0;
    
    TDirectory *curr = gDirectory;
    histodir->cd();
    histodir->mkdir("Input");
    histodir->cd("Input");
    gDirectory->cd("Input");
    
    nStripFHits = new TH1I("nHits_f", "NHits On Front Strip", 10, 0, 10);
    nStripBHits = new TH1I("nHits_b", "NHits On Back Strip", 10, 0, 10);
    nStripFMC = new TH1I("nMC_f", "N MC Points On Front Strip", 10, 0, 10);
    nStripBMC = new TH1I("nMC_b", "N MC Points On Back Strip", 10, 0, 10);

    pullX = new TH1F("Px", "Pull x", 50, -10, 10);
    pullY = new TH1F("Py", "Pull y", 50, -10, 10);

    resX = new TH1F("x", "Residual x", 50, -200, 200);
    resY = new TH1F("y", "Residual y", 50, -200, 200);
    TH1* histo;
    histo = resX;
    histo->GetXaxis()->SetTitle("Residual x, um");
    histo = resY;
    histo->GetXaxis()->SetTitle("Residual y, um");
    
    curr->cd();
  } // first_call

  std::map<unsigned int, unsigned int> stripFToNHitMap,stripBToNHitMap;
  std::map<unsigned int, unsigned int> stripFToNMCMap,stripBToNMCMap;

  map<unsigned int, unsigned int>::iterator it;
  Int_t nMC = -1;
  if( listStsPts ){
    nMC = listStsPts->GetEntries();
  }
  if( 0 && listStsHits ){
    Int_t nEnt = listStsHits->GetEntries();
//     Int_t nMC = listStsPts->GetEntries();
    for (int j=0; j < nEnt; j++ ){
      CbmStsHit *sh = (CbmStsHit*) listStsHits->At(j);
      
        // strip - MC correspondence
      int iStripF = sh->GetDigi(0);
      int iStripB = sh->GetDigi(1);
      if ( iStripF >= 0 ) stripFToNHitMap[iStripF]++;
      if ( iStripB >= 0 ) stripBToNHitMap[iStripB]++;
      int iMC = sh->GetRefIndex();  // TODO: adapt to linking
      if( iMC>=0 && iMC<nMC){
        if ( iStripF >= 0 ) stripFToNMCMap[iStripF]++;
        if ( iStripB >= 0 ) stripBToNMCMap[iStripB]++;
      }

        // hit pulls and residuals

      TVector3 hitPos, mcPos, hitErr;
      sh->Position(hitPos);
      sh->PositionError(hitErr);
      CbmStsPoint *pt = 0;
      if( iMC>=0 && iMC<nMC) pt = (CbmStsPoint*) listStsPts->At(iMC);
      if ( !pt ){
//         cout << " No MC points! " << "iMC=" << iMC << endl;
        continue;
      }
//       pt->Position(mcPos); // this is wrong! 
      mcPos.SetX( pt->GetX( hitPos.Z() ) );
      mcPos.SetY( pt->GetY( hitPos.Z() ) );
      mcPos.SetZ( hitPos.Z() );
      
//       if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / hitErr.X() ); // standard errors
//       if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / hitErr.Y() );
//       if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / sh->GetDx() ); // qa errors
//       if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / sh->GetDy() );
      if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / sqrt(algo->vStations[NMvdStations].XYInfo.C00[0]) );  // errors used in TF
      if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / sqrt(algo->vStations[NMvdStations].XYInfo.C11[0]) );

      resX->Fill((hitPos.X() - mcPos.X())*10*1000);
      resY->Fill((hitPos.Y() - mcPos.Y())*10*1000);
      
    }
  } // sts
  if( listMvdHits ){
    Int_t nEnt = listMvdHits->GetEntries();
//     Int_t nMC = listStsPts->GetEntries();
    for (int j=0; j < nEnt; j++ ){
      CbmMvdHit *sh = (CbmMvdHit*) listMvdHits->At(j);
      CbmMvdHitMatch *hm = (CbmMvdHitMatch*) listMvdHitMatches->At(j);

//       int iMC = sh->GetRefIndex();
      int iMC = hm->GetPointId();

        // hit pulls and residuals

      TVector3 hitPos, mcPos, hitErr;
      sh->Position(hitPos);
      sh->PositionError(hitErr);
      CbmMvdPoint *pt = 0;
      if( iMC>=0 && iMC<nMC) pt = (CbmMvdPoint*) listMvdPts->At(iMC);
      if ( !pt ){
//         cout << " No MC points! " << "iMC=" << iMC << endl;
        continue;
      }
//       pt->Position(mcPos); // this is wrong! 
      mcPos.SetX( ( pt->GetX() + pt->GetXOut() )/2. );
      mcPos.SetY( ( pt->GetY() + pt->GetYOut() )/2 );
      mcPos.SetZ( hitPos.Z() );
      
//       if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / hitErr.X() ); // standard errors
//       if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / hitErr.Y() );
//       if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / sh->GetDx() ); // qa errors
//       if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / sh->GetDy() );
      if (hitErr.X() != 0) pullX->Fill( (hitPos.X() - mcPos.X()) / sqrt(algo->vStations[0].XYInfo.C00[0]) );  // errors used in TF
      if (hitErr.Y() != 0) pullY->Fill( (hitPos.Y() - mcPos.Y()) / sqrt(algo->vStations[0].XYInfo.C11[0]) );

      resX->Fill((hitPos.X() - mcPos.X())*10*1000);
      resY->Fill((hitPos.Y() - mcPos.Y())*10*1000);
      
    }
  } // mvd


  
  for (it = stripFToNHitMap.begin(); it != stripFToNHitMap.end(); it++){
    nStripFHits->Fill(it->second);
  }
  for (it = stripBToNHitMap.begin(); it != stripBToNHitMap.end(); it++){
    nStripBHits->Fill(it->second);
  }
  for (it = stripFToNMCMap.begin(); it != stripFToNMCMap.end(); it++){
    nStripFMC->Fill(it->second);
  }
  for (it = stripBToNMCMap.begin(); it != stripBToNMCMap.end(); it++){
    nStripBMC->Fill(it->second);
  }

    // strips   Not ended
//   if( listCbmStsDigi ){
//     Int_t nEnt = listCbmStsDigi->GetEntries();
//     for (int j=0; j < nEnt; j++ ){
//       CbmStsDigi *digi = (CbmStsDigi*) listCbmStsDigi->At(j);
// //  = sh->GetNLinks(0);
//         // find position of mc point
//       FairLink link = digi->GetLink(0);
//       int iMCPoint = link.GetIndex();
//       CbmStsPoint *mcPoint = (CbmStsPoint*) listStsPts->At(iMCPoint);
//       TVector3 mcPos;
//       if (digi->GetSide() == 0)
//         mcPoint->PositionIn(mcPos);
//       else
//         mcPoint->PositionOut(mcPos);
// 
//       CbmStsStation *sta = StsDigi.GetStation(digi->GetStationNr());
//       CbmStsSector* sector = sta->GetSector(digi->GetSectorNr());
//       digi->GetChannelNr();
//       
//     }
//   } // listCbmStsDigi
  
  
} // void CbmL1::InputPerformance()

