// -------------------------------------------------------------------------
// -----                         CbmTrdPhotonAnalysis header file      -----
// -----                        Created 13/02/12  by C.Bergmann        -----
// -------------------------------------------------------------------------



#ifndef CBMTRDPHOTONANALYSIS_H
#define CBMTRDPHOTONANALYSIS_H

#include "CbmTrdDetectorId.h"
#include "FairTask.h"
#include "TH2I.h"
#include "TH3I.h"
#include <vector>

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
class vector;

typedef struct MCParticle
{
  Int_t PID;
  Int_t motherId;
  std::vector<Int_t> daughterIds;
MCParticle() : PID(0), motherId(0), daughterIds() {};
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

  Bool_t VertexInMagnet(CbmMCTrack* track); 
  Bool_t VertexInTarget(CbmMCTrack* track);

  Bool_t PairFromGamma(Int_t firstId, Int_t secondId);
  Bool_t PairFromPi0(Int_t firstId, Int_t secondId);

  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Finish (called after each event) **/
  virtual void FinishEvent();
 
  /** Finish task (called after all event) **/
  virtual void FinishTask();

  void Register();

  Int_t PdgToGeant(Int_t PdgCode);

  //void NiceHisto(TProfile *h, Int_t color, Int_t mStyle, Int_t mSize);
  void NiceHisto1(TH1 *h, Int_t color, Int_t mStyle, Int_t mSize);
  void NiceHisto2(TH2 *h, Int_t color, Int_t mStyle, Int_t mSize);
  void NiceProfile(TProfile *h, Int_t color, Int_t mStyle, Int_t mSize);
  void NiceLegend(TLegend *l);

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
  CbmTrdDetectorId fTrdId; 


  //Histos
  TH3I*  fMotherDaughterZBirth;
  TH3I*  fBirthGamma;
  TH3I*  fBirthPi0;
  TH3I*  fBirthPair;

  TH2I*  fZBirthAll;
  TH2I*  fMotherDaughter;

  TH2I* fNoDaughters;

  TH1I*  fZBirthEPfromGamma;
  TH1I*  fMCPid;
  TH1I*  fGTPid;
  TH1I*  fePlusMinusMother;
  TH1I*  fePlusAndMinusMother;
  TH1I*  fgammaMother;
  TH1I*  fgammaAndGammaMother;
  TH1I*  fDalizMother;
  TH1I*  fgammaDaughter;
  TH1I*  fZBirth[4];
  TH1I*  fPairHistory;

  TH1I* fPairAllVertex[3];
  TH1I* fPairGammaVertex[3];
  TH1I* fPairPi0Vertex[3];

  std::map<Int_t, MCParticle*> fMCParticleMap;
  std::map<Int_t, MCParticle*>::iterator it;

  std::vector<Int_t> fElectronIds;
  std::vector<Int_t> fPositronIds;
  std::vector<Int_t> fGammaIds;
  std::vector<Int_t> fPi0Ids;

  ClassDef(CbmTrdPhotonAnalysis,1)
    };


#endif //CBMTRDPHOTONANALYSIS_H
