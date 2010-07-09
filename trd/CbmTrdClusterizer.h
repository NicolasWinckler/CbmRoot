#ifndef CBMTRDCLUSTERIZER_H
#define CBMTRDCLUSTERIZER_H

#include "CbmTrdDetectorId.h"

#include "FairTask.h"

#include <map>
#include <list>
#include "CbmTrdDigi.h"

class CbmTrdDigiPar;
//class CbmTrdDigi;
class CbmTrdModule;
class CbmTrdRadiator;

class TClonesArray;
class TH1F;
class TH2F;
class TProfile;

class CbmTrdClusterizer : public FairTask {

 public:

  /** Default constructor **/
  CbmTrdClusterizer();

  /** Standard constructor **/
  CbmTrdClusterizer(const char *name, const char *title="CBM Task",
		    CbmTrdRadiator *radiator=NULL);

  /** Destructor **/
  virtual ~CbmTrdClusterizer();

  /** Initialisation **/
  virtual InitStatus ReInit();
  virtual InitStatus Init();
  virtual void SetParContainers();

  /** Executed task **/
  virtual void Exec(Option_t * option);

  /** Finish (called after each event) **/
  virtual void FinishEvent();
 
  /** Finish task (called after all event) **/
  virtual void FinishTask(){;}

  /*
    void AddHit(TVector3 &posHit, TVector3 &posHitErr,
    Int_t TrackID, Int_t PlaneID, Int_t ref, Double_t ELoss,
    Double_t ELossTR, Double_t ELossdEdX);
  */

  void Register();

 private:

  CbmTrdClusterizer& operator=(const CbmTrdClusterizer&);
  CbmTrdClusterizer(const CbmTrdClusterizer&);

  void GetModuleInformationFromDigiPar(Bool_t Sector, Int_t VolumeID);
  void GetModuleInformation();

  void AddDigi(const Int_t pointID, Int_t iCol, Int_t iRow, Float_t iCharge);

  void CalculatePixel(Bool_t Sector);

  void CalcPixelCoordinate();

  float GetFloatPositionY(Double_t tempPosY);

  void SplitPathSlices(const Int_t pointID, Bool_t Sector, Bool_t Histo, Bool_t TEST, TH2F* DeltaSlice2, TH2F* In, TH2F* Out, TH2F* Clusterposition, Double_t* PadChargeModule, Int_t nCol, Int_t nRow, Int_t j, Double_t* padW, Double_t* padH,  TH2F* Reco, TH2F* recoTRD1, TH2F* recoTRD2, TProfile* deltarecoTRD1, TH2F*  deltarecoTRD2, TProfile* deltarecoPad, TH1F* Xreco, TH1F* PR, TH2F* PRF, TProfile* PRF2, TH1F* TestIntegration);

  void GetIntegrationArea(Bool_t Histo, TH1F* PadX ,TH1F* PadY);

  void ChargeConservation(Int_t Row_slice, Int_t Col_slice, Int_t nCol, Int_t nRow);

  void ClusterMapping(Int_t nCol, Int_t nRow, Int_t Col_slice, Int_t Row_slice, Double_t* PadChargeModule);

  void CalcMathieson(Bool_t TEST, Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H);

  void CalcCenterOfGravity();

  void CalcPRF(Bool_t TEST, Bool_t Histo, Int_t nRow, Int_t nCol, Double_t* PadChargeModule, TH2F* Reco, TH2F* recoTRD1, TH2F* recoTRD2, TProfile* deltarecoTRD1, TH2F*  deltarecoTRD2, TProfile* deltarecoPad, TH1F* Xreco, TH1F* PR, TH2F* PRF, TProfile* PRF2);

  void TransformLL2C(Double_t* LLCoordinate, Double_t* CCoordinate, Double_t* StrucDim);
 
  void TransformC2LL(Double_t* CCoordinate, Double_t* LLCoordinate, Double_t* StrucDim);

  double GetPadHeight(Int_t iSector);

  int GetRow(Double_t tempPosY);

  int GetCol(Double_t tempPosX);

  int GetSector(Double_t tempPosY);

  void GetPadSizeMatrix(Double_t* H, Double_t* W, Double_t* padH, Double_t* padW, Int_t Row_slice, Int_t Col_slice, Int_t nRow, Int_t nCol);

