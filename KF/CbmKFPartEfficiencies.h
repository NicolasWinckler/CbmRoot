#ifndef CbmKFPartEfficiencies_H
#define CbmKFPartEfficiencies_H

#include "TNamed.h"
#include "CbmL1Counters.h"

class CbmKFPartEfficiencies: public TNamed
{
 public:

  CbmKFPartEfficiencies():
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
    int mPartPDG[nParticles] = {310,3122,3312,-3312,3334,3000,421,411,-411};
    TString mPartName[nParticles] = {"ks","lambda","ksi-","ksi+","omega-","Hdb","D0","D+","D-"};
    TString mPartTitle[nParticles] = {"KShort",
                                      "Lambda",
                                      "Ksi-  ",
                                      "Ksi+  ",
                                      "Omega-",
                                      "Hdb   ",
                                      "D0    ",
                                      "D+    ",
                                      "D-    "};

    for(int iP=0; iP<nParticles; iP++)
    {
      partPDG[iP] = mPartPDG[iP];
      partName[iP] = mPartName[iP];
      partTitle[iP] = mPartTitle[iP];
    }

    for(int iP=0; iP<nParticles; iP++)
    {
      AddCounter(Form("%s",partName[iP].Data()), Form("%-*s",11,partTitle[iP].Data()));
      AddCounter(Form("%s_prim",partName[iP].Data()), Form("%s Prim",partTitle[iP].Data()));
      AddCounter(Form("%s_sec",partName[iP].Data()), Form("%s Sec ",partTitle[iP].Data()));
    }
  }

  virtual ~CbmKFPartEfficiencies(){};

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

  CbmKFPartEfficiencies& operator+=(CbmKFPartEfficiencies& a){
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
    std::cout.setf(ios::fixed);
    std::cout.setf(ios::showpoint);
    std::cout.precision(3);
    std::cout << "Particle     : "
         << "   Eff "
         <<" / "<< " Ghost "
         <<" / "<< "BackGr "
         <<" / "<< "N Ghost"
         <<" / "<< "N BackGr"
         <<" / "<< "N Reco "
         <<" | "<< "  N MC "  << std::endl;
    
    int NCounters = mc.NCounters;
    for (int iC = 0; iC < NCounters; iC++){
        std::cout << names[iC]
             << "  : " << setw(6) << ratio_reco.counters[iC]              
             << "  / " << setw(6) << ratio_ghost.counters[iC]  // particles w\o MCParticle
             << "  / " << setw(6) << ratio_bg.counters[iC]     // particles with incorrect MCParticle
             << "  / " << setw(6) << ghost.counters[iC]
             << "  / " << setw(7) << bg.counters[iC]
             << "  / " << setw(6) << reco.counters[iC]
             << "  | " << setw(6) << mc.counters[iC]  << std::endl;
    }
  };

  friend std::fstream & operator<<(std::fstream &strm, CbmKFPartEfficiencies &a) {

    strm << a.ratio_reco;
    strm << a.mc;
    strm << a.reco;
    strm << a.ratio_ghost;
    strm << a.ratio_bg;
    strm << a.ghost;
    strm << a.bg;

    return strm;
  }

  friend std::fstream & operator>>(std::fstream &strm, CbmKFPartEfficiencies &a){

    strm >> a.ratio_reco;
    strm >> a.mc;
    strm >> a.reco;
    strm >> a.ratio_ghost;
    strm >> a.ratio_bg;
    strm >> a.ghost;
    strm >> a.bg;

    return strm;
  }

  void AddFromFile(TString fileName)
  {
    std::fstream file(fileName.Data(),fstream::in);
    file >> *this;
  }

  static const int nParticles = 9;
  int partPDG[nParticles];
  TString partName[nParticles];
  TString partTitle[nParticles];

  ClassDef(CbmKFPartEfficiencies,1);

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
