/** Matching between MC photons and reconstructed photons **/

#ifndef CBMECALMATCHING_H
#define CBMECALMATCHING_H

#include "CbmTask.h"
#include <list>
#include <map>

class TClonesArray;
class CbmEcalCell;
class CbmEcalRecParticle;
class TTree;

class CbmEcalMatching : public CbmTask
{
public:
  /** Only to comply with framework **/
  CbmEcalMatching();
  /** Standard constructor **/
  CbmEcalMatching(const char* name, const Int_t verbose);

  /** Task initialization **/
  virtual InitStatus Init();

  /** Exec for task **/
  virtual void Exec(Option_t* opt);
  
  /** Finish task **/
  virtual void Finish();

  /** Get/set threshold **/
  void SetThreshold(Double_t threshold=0.8) {fThreshold=threshold;}
  Double_t GetThreshold() const {return fThreshold;}
  /** Get/set photon threshold **/
  void SetPhotonThreshold(Double_t threshold=0.95) {fPhotonThr=threshold;}
  Double_t GetPhotonThreshold() const {return fPhotonThr;}

  /** virtual destructor **/
  ~CbmEcalMatching();

  /** Set output to tree **/
  void SetOutToTree(Bool_t totree=kFALSE) {fToTree=totree;}
private:
  /** Form a precluster **/
  void FormPreCluster(CbmEcalRecParticle* p);
  /** Form a map of energies for precluster **/
  void FormE();
  /** Add energy to track and all its mothers **/
  void AddTrackE(Int_t track, Double_t e);
  /** Match MCTrack and reconstructed particle **/
  void Match(CbmEcalRecParticle* p);
  /** An array of reconstructed photons **/
  TClonesArray* fReco;			//!
  /** An array of MC tracks **/
  TClonesArray* fMCTracks;		//!
  /** An array of calorimeter points **/
  TClonesArray* fPoints;		//!

  /** A threshold: if energy of photon is more than threshold*Ecluster
   ** than cluster is formed by that photon **/
  Double_t fThreshold;
  /** If energy of max photon > fPhotonThr*energy of any other particle will match with photon **/
  Double_t fPhotonThr;

  /** An event number **/
  Int_t fEv;

  /** A precluster **/
  std::list<CbmEcalCell*> fPreCluster;	//!
  /** An energy of precluster **/
  Double_t fClusterE;
  /** An energy associated with track number **/
  std::map<Int_t, Double_t> fE;		//!

  /** Current cell **/
  CbmEcalCell* fCell;			//!

  /** Shoul write information to tree? **/
  Bool_t fToTree;
  /** An output tree **/
  TTree* fTree;				//!
  /** Init tree **/
  void InitTree();
  /** Number of corresponding MC track **/
  Int_t fMCNum;
  /** Reconstructed information **/
  Double_t fPx;
  Double_t fPy;
  Double_t fPz;
  Double_t fRE;
  /** MC information **/
  Double_t fMCPx;
  Double_t fMCPy;
  Double_t fMCPz;
  Double_t fMCE;
  Int_t fPDG;
  /** Information about cluster **/
  Double_t fChi2;
  /** Number of maximums **/
  Int_t fNM;
  /** Number cells **/
  Int_t fNC;

  ClassDef(CbmEcalMatching, 1)
};

#endif
