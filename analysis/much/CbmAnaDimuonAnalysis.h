/** CbmAnaDimuonAnalysis.h
 *@author E.Kryshen <e.kryshen@gsi.de>
 *@since 2009-09-11
 **
 ** Jpsi polarization studies
 **/

#ifndef CBMANADIMUONANALYSIS_H_
#define CBMANADIMUONANALYSIS_H_ 1

#include "FairTask.h"
#include "TString.h"
#include <map>

class CbmAnaDimuonAnalysis;
class TClonesArray;
class CbmTrackMatch;
class TH1D;
class TH2D;
class TH3D;
class TLorentzVector;
class CbmMuchTrack;
class CbmMuchGeoScheme;
using std::map;

class CbmAnaDimuonAnalysis : public FairTask{
public:
  /** Default constructor **/
  CbmAnaDimuonAnalysis();

  /** Standard constructor
  *@param name   Name of class
  *@param title  Task title
  **/
  CbmAnaDimuonAnalysis(const char* name, TString digiFileName, TString histoFileName);

  /** Destructor **/
  virtual ~CbmAnaDimuonAnalysis();

  /** Initialisation **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t* opt);

  /** Finish at the end **/
  virtual void Finish();

  /** SetParContainers **/
  virtual void SetParContainers();

  // Set STS acceptance criteria as a number of STS points
  void SetStsPointsAccQuota(Int_t nPoints)  { fStsPointsAccQuota = nPoints; }
  void SetMuchPointsAccQuota(Int_t nPoints) { fMuchPointsAccQuota = nPoints; }
  void SetStsTrueHitQuota(Double_t quota)   { fStsTrueHitQuota = quota; }
  void SetMuchTrueHitQuota(Double_t quota)  { fMuchTrueHitQuota = quota; }

  Bool_t Trigger(CbmMuchTrack* track);
private:
  Int_t         fEvent;             //!
  TClonesArray* fMCTracks;          //!
  TClonesArray* fStsTracks;         //!
  TClonesArray* fStsPoints;         //!
  TClonesArray* fMuchPoints;        //!
  TClonesArray* fMuchHits;          //!
  TClonesArray* fMuchTracks;        //!
  TClonesArray* fMuchTrackMatches;  //!
  TClonesArray* fStsTrackMatches;   //!
  TClonesArray* fMuPlusCandidates;  //!
  TClonesArray* fMuMinusCandidates; //!
  TClonesArray* fGlobalTracks;      //!

  Int_t    fStsPointsAccQuota;
  Double_t fStsTrueHitQuota;
  Int_t    fMuchPointsAccQuota;
  Double_t fMuchTrueHitQuota;
  TString  fHistoFileName;
  TString  fDigiFileName;
  
  
  map<Int_t,Int_t> mapRecSts;       //!
  map<Int_t,Int_t> mapRecMuch;      //!

  map<Int_t,Int_t> mapAllMuPlus;    //!
  map<Int_t,Int_t> mapAccMuPlus;    //!
  map<Int_t,Int_t> mapRecMuPlus;    //!
  map<Int_t,Int_t> mapAllMuMinus;   //!
  map<Int_t,Int_t> mapAccMuMinus;   //!
  map<Int_t,Int_t> mapRecMuMinus;   //!

  CbmMuchGeoScheme* fGeoScheme;     //!
  
  
  ClassDef(CbmAnaDimuonAnalysis,1);
};

#endif
