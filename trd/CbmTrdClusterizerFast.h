#ifndef CBMTRDCLUSTERIZERFAST_H
#define CBMTRDCLUSTERIZERFAST_H

#include "CbmTrdDetectorId.h"

#include "FairTask.h"

#include <map>
#include <list>
#include <vector>
#include "CbmTrdDigi.h"

class CbmTrdDigiPar;
//class CbmTrdDigi;
class CbmTrdModule;
class CbmTrdRadiator;

class TClonesArray;
class TH1F;
class TH2F;
class TProfile;

typedef struct ClusterPad
{
  Double_t SizeX;
  Double_t SizeY;
  Double_t Charge;
ClusterPad() : SizeX(0), SizeY(0), Charge(0) {}
} ClusterPad;


typedef struct ClusterModule
{
  Int_t Station;
  Int_t Layer;
  Int_t moduleId;
  Int_t nCol;
  Int_t nRow;
  Int_t NoSectors;
  std::vector<std::vector<ClusterPad*> > PadPlane;
  
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

ModulePara() : Station(-1), Layer(-1), moduleId(-1), PadPlane(),
    nCol(0), nRow(0), NoSectors(0), SectorSizeX(), SectorSizeY(), PadSizeX(), 
    PadSizeY(), SecCol(), SecRow(), ModuleSizeX(0.), ModuleSizeY(0.), 
    ModulePositionX(0.), ModulePositionY(0.), ModulePositionZ() {}
} ClusterModule;

class CbmTrdClusterizerFast : public FairTask {

 public:

  /** Default constructor **/
  CbmTrdClusterizerFast();

  /** Standard constructor **/
  CbmTrdClusterizerFast(const char *name, const char *title="CBM Task",
		    CbmTrdRadiator *radiator=NULL);

  /** Destructor **/
  virtual ~CbmTrdClusterizerFast();

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

  CbmTrdClusterizerFast& operator=(const CbmTrdClusterizerFast&);
  CbmTrdClusterizerFast(const CbmTrdClusterizerFast&);

  void GetModuleInformationFromDigiPar(Int_t VolumeID);
  void GetModuleInformation();

  void AddDigi(const Int_t pointID, Int_t iCol, Int_t iRow, Int_t nCol, Int_t nRow, Double_t iCharge); 

  void SplitPathSlices();

  void WireQuantisation();

  void ChargeConservation();

  Double_t CalcMathieson(Double_t x, Double_t W);

  void TransformLL2C(Double_t* LLCoordinate, Double_t* CCoordinate, Double_t* StrucDim);
 
  void TransformC2LL(Double_t* CCoordinate, Double_t* LLCoordinate, Double_t* StrucDim);

  Int_t Digicounter;
  Double_t fLayerZ[12];
  Double_t fTime;
  Int_t   fModuleType;
  Int_t   fModuleCopy;
  Int_t   fModuleID;//Unique number for detector module
  Int_t   fMCindex;// index to MCPoint

  static const Int_t fPadNrX = 11;//15;//7; // has to be odd
  static const Int_t fPadNrY = 11;//5;//5; // has to be odd
 
  //static const Int_t fNoSectors = 3;
  static const Int_t endOfMathiesonArray = 35; //+- mm

 
 
  Float_t fEfficiency; // Digi production efficiency (0-100%)

  TClonesArray *fTrdPoints; //! Trd MC points
  TClonesArray *fDigiCollection; //! TRD digis
  TClonesArray *fDigiMatchCollection; //! Corresponding MCPoints to TRD digis
  TClonesArray *fMCStacks;  //! MC Track information

  CbmTrdDigiPar  *fDigiPar;    //!
  CbmTrdModule   *fModuleInfo; //!
  CbmTrdRadiator *fRadiators;  //!

  CbmTrdDetectorId fTrdId; //!
 
  std::map<Int_t, ClusterModule*> fModuleClusterMap;
  std::map<Int_t, ClusterModule*>::iterator fModuleClusterMapIt;

  ClassDef(CbmTrdClusterizerFast,1)

    };
#endif // CBMTRDCLUSTERIZERFAST_H
