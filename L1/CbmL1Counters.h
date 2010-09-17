#ifndef CbmL1Counters_H
#define CbmL1Counters_H

#include <iostream>
#include <vector>
#include <map>
using std::ios;
using std::vector;
using std::map;

  /// counters used for efficiency calculation
template <typename T>
struct TL1TracksCatCounters // counters for different tracks categories
{
  int NCounters;
  
  vector<T> counters;

  TL1TracksCatCounters():NCounters(0),counters(){ counters.clear(); };
  TL1TracksCatCounters(int nCounters):NCounters(nCounters),counters(){ counters.resize( NCounters, T(0)); };

  void AddCounter(){ NCounters++; counters.push_back(T(0)); };
  void AddCounters(int nCounters){ NCounters += nCounters; counters.resize( NCounters, T(0)); };
  
  TL1TracksCatCounters& operator+=(TL1TracksCatCounters& a){
    if (NCounters != a.NCounters){
      cout << " TL1TracksCatCounters: Error. Addition of counters of different sizes: " << NCounters << " " << a.NCounters << endl;
    }
    else{
      for (int iC = 0; iC < NCounters; iC++){
        counters[iC] += a.counters[iC];
      }
    }
    return *this;
  };

  template <typename T2>
  TL1TracksCatCounters<double> operator/(TL1TracksCatCounters<T2>& a){
    TL1TracksCatCounters<double> b(NCounters);
    if (NCounters != a.NCounters){
      cout << " TL1TracksCatCounters: Error. Addition of counters of different sizes: " << NCounters << " " << a.NCounters << endl;
    }
    else{
      for (int iC = 0; iC < NCounters; iC++){
        b.counters[iC] = Div(counters[iC],a.counters[iC]);
      }
    }
    return b;
  };

  template <typename T2>
  TL1TracksCatCounters<T2> operator/(double a){
    TL1TracksCatCounters<T2> b(NCounters);
    for (int iC = 0; iC < NCounters; iC++){
      b.counters[iC] = (T2)Div(counters[iC],a);
    }
    return b;
  };

  private:
    double Div(double a, double b){return (b > 0) ? a/b : -1.;};
};

struct TL1Efficiencies
{
  TL1Efficiencies():names(),indices(),ratio_reco(),ratio_ghosts(0),ratio_clones(0),mc(),reco(),ghosts(0),clones(0){
    // you should add counter with shortname="total" !!
  };

  virtual ~TL1Efficiencies(){};

  virtual void AddCounter(TString shortname, TString name);
  
  TL1Efficiencies& operator+=(TL1Efficiencies& a);
  void CalcEff();
  void Inc(bool isReco, TString name); // increment counters according to parameters

  void PrintEff();

  
  vector<TString> names; // names counters indexed by index of counter
  map<TString, int> indices; // indices of counters indexed by a counter shortname
  
  TL1TracksCatCounters<double> ratio_reco;
  double ratio_ghosts;
  double ratio_clones;
  
  TL1TracksCatCounters<int> mc;
  TL1TracksCatCounters<int> reco;
  int ghosts;
  int clones;
};

inline void TL1Efficiencies::AddCounter(TString shortname, TString name)
{
  indices[shortname] = names.size();
  names.push_back(name);
    
  ratio_reco.AddCounter();
  mc.AddCounter();
  reco.AddCounter();
}

inline void TL1Efficiencies::CalcEff()
{
  ratio_reco = reco/mc;
  if (mc.counters[0] > 0){
    ratio_clones = clones/double(mc.counters[indices["total"]]);
  }
  else{
    ratio_clones = -1;
  }
  if (reco.counters[0] > 0){
    ratio_ghosts = ghosts/double(reco.counters[indices["total"]]);
  }
  else{
    ratio_ghosts = -1;
  }
};

inline TL1Efficiencies& TL1Efficiencies::operator+=(TL1Efficiencies& a)
{
  mc += a.mc; reco += a.reco;
  ghosts += a.ghosts; clones += a.clones;
  return *this;
};

inline void TL1Efficiencies::Inc(bool isReco, TString name)
{
  const int index = indices[name];
    
  mc.counters[index]++;
  if (isReco) reco.counters[index]++;
};

inline void TL1Efficiencies::PrintEff(){
  cout.setf(ios::fixed);
  cout.setf(ios::showpoint);
  cout.precision(3);
  cout << "Track category         : " << " Eff "        <<" | "<< "All MC"  << endl;

  int NCounters = mc.NCounters;
  for (int iC = 0; iC < NCounters; iC++){
    cout << names[iC]  << "   : "
        << ratio_reco.counters[iC]
        << "  | " << mc.counters[iC]  << endl;
  }

  cout << "Clone     probability  : " << ratio_clones <<" | "<< clones << endl;
  cout << "Ghost     probability  : " << ratio_ghosts <<" | "<< ghosts << endl;
};

#endif
