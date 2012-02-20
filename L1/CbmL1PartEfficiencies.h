#ifndef CbmL1PartEfficiencies_H
#define CbmL1PartEfficiencies_H

#include "CbmL1Counters.h"

struct CbmL1PartEfficiencies
{
  CbmL1PartEfficiencies():
    names(),
    indices(),
    ratio_reco(),
    mc(),
    reco(),
    ratio_ghost(),
    ratio_bg(),
    ghost(),
    bg()
  {
          // add total efficiency
    // AddCounter("piPlus"  ,"PiPlus  efficiency");
    // AddCounter("piMinus" ,"PiMinus efficiency");
    AddCounter("ks"           ,"KShort     ");
    AddCounter("ks_prim"      ,"KShortPrim ");
    AddCounter("ks_sec"       ,"KShortSec  ");
    AddCounter("lambda"       ,"Lambda     ");
    AddCounter("lambda_prim"  ,"LambdaPrim ");
    AddCounter("lambda_sec"   ,"LambdaSec  ");
  }
  
  virtual ~CbmL1PartEfficiencies(){};

  virtual void AddCounter(TString shortname, TString name){
    indices[shortname] = names.size();
    names.push_back(name);
    
    ratio_reco.AddCounter();
    mc.AddCounter();
    reco.AddCounter();
    
    ratio_ghost.AddCounter();
    ratio_bg.AddCounter();
    ghost.AddCounter();
    bg.AddCounter();
  };
  
  CbmL1PartEfficiencies& operator+=(CbmL1PartEfficiencies& a){
    mc += a.mc; reco += a.reco;
    ghost += a.ghost; bg += a.bg;
    return *this;
  };
  
  void CalcEff(){
    ratio_reco = reco/mc;

    TL1TracksCatCounters<int> allReco = reco + ghost + bg;
    ratio_ghost = ghost/allReco;
    ratio_bg  = bg/allReco;
  };
  

  void Inc(bool isReco, TString name)
  {
    const int index = indices[name];
    
    mc.counters[index]++;
    if (isReco) reco.counters[index]++;
  };

  void IncReco(bool isGhost, bool isBg, TString name){
    const int index = indices[name];

    if (isGhost) ghost.     counters[index]++;
    if (isBg)    bg.counters[index]++;
  };

  void PrintEff(){
    cout.setf(ios::fixed);
    cout.setf(ios::showpoint);
    cout.precision(3);
    cout << "Particle     : "
         << "   Eff "
         <<" / "<< " Ghost "
         <<" / "<< "BackGr "
         <<" / "<< "N Ghost"
         <<" / "<< "N BackGr"
         <<" / "<< "N Reco "
         <<" | "<< "  N MC "  << endl;
    
    int NCounters = mc.NCounters;
    for (int iC = 0; iC < NCounters; iC++){
        cout << names[iC]
             << "  : " << setw(6) << ratio_reco.counters[iC]              
             << "  / " << setw(6) << ratio_ghost.counters[iC]  // particles w\o MCParticle
             << "  / " << setw(6) << ratio_bg.counters[iC]     // particles with incorrect MCParticle
             << "  / " << setw(6) << ghost.counters[iC]
             << "  / " << setw(7) << bg.counters[iC]
             << "  / " << setw(6) << reco.counters[iC]
             << "  | " << setw(6) << mc.counters[iC]  << endl;
    }
  };

 private:
  vector<TString> names; // names counters indexed by index of counter
  map<TString, int> indices; // indices of counters indexed by a counter shortname
  
  TL1TracksCatCounters<double> ratio_reco;
  
  TL1TracksCatCounters<int> mc;
  TL1TracksCatCounters<int> reco;
  
  TL1TracksCatCounters<double> ratio_ghost;
  TL1TracksCatCounters<double> ratio_bg;

  TL1TracksCatCounters<int> ghost;
  TL1TracksCatCounters<int> bg; // background
};

#endif
