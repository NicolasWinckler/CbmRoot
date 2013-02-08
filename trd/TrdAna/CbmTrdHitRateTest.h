#ifndef CBMTRDHITRATETEST_H
#define CBMTRDHITRATETEST_H

#include "FairTask.h"

#include <map>
#include <list>
#include <vector>

class CbmTrdDigiPar;
class CbmTrdDigi;
class CbmTrdModule;
class CbmTrdRadiator;
class CbmTrdGeoHandler;

class TClonesArray;
class TCanvas;
class TH1F;
class TH2F;

typedef struct HitRateGeoPara
{
  Int_t moduleId;
  Int_t layerId;
  Int_t stationId;
  Double_t mPos[3];
  Double_t mSize[3];// [dimensions]
  Double_t sSize[3][3];
  Double_t pSize[3][3]; // [sectors][dimensions]
  Int_t nCol;
  Int_t nRow;
  Int_t sCol[3];
  Int_t sRow[3];
  Double_t vOrigin[3];
  Double_t vX[3];
  Double_t vY[3];
  Double_t vN[3];
  Double_t lambda;
  Double_t cosX;
  Double_t cosY;
  Int_t stepDirection[3];
} HitRateGeoPara;

class CbmTrdHitRateTest : public FairTask {

 public:

  /** Default constructor **/
  CbmTrdHitRateTest();

  /** Standard constructor **/
  CbmTrdHitRateTest(const char *name, const char *title="CBM Task",
		    CbmTrdRadiator *radiator=NULL);

  /** Destructor **/
  virtual ~CbmTrdHitRateTest();

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

  void HistoInit(TCanvas*& c1, TCanvas*& c2, TH2F*& Layer,TH1F*& HitPad, Char_t* Canfile1, Char_t* Canfile2, Double_t ZRangeL, Double_t ZRangeU, Double_t mm2bin);

  void GetModuleInformationFromDigiPar(HitRateGeoPara *GeoPara, Bool_t Fast, Bool_t Lines, Int_t VolumeID, TH2F* Layer, TCanvas* c1, Char_t* Canfile1, TH1F* HitPad, TCanvas* c2, TH2F* Topview[3], TCanvas* c0, Double_t mm2bin);

  void GetModuleInformationSL( Int_t VolumeID);

  void FillVector(Bool_t firstLayer, Int_t VolumeID,
		   std::vector<int>& L1S1, std::vector<int>& L2S1, std::vector<int>& L3S1, std::vector<int>& L4S1, 
		   std::vector<int>& L1S2, std::vector<int>& L2S2, std::vector<int>& L3S2, std::vector<int>& L4S2, 
		   std::vector<int>& L1S3, std::vector<int>& L2S3, std::vector<int>& L3S3, std::vector<int>& L4S3);

  void GetModuleInformation();

  void Histo(HitRateGeoPara *GeoPara, Bool_t Fast, Double_t* Mpos, Double_t* Msize,Double_t* Ssize, Double_t* Padsize, Int_t nRow, Int_t nCol, Int_t nSec, TH2F* Layer, TCanvas* c1, Char_t* Canfile1, TH1F* HitPad, TCanvas* c2,  TH2F* Topview[3], TCanvas* c0, Double_t mm2bin);
 
  float CalcHitRate(HitRateGeoPara *GeoPara, Float_t StartX, Float_t StopX, Int_t xSteps, Float_t StartY, Float_t StopY, Int_t ySteps, Double_t* Mpos, TH2F* Topview[3], TCanvas* c0);

  void DrawLines(Int_t Mid, Double_t* Mpos, Double_t* Msize,Double_t* Ssize, Double_t* Padsize, Int_t nRow, Int_t nCol, Int_t nSec, TH2F* Layer, TCanvas* c1, TH2F* Topview[3], TCanvas* c0);

  void DrawDigi();

 
  Int_t Digicounter;

  TFile *tFile;

  Bool_t fDraw;

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
  static const Int_t fPadNrX = 7;//7; // has to be odd
  static const Int_t fPadNrY = 5;//5; // has to be odd
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



  Float_t fPadCharge[fPadNrY][fPadNrX]; //Charge on 3 adjacent pads calculated by using the Mathieson formula

  Float_t fPRFHitPositionLL, fPRFHitPositionC;

  Float_t fEfficiency; // Digi production efficiency (0-100%)
  //    Double_t fthreshold; //pixel threshold in electrons
  //    Double_t fnoise; //pixel noise in electrons

  TClonesArray *fTrdPoints; //! Trd MC points
  TClonesArray *fDigiCollection; //! TRD digis
  TClonesArray *fDigiMatchCollection; //! Corresponding MCPoints to TRD digis
  TClonesArray *fMCStacks;  //! MC Track information

  CbmTrdDigiPar  *fDigiPar;
  CbmTrdModule   *fModuleInfo;
  CbmTrdRadiator *fRadiators; 

  CbmTrdGeoHandler* fGeoHandler; //!
    
  /**  map to store digis for pair of x,y position in module **/
  // map<pair<ModuleID,pair<x,y>>, CbmTrdDigi*>
  std::map<std::pair< Int_t, std::pair< Int_t, Int_t > >, CbmTrdDigi* > fDigiMap; 
  /**  iterator over map to store digis for pair of x,y position in module **/
  std::map<std::pair< Int_t, std::pair< Int_t, Int_t > >, CbmTrdDigi* >::iterator fDigiMapIt; //! iterator over array above

  CbmTrdHitRateTest(const CbmTrdHitRateTest&);
  CbmTrdHitRateTest& operator=(const CbmTrdHitRateTest&);

  ClassDef(CbmTrdHitRateTest,2)

};

#endif // CBMTRDHITRATETEST_H
