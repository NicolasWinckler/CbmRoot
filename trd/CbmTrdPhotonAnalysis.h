// -------------------------------------------------------------------------
// -----                         CbmTrdPhotonAnalysis header file      -----
// -----                        Created 13/02/12  by C.Bergmann        -----
// -------------------------------------------------------------------------



#ifndef CBMTRDPHOTONANALYSIS_H
#define CBMTRDPHOTONANALYSIS_H

#include "FairTask.h"
#include "TH2I.h"
#include "TH3I.h"
#include <vector>
#include "CbmMCTrack.h"

class TClonesArray;

class CbmTrdDigiPar;
class CbmTrdDigi;
class CbmTrdModule;
class CbmMCTrack;
class TLegend;
class TProfile;
class TH1;
class TH1I;
//class TH1F;
//class TH1D;
class TH2;
class TH2I;
//class TH2F;
//class TH2D;
//class vector;

typedef struct MCParticle
{
  Int_t PID; // PDG Code
  Int_t motherId; // TConeArray ID of the mother 
  //Int_t motherPid; // PDG Code
  std::vector<Int_t> daughterIds; // TConeArray IDs of all daughters
  //std::vector<Int_t> daughterPids; // PDG Code
  Double_t Px;
  Double_t Py;
  Double_t Pz;
MCParticle() : PID(0), motherId(0), /*motherPid(0),*/ daughterIds(), /*daughterPids(),*/ Px(0), Py(0), Pz(0) {};
} MCParticle;

class CbmTrdPhotonAnalysis : public FairTask {
  
 public:
  CbmTrdPhotonAnalysis();
  CbmTrdPhotonAnalysis(const char *name, const char *title="CBM Task", Int_t iVerbose=1);
  virtual ~CbmTrdPhotonAnalysis();


  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();

  void InitHistos();
  void SaveHistosToFile();
  void NormalizeHistos(TH1* h);

  Bool_t VertexInMagnet(CbmMCTrack* track); 
  Bool_t VertexInTarget(CbmMCTrack* track);
  Bool_t CloseByVertex(CbmMCTrack* trackA, CbmMCTrack* trackB, Double_t minimumDistance /*[cm]*/);

  Double_t CalcVertexDistance(CbmMCTrack* trackA, CbmMCTrack* trackB);

  Bool_t PairFromGamma(Int_t firstId, Int_t secondId);
  Bool_t PairFromPi0(Int_t firstId, Int_t secondId);

  Double_t CalcInvariantMass(CbmMCTrack* trackA, CbmMCTrack* trackB);
  Double_t CalcOpeningAngle(CbmMCTrack* trackA, CbmMCTrack* trackB);
  Double_t CalcPt(CbmMCTrack* trackA, CbmMCTrack* trackB);
  Double_t CalcP(CbmMCTrack* trackA, CbmMCTrack* trackB);

  void StatisticHistos();

  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Finish (called after each event) **/
  virtual void FinishEvent();
 
  /** Finish task (called after all event) **/
  virtual void FinishTask();

  void Register();

  Int_t PdgToGeant(Int_t PdgCode);

  //void NiceHisto(TProfile *h, Int_t color, Int_t mStyle, Int_t mSize);
  void NiceHisto1(TH1 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle);
  void NiceHisto2(TH2 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle, TString zTitle);
  void NiceHisto3(TH3 *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle, TString zTitle);
  void NiceProfile(TProfile *h, Int_t color, Int_t mStyle, Int_t mSize, TString xTitle, TString yTitle);
  void NiceLegend(TLegend *l);
  void Statusbar(Int_t i, Int_t n);
 private:
  CbmTrdPhotonAnalysis(const CbmTrdPhotonAnalysis&);
  CbmTrdPhotonAnalysis& operator=(const CbmTrdPhotonAnalysis&);

  TClonesArray*     fMCTracks; 
  TClonesArray*     fTrdPoints;      /**  array of Trd MC points **/
  TClonesArray*     fTrdDigis;       /**  array of CbmTrdDigi    **/
  TClonesArray*     fTrdClusters;    /**  array of CbmTrdCluster **/
  TClonesArray*     fTrdHits;        /**  array of CbmTrdHit     **/
  TClonesArray*     fTrdTracks;      /**  CbmTrdTrack array      **/
  TClonesArray*     fGlobalTracks;   /**  CbmGlobalTrack array   **/
  TClonesArray*     fPrimaryVertex;  /**  PrimaryVertex  array   **/

  CbmTrdDigiPar  *fDigiPar;
  CbmTrdModule   *fModuleInfo;

  //Histos

  std::map<TString, TH1*> fHistoMap;
  std::map<TString, TH1*>::iterator fHistoMapIt;
  
  // Birth Vertex
  TH3I*  fMotherDaughterZBirth;
  TH3I*  fBirthGamma;
  TH3I*  fBirthPi0;
  TH3I*  fBirthPair;
  
  ClassDef(CbmTrdPhotonAnalysis,2)
    };


#endif //CBMTRDPHOTONANALYSIS_H
