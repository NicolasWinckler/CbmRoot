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

typedef struct MyPoint
{
  Double_t global_inC[3];
  Double_t global_outC[3];
  Double_t local_inC[3];
  Double_t local_outC[3];
  Int_t Sec_in;
  Int_t Row_in;
  Int_t Col_in;
  Int_t Sec_out;
  Int_t Row_out;
  Int_t Col_out;
  Double_t clusterPosC[3];
  Int_t Sec_cluster;
  Int_t Row_cluster;
  Int_t Col_cluster;

  Double_t global_inLL[3];
  Double_t global_outLL[3];
  Double_t local_inLL[3];
  Double_t local_outLL[3];
  Double_t clusterPosLL[3];

  Double_t deltaR;

  Double_t ELoss;
  Double_t ELossTR;
  Double_t ELossdEdX;

  Float_t alpha;
  Float_t beta;
  Float_t gamma;
} MyPoint;

typedef struct ModulePara
{
  Int_t Station;
  Int_t Layer;
  Int_t moduleId;
  Int_t xPos;
  Int_t yPos;
  Int_t zPos;
  Int_t nCol;
  Int_t nRow;
  Int_t NoSectors;
  
  std::vector<Float_t> SectorSizeX;
  std::vector<Float_t> SectorSizeY;
  std::vector<Float_t> PadSizeX;
  std::vector<Float_t> PadSizeY;
  std::vector<Int_t> SecCol;
  std::vector<Int_t> SecRow;
   
  Float_t ModuleSizeX;
  Float_t ModuleSizeY;
  Float_t ModulePositionX;
  Float_t ModulePositionY;
  Float_t ModulePositionZ;
} ModulePara;

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

  void GetModuleInformationFromDigiPar(Int_t VolumeID);
  void GetModuleInformation();

  void AddDigi(const Int_t pointID, Int_t iCol, Int_t iRow, Int_t nCol, Int_t nRow, Double_t iCharge);

  float GetFloatPositionY(Double_t tempPosY);

  void SplitPathSlices(Bool_t lookup, const Int_t pointID, MyPoint *point, Double_t* PadChargeModule, 
		       Int_t j, Double_t* padW, Double_t* padH );

  void WireQuantisation(/*Double_t *ClusterMLL,*/ MyPoint *point);

  void GetIntegrationArea(TH1F* PadX ,TH1F* PadY);

  void ChargeConservation(MyPoint *point);

  void ClusterMapping(MyPoint *point, Double_t* PadChargeModule);

  void CalcMathieson(Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H);

  void FillMathiesonVector();

  void LookupMathiesonVector(Double_t x_mean, Double_t y_mean, Double_t SliceELoss, Double_t* W, Double_t* H);

  void TransformLL2C(Double_t* LLCoordinate, Double_t* CCoordinate, Double_t* StrucDim);
 
  void TransformC2LL(Double_t* CCoordinate, Double_t* LLCoordinate, Double_t* StrucDim);

  double GetPadHeight(Int_t iSector);

  int GetRow(Double_t tempPosY);

  int GetCol(Double_t tempPosX);

  int GetSector(Double_t tempPosY);

  void GetPadSizeMatrix(MyPoint *point, Double_t* H, Double_t* W, Double_t* padH, Double_t* padW);

  Double_t fRound(Double_t value);

  //void TransformMod2Pad(Double_t* MCoordinate, Double_t* PCoordinate, Double_t* StrucDim);
 
  Int_t Digicounter;
  Double_t fLayerZ[12];
  Double_t fTime;
  Int_t   fModuleType;
  Int_t   fModuleCopy;
  Int_t   fModuleID;//Unique number for detector module
  Int_t   fMCindex;// index to MCPoint

  Double_t tempx;
  Double_t tempy;

  static const Int_t accuracy = 1;// '1/accuracy' integration step width [mm]
  static const Int_t Accuracy = 1000; // fMathieson array accuracy in values per mm
  static const Int_t fPadNrX = 15;//7; // has to be odd
  static const Int_t fPadNrY = 5;//5; // has to be odd
  static const Int_t fNoSectors = 3;
  static const Int_t endOfMathiesonArray = 35; //+- mm
  Double_t fMathieson[endOfMathiesonArray * 1000];//endOfMathiesonArray * Accuracy
  Double_t fModuleSize[3];
  Double_t fPadCharge[fPadNrY][fPadNrX]; //Charge on 3 adjacent pads calculated by using the Mathieson formula
  Float_t fEfficiency; // Digi production efficiency (0-100%)

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

  std::map<Int_t, ModulePara*> fModuleParaMap;

  ClassDef(CbmTrdClusterizer,1)

    };
#endif // CBMTRDCLUSTERIZER_H
