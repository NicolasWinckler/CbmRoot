
#ifndef CBMTRDRAWTODIGISPADIC03_H
#define CBMTRDRAWTODIGISPADIC03_H


#include "FairTask.h"
#include "CbmTrdDigi.h"


#define NUM_SPADIC_CHA             8
#define SPADIC_TRACE_SIZE         45
#define SPADIC_ADC_MAX           255

class TPrincipal;
class CbmTrdDigiPar;
class CbmTrdModule;
class TClonesArray;
class CbmTrdGeoHandler;
class TH1I;
class TH1D;
class TH2I;
class TH2D;
class TFile;

class CbmTrdRawToDigiSpadic03 
: public FairTask {

 public:

  CbmTrdRawToDigiSpadic03();

  CbmTrdRawToDigiSpadic03(const char *name, const char *title="CBM Task");

  virtual ~CbmTrdRawToDigiSpadic03();

  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();

  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Finish (called after each event) **/
  virtual void FinishEvent();
 
  /** Finish task (called after all event) **/
  virtual void FinishTask(){;}

  void Register();



  void SetNBaselineTB(Int_t NBaselineTB);

 
  void SetLowerCorThreshold (std::map<Int_t, Double_t> lowercorthreshold);
  void SetMinimumAmplitudeThreshold (std::map<Int_t, Double_t> minimumamplitudethreshold);
  void SetSusIds (std::vector<Int_t> susIds);

  void SetInputFile(const TString fileName);

 private:
  void BaselineCompensation(TH1D* inPulse[NUM_SPADIC_CHA], TH1D* outPulse[NUM_SPADIC_CHA]);
  Int_t CancelNoise_Cova(TH1D* inPulse[NUM_SPADIC_CHA], TH1D* outPulse[NUM_SPADIC_CHA]);
  Int_t GetPadMax(TH1D* inPulse[NUM_SPADIC_CHA]);

  TFile *fFile;
  Int_t fSusid;
  std::vector<Int_t> fSusIds;
  Int_t fNBaselineTB;
  std::map<Int_t, Double_t> fLowercorthreshold;
  std::map<Int_t, Double_t> fMinimumAmplitudeThreshold;

  TPrincipal* fPrincipal;
  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  TClonesArray *fDigis; //! TRD digis
  CbmTrdGeoHandler* fGeoHandler; //!

  CbmTrdRawToDigiSpadic03& operator=(const CbmTrdRawToDigiSpadic03&);
  CbmTrdRawToDigiSpadic03(const CbmTrdRawToDigiSpadic03&);


  TH2I*baselineDistribution;
  TH1I*covaMatixValue;
  TH2I*covaMatixValueMaxAmplitude;
  TH2I*covaMatixValueHitTime;
  TH2I*maxAmplitudeHitTime;
  TH1I*noiseDistribution;
  TH1I*clusterSize;
  TH1I*signalChDistance;
  TH2I*averageNoise_2D;
  TH2I*averageSignal_2D;
  TH2I*covaMatixValueClusterSize;

  ClassDef(CbmTrdRawToDigiSpadic03,1)

    };

#endif//CBMTRDRAWTODIGISPADIC03_H