  int GetPadMax(Int_t iRow, Int_t nCol, Double_t* PadChargeModule);

  //void TransformMod2Pad(Double_t* MCoordinate, Double_t* PCoordinate, Double_t* StrucDim);
 
  Int_t Digicounter;

  Int_t   fStation;
  Int_t   fLayer;
  Int_t   fModuleType;
  Int_t   fModuleCopy;
  Int_t   fCol_mean,fCol_in,fCol_out; //Calculated pixel column were the hit is in
  Int_t   fRow_mean,fRow_in,fRow_out; //Calculated pixel row were the hit is in
  Int_t   fModuleID;//Unique number for detector module
  Int_t   fMCindex;// index to MCPoint
  // --------------->[cm]<---------------
  Double_t local_meanLL[3];
  Double_t local_meanC[3];
  Double_t global_meanLL[3];//[cm]
  Double_t global_meanC[3];
  Double_t local_inLL[3];
  Double_t local_inC[3];
  Double_t global_inLL[3];//[cm]
  Double_t global_inC[3];

  Double_t local_outLL[3];
  Double_t local_outC[3];
  Double_t global_outLL[3];//[cm]
  Double_t global_outC[3];
  // --------------->[mm]<---------------

  Float_t fx_in, fx_out, fy_in, fy_out, fz_in, fz_out, fx_mean, fy_mean, fz_mean;
  Int_t fSector;
  //Int_t tempNosectors =  (fModuleInfo->GetNoSectors);
  static const Int_t accuracy = 1;// '1/accuracy' integration step width [mm]
  static const Int_t fPadNrX = 13;//7; // has to be odd
  static const Int_t fPadNrY = 3;//5; // has to be odd
  static const Int_t fNoSectors = 3;
  //Int_t fNoSectors;
  /*
    static const Bool_t Histo = true;
    //static const Bool_t Histo = false;
    static const Bool_t TEST = true;
    //static const Bool_t TEST = false;
    //static const Bool_t Sector = true;
    static const Bool_t Sector = false;
  */
  
  Double_t padsize[3];
  Double_t modulesize[3];
  Double_t sectorsize[fNoSectors]; // 3 sectors per module
  Int_t sectorrows[fNoSectors];

  Float_t fELoss;//energy loss from MCPoint 
  Float_t fELossdEdX;
  Float_t fELossTR;
  Float_t fPosXLL, fPosYLL;//Hit position in chamber coordinates origin 'L'ower 'L'eft corner
 
  Float_t fPadPosxLL, fPadPosyLL;//Hit position in pad coordinates (not rotated) origin 'L'ower 'L'eft corner
  Float_t fPadPosxC, fPadPosyC;//Hit position in pad coordinates (not rotated) origin pad 'C'enter
  Float_t fDeltax, fDeltay;



  Double_t fPadCharge[fPadNrY][fPadNrX]; //Charge on 3 adjacent pads calculated by using the Mathieson formula

  Float_t fPRFHitPositionLL, fPRFHitPositionC;

  Float_t fEfficiency; // Digi production efficiency (0-100%)
  //    Double_t fthreshold; //pixel threshold in electrons
  //    Double_t fnoise; //pixel noise in electrons

  TClonesArray *fTrdPoints; //! Trd MC points
  TClonesArray *fDigiCollection; //! TRD digis
  TClonesArray *fDigiMatchCollection; //! Corresponding MCPoints to TRD digis
  TClonesArray *fMCStacks;  //! MC Track information

  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  CbmTrdRadiator *fRadiators;  //!

  CbmTrdDetectorId fTrdId; //!
    
  /**  map to store digis for pair of x,y position in module **/
  // map<pair<ModuleID,pair<x,y>>, CbmTrdDigi*>
  std::map<std::pair< Int_t, std::pair< Int_t, Int_t > >, CbmTrdDigi* > fDigiMap; //!
  /**  iterator over map to store digis for pair of x,y position in module **/
  std::map<std::pair< Int_t, std::pair< Int_t, Int_t > >, CbmTrdDigi* >::iterator fDigiMapIt; //! iterator over array above

  ClassDef(CbmTrdClusterizer,1)

    };
#endif // CBMTRDCLUSTERIZER_H
