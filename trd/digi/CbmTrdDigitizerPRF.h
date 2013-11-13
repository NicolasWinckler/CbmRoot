#ifndef CBMTRDDIGITIZERPRF_H
#define CBMTRDDIGITIZERPRF_H

#include "FairTask.h"

#include <map>
using std::map;
using std::pair;

#include <list>
#include <vector>
#include "CbmTrdDigi.h"

class CbmTrdPoint;

class CbmTrdDigiPar;
class CbmTrdModule;
class CbmTrdRadiator;
class CbmTrdGeoHandler;
class CbmMatch;

class TClonesArray;
class TH1F;
class TH2F;
class TProfile;

class CbmTrdDigitizerPRF : public FairTask {

 public:

  /** Default constructor **/
  CbmTrdDigitizerPRF();

  /** Standard constructor **/
  CbmTrdDigitizerPRF(const char *name, const char *title="CBM Task",
		     CbmTrdRadiator *radiator=NULL);
  /*
    oneClusterPerPoint: produces just one cluster per MC-track if true -> less ghost hits
    produces n cluster per track depending on the path length within the detector volume -> leads to at least one hit per crossed pad row -> high number of ghost hits
  */

  /** Destructor **/
  virtual ~CbmTrdDigitizerPRF();

  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();

  /** Executed task **/
  virtual void Exec(Option_t * option);

  void FinishEvent();

 private:

  CbmTrdDigitizerPRF& operator=(const CbmTrdDigitizerPRF&);
  CbmTrdDigitizerPRF(const CbmTrdDigitizerPRF&);

  Double_t CalcPRF(Double_t x, Double_t W, Double_t h);

  void ScanPadPlane(const Double_t* local_point, Double_t clusterELoss);

  void SplitTrackPath(const CbmTrdPoint* point, Double_t ELoss);

  void AddDigi(Int_t pointId, Int_t address, Double_t charge, Double_t time);

  Bool_t fDebug;

  Double_t fTime;
  Int_t fnRow;
  Int_t fnCol;
  Int_t fnSec;
  Int_t fLayerId;
  Int_t fModuleId;
  Int_t fModuleAddress;
  //Int_t fnSecRow[3];

  Int_t   fModuleType;
  Int_t   fModuleCopy;
  //Int_t   fModuleID;//Unique number for detector module
  Int_t   fMCPointId;// index to MCPoint
  //Double_t fELossTR;

  TClonesArray *fTrdPoints; //! Trd MC points
  TClonesArray *fDigis; //! TRD digis
  TClonesArray *fDigiMatchs; //! Corresponding MCPoints to TRD digis
  TClonesArray *fMCStacks;  //! MC Track information

  CbmTrdDigi* digi;
  //CbmTrdDigiMatch* digiMatch;

  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  CbmTrdRadiator *fRadiators;  //!

  CbmTrdGeoHandler* fGeoHandler; //!

  // Temporary storage for digis.
  // map<address, pair<CbmTrdDigi*, CbmTrdDigiMatch*>
  std::map<Int_t, pair<CbmTrdDigi*, CbmMatch*> > fDigiMap;
  std::map<Int_t, pair<CbmTrdDigi*, CbmMatch*> >::iterator fDigiMapIt;
  ClassDef(CbmTrdDigitizerPRF,2)

    };
#endif // CBMTRDDIGITIZER_PRF_H
