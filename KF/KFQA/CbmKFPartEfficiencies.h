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
    ratio_clone(),
    ghost(),
    bg(),
    clone()
  {
          // add total efficiency
    // AddCounter("piPlus"  ,"PiPlus  efficiency");
    // AddCounter("piMinus" ,"PiMinus efficiency");
    int mPartPDG[nParticles] = {310,3122,-3122,3312,-3312,3334,-3334, //strange meson and hyperons
                                313,-313,323,-323, //K* resonances
                                3224,3114,-3114,-3224, //sigma resonances
                                3124,-3124, //Lambda resonances
                                3324, -3324, 1003314, -1003314, //Xi resonances
                                1003334, -100334, //Omega resonances
                                3000, //exotics
                                333,113, //vector mesons, hadron chanel
                                100113, 200113, //light vector mesons
                                22, //dielectrons
                                443,100443, // J/Psi
                                421,-421,100421,-100421, //D0
                                411,-411, //D+, D-
                                431,-431, //Ds+, Ds-
                                4122,-4122, //Lambdac
                                10421, -10421, 10411, -10411, 20411, -20411
                               };
    TString mPartName[nParticles] = {"ks","lambda","lambdab","xi-","xi+","omega-","omega+",
                                     "k*0","k*0b","k*+","k*-",
                                     "sigma*+","sigma*-","sigma*+b","sigma*-b",
                                     "lambda*","lambda*b",
                                     "xi*0", "xi*0b", "xi*-_{#Lambda,K}", "xi*+_{#Lambda,K}",
                                     "omega*-","omega*+",
                                     "Hdb",
                                     "phi_{KK}", "rho_{#pi#pi}",
                                     "rho_{ee}", "rho_{#mu#mu}",
                                     "gamma",
                                     "J#Psi_ee","J#Psi_#mu#mu",
                                     "D0","D0b","D0_4","D0b_4",
                                     "D+","D-",
                                     "Ds+","Ds-",
                                     "lambdac", "lambdacb",
                                     "D*0", "D*0b", "D*+", "D*-", "D*+_4", "D*-_4"
                                    };
    TString mPartTitle[nParticles] = {"KShort   ", //0
                                      "Lambda   ", //1
                                      "Lambda b ", //2
                                      "Xi-      ", //3
                                      "Xi+      ", //4
                                      "Omega-   ", //5
                                      "Omega+   ", //6
                                      "K*0      ", //7
                                      "K*0 b    ", //8
                                      "K*+      ", //9
                                      "K*-      ", //10
                                      "Sigma*+  ", //11
                                      "Sigma*-  ", //12
                                      "Sigma*+ b", //13
                                      "Sigma*- b", //14
                                      "Lambda*  ", //15
                                      "Lambda* b", //16
                                      "Xi*0     ", //17
                                      "Xi*0 b   ", //18
                                      "Xi*-_lk  ", //19
                                      "Xi*+_lk  ", //20
                                      "Omega*-  ", //21
                                      "Omega*+  ", //22
                                      "Hdb      ", //23
                                      "phi_kk   ", //24
                                      "rho_pipi ", //25
                                      "rho_ee   ", //26
                                      "rho_mm   ", //27
                                      "gamma    ", //28
                                      "JPsi_ee  ", //29
                                      "JPsi_mm  ", //30
                                      "D0       ", //31
                                      "D0b      ", //32
                                      "D0_4     ", //33
                                      "D0b_4    ", //34
                                      "D+       ", //35
                                      "D-       ", //36
                                      "Ds+      ", //37
                                      "Ds-      ", //38
                                      "Lambdac  ", //39
                                      "Lambdac b", //40
                                      "D*0      ", //41
                                      "D*0 b    ", //42
                                      "D*+      ", //43
                                      "D*-      ", //44
                                      "D*+_4    ", //45
                                      "D*-_4    " //46
                                     };

    float mPartMHistoMin[nParticles] = {0.3, 1., 1., 1., 1.,1.,1.,
                                        0.6, 0.6, 0.6, 0.6,
                                        1.,1.,1.,1.,
                                        1.4, 1.4,
                                        1.4, 1.4, 1.4, 1.4,
                                        1.8,1.8,
                                        1.,
                                        0.6, 0.1,
                                        0.1, 0.1,
                                        0.,
                                        1.,1.,
                                        1.,1.,1.,1.,
                                        1.,1.,
                                        1.,1.,
                                        1.8,1.8,
                                        1.8,1.8,1.8,1.8,1.8,1.8};
    float mPartMHistoMax[nParticles] = {1.3, 2., 2., 3., 3., 3., 3.,
                                        2.6, 2.6, 2.6, 2.6,
                                        3., 3., 3., 3.,
                                        3.4, 3.4,
                                        3.4, 3.4, 3.4, 3.4,
                                        3.8, 3.8,
                                        3.,
                                        1.6, 2.1,
                                        2.1, 2.1,
                                        3.,
                                        4.,4.,
                                        3.,3.,3.,3.,
                                        3.,3.,
                                        3.,3.,
                                        3.8,3.8,
                                        3.8,3.8,3.8,3.8,3.8,3.8};
    //set decay mode
    partDaughterPdg.resize(nParticles);

    partDaughterPdg[ 0].push_back(  211); //K0s -> pi+ pi-
    partDaughterPdg[ 0].push_back( -211);

    partDaughterPdg[ 1].push_back( 2212); //Lambda -> p pi-
    partDaughterPdg[ 1].push_back( -211);

    partDaughterPdg[ 2].push_back(-2212); //Lambda_bar -> p- pi+
    partDaughterPdg[ 2].push_back(  211);

    partDaughterPdg[ 3].push_back( 3122); //Ksi- -> Lambda pi-
    partDaughterPdg[ 3].push_back( -211);

    partDaughterPdg[ 4].push_back(-3122); //Ksi+ -> Lambda_bar pi+
    partDaughterPdg[ 4].push_back(  211);

    partDaughterPdg[ 5].push_back( 3122); //Omega- -> Lambda K-
    partDaughterPdg[ 5].push_back( -321);

    partDaughterPdg[ 6].push_back(-3122); //Omega+ -> Lambda_bar K+
    partDaughterPdg[ 6].push_back(  321);

    partDaughterPdg[ 7].push_back(  321); //K*0 -> K+ pi-
    partDaughterPdg[ 7].push_back( -211);

    partDaughterPdg[ 8].push_back( -321); //K*0_bar -> K- pi+
    partDaughterPdg[ 8].push_back(  211);

    partDaughterPdg[ 9].push_back(  310); //K*+ -> K0s pi+
    partDaughterPdg[ 9].push_back(  211);

    partDaughterPdg[10].push_back(  310); //K*- -> K0s pi-
    partDaughterPdg[10].push_back( -211);

    partDaughterPdg[11].push_back( 3122); //Sigma+ -> Lambda pi+
    partDaughterPdg[11].push_back(  211);

    partDaughterPdg[12].push_back( 3122); //Sigma- -> Lambda pi-
    partDaughterPdg[12].push_back( -211);

    partDaughterPdg[13].push_back(-3122); //Sigma+_bar -> Lambda_bar pi+
    partDaughterPdg[13].push_back(  211);

    partDaughterPdg[14].push_back(-3122); //Sigma-_bar -> Lambda_bar pi-
    partDaughterPdg[14].push_back( -211);

    partDaughterPdg[15].push_back( 2212); //Lambda* -> p K-
    partDaughterPdg[15].push_back( -321);

    partDaughterPdg[16].push_back(-2212); //Lambda*_bar -> p- K+
    partDaughterPdg[16].push_back(  321);

    partDaughterPdg[17].push_back( 3312); //Xi*0 -> Xi- pi+
    partDaughterPdg[17].push_back(  211);

    partDaughterPdg[18].push_back(-3312); //Xi*0_bar -> Xi+ pi-
    partDaughterPdg[18].push_back( -211);

    partDaughterPdg[19].push_back( 3122); //Xi*- -> Lambda K-
    partDaughterPdg[19].push_back( -321);

    partDaughterPdg[20].push_back(-3122); //Xi*+ -> Lambda_bar K+
    partDaughterPdg[20].push_back(  321);

    partDaughterPdg[21].push_back( 3312); //Omega*- -> Xi- pi+ K-
    partDaughterPdg[21].push_back(  211);
    partDaughterPdg[21].push_back( -321);

    partDaughterPdg[22].push_back(-3312); //Omega*- -> Xi+ pi- K+
    partDaughterPdg[22].push_back( -211);
    partDaughterPdg[22].push_back(  321);

    partDaughterPdg[23].push_back( 3122); //H-dibar -> Lambda Lambda
    partDaughterPdg[23].push_back( 3122);

    partDaughterPdg[24].push_back(  321); //phi -> K+ K-
    partDaughterPdg[24].push_back( -321);

    partDaughterPdg[25].push_back(  211); //rho, omega, phi -> pi+ pi-
    partDaughterPdg[25].push_back( -211);

    partDaughterPdg[26].push_back(   11); //rho, omega, phi -> e+ e-
    partDaughterPdg[26].push_back(  -11);

    partDaughterPdg[27].push_back(   13); //rho, omega, phi -> mu+ mu-
    partDaughterPdg[27].push_back(  -13);

    partDaughterPdg[28].push_back(   11); //gamma -> e+ e-
    partDaughterPdg[28].push_back(  -11);

    partDaughterPdg[29].push_back(   11); //JPsi -> e+ e-
    partDaughterPdg[29].push_back(  -11);

    partDaughterPdg[30].push_back(   13); //JPsi -> mu+ mu-
    partDaughterPdg[30].push_back(  -13);

    partDaughterPdg[31].push_back(  211); //D0 -> pi+ K-
    partDaughterPdg[31].push_back( -321);

    partDaughterPdg[32].push_back( -211); //D0_bar -> K+ pi-
    partDaughterPdg[32].push_back(  321);

    partDaughterPdg[33].push_back(  211); //D0 -> pi+ pi+ pi- K-
    partDaughterPdg[33].push_back(  211);
    partDaughterPdg[33].push_back( -211);
    partDaughterPdg[33].push_back( -321);

    partDaughterPdg[34].push_back( -211); //D0_bar -> pi- pi- pi+ K+
    partDaughterPdg[34].push_back( -211);
    partDaughterPdg[34].push_back(  211);
    partDaughterPdg[34].push_back(  321);

    partDaughterPdg[35].push_back( -321); //D+ -> K- pi+ pi+
    partDaughterPdg[35].push_back(  211);
    partDaughterPdg[35].push_back(  211);

    partDaughterPdg[36].push_back(  321); //D- -> K+ pi- pi-
    partDaughterPdg[36].push_back( -211);
    partDaughterPdg[36].push_back( -211);

    partDaughterPdg[37].push_back( -321); //Ds+ -> K- K+ pi+
    partDaughterPdg[37].push_back(  321);
    partDaughterPdg[37].push_back(  211);

    partDaughterPdg[38].push_back(  321); //Ds- -> K+ K- pi-
    partDaughterPdg[38].push_back( -321);
    partDaughterPdg[38].push_back( -211);

    partDaughterPdg[39].push_back(  211); //Lambdac -> pi+ K- p
    partDaughterPdg[39].push_back( -321);
    partDaughterPdg[39].push_back( 2212);

    partDaughterPdg[40].push_back( -211); //Lambdac_bar -> pi- K+ p-
    partDaughterPdg[40].push_back(  321);
    partDaughterPdg[40].push_back(-2212);

    partDaughterPdg[41].push_back(  411); //D*0 -> D+ pi-
    partDaughterPdg[41].push_back( -211);

    partDaughterPdg[42].push_back( -411); //D*0_bar -> D- pi+
    partDaughterPdg[42].push_back(  211);

    partDaughterPdg[43].push_back(  421); //D*+ -> D0 pi+
    partDaughterPdg[43].push_back(  211);

    partDaughterPdg[44].push_back( -421); //D*- -> D0_bar pi-
    partDaughterPdg[44].push_back( -211);

    partDaughterPdg[45].push_back(  421); //D*+ -> D04 pi+
    partDaughterPdg[45].push_back(  211);

    partDaughterPdg[46].push_back( -421); //D*- -> D04_bar pi-
    partDaughterPdg[46].push_back( -211);

    for(int iP=0; iP<nParticles; iP++)
    {
      partPDG[iP] = mPartPDG[iP];
      partName[iP] = mPartName[iP];
      partTitle[iP] = mPartTitle[iP];
      partMHistoMin[iP] = mPartMHistoMin[iP];
      partMHistoMax[iP] = mPartMHistoMax[iP];
    }

    for(int iP=0; iP<nParticles; iP++)
    {
      AddCounter(Form("%s",partName[iP].Data()), Form("%-*s",14,partTitle[iP].Data()));
      AddCounter(Form("%s_prim",partName[iP].Data()), Form("%s Prim",partTitle[iP].Data()));
      AddCounter(Form("%s_sec",partName[iP].Data()), Form("%s Sec ",partTitle[iP].Data()));
    }

    for(int iP=0; iP<nParticles; iP++)
      fPdgToIndex[mPartPDG[iP]] = iP;
  }

  virtual ~CbmKFPartEfficiencies(){};

  int GetParticleIndex(int pdg)
  {
    std::map<int, int>::iterator it;
    it=fPdgToIndex.find(pdg);
    if(it != fPdgToIndex.end()) return it->second;
    else return -1;
  }

  virtual void AddCounter(TString shortname, TString name){
    indices[shortname] = names.size();
    names.push_back(name);

    ratio_reco.AddCounter();
    mc.AddCounter();
    reco.AddCounter();

    ratio_ghost.AddCounter();
    ratio_bg.AddCounter();
    ratio_clone.AddCounter();
    ghost.AddCounter();
    bg.AddCounter();
    clone.AddCounter();
  };

  CbmKFPartEfficiencies& operator+=(CbmKFPartEfficiencies& a){
    mc += a.mc; reco += a.reco;
    ghost += a.ghost; bg += a.bg; clone += a.clone;
    return *this;
  };
  
  void CalcEff(){
    ratio_reco = reco/mc;

    TL1TracksCatCounters<int> allReco = reco + ghost + bg;
    ratio_ghost = ghost/allReco;
    ratio_bg  = bg/allReco;
    ratio_clone  = clone/allReco;
  };
  

  void Inc(bool isReco, int nClones, TString name)
  {
    const int index = indices[name];
    
    mc.counters[index]++;
    if (isReco) reco.counters[index]++;
    if(nClones > 0)
      clone.counters[index] += nClones;
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
         <<" / "<< "N Clone "
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
             << "  / " << setw(7) << clone.counters[iC]
             << "  | " << setw(6) << mc.counters[iC]  << std::endl;
    }
  };

  friend std::fstream & operator<<(std::fstream &strm, CbmKFPartEfficiencies &a) {

    strm << a.ratio_reco;
    strm << a.mc;
    strm << a.reco;
    strm << a.ratio_ghost;
    strm << a.ratio_bg;
    strm << a.ratio_clone;
    strm << a.ghost;
    strm << a.bg;
    strm << a.clone;

    return strm;
  }

  friend std::fstream & operator>>(std::fstream &strm, CbmKFPartEfficiencies &a){

    strm >> a.ratio_reco;
    strm >> a.mc;
    strm >> a.reco;
    strm >> a.ratio_ghost;
    strm >> a.ratio_bg;
    strm >> a.ratio_clone;
    strm >> a.ghost;
    strm >> a.bg;
    strm >> a.clone;

    return strm;
  }

  void AddFromFile(TString fileName)
  {
    std::fstream file(fileName.Data(),fstream::in);
    file >> *this;
  }

  static const int nParticles = 47;
  int partPDG[nParticles];
  TString partName[nParticles];
  TString partTitle[nParticles];
  vector<vector<int> > partDaughterPdg;
  float partMHistoMin[nParticles];
  float partMHistoMax[nParticles];

  ClassDef(CbmKFPartEfficiencies,1);

 private:
  vector<TString> names; // names counters indexed by index of counter
  map<TString, int> indices; // indices of counters indexed by a counter shortname

  map<int, int> fPdgToIndex;

  TL1TracksCatCounters<double> ratio_reco;

  TL1TracksCatCounters<int> mc;
  TL1TracksCatCounters<int> reco;

  TL1TracksCatCounters<double> ratio_ghost;
  TL1TracksCatCounters<double> ratio_bg;
  TL1TracksCatCounters<double> ratio_clone;

  TL1TracksCatCounters<int> ghost;
  TL1TracksCatCounters<int> bg; // background
  TL1TracksCatCounters<int> clone; // background
};

#endif
